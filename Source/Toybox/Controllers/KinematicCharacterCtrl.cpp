#include "../Controllers/KinematicCharacterCtrl.h"
#include "../Math/Vector3Ext.h"
#include "../Core/Settings.h"
//#include "../Core/Debug.h"

THIRD_PARTY_GUARDS_BEGIN
//  ...
THIRD_PARTY_GUARDS_END


namespace Toybox
{

KinematicCharacterCtrl::KinematicCharacterCtrl(Urho3D::Context* context) :
    Urho3D::LogicComponent(context),

    spring_height_(0.0f),
    spring_cast_length_(5.0f),
    gravity_(0.0f),

    forward_speed_(0.0f),
    back_speed_(0.0f),
    strafe_speed_(0.0f),
    jump_speed_(0.0f),
    rotation_step_(0.0f),
    run_mul_(0.0f),
    air_speed_mul_(0.0f),
    slope_factor_(0.0f),
    //forward_speed_(0.0f),
    //strafe_speed_(0.0f),
    rotation_(0.0f),
    //jump_speed_(0.0f),
    air_time_(0.0f),
    down_vel_(0.0f),
    momentum_(Urho3D::Vector3::ZERO),
    frame_counter(0)
{
    SetUpdateEventMask(Urho3D::USE_FIXEDUPDATE);
}


void KinematicCharacterCtrl::RegisterObject(Urho3D::Context* context)
{
    context->RegisterFactory<KinematicCharacterCtrl>();

    // These macros register the class attributes to the Context for automatic load / save handling.
    // We specify the Default attribute mode which means it will be used both for saving into file, and network replication
//    URHO3D_ATTRIBUTE("Controls Yaw", float, controls_.yaw_, 0.0f, AM_DEFAULT);
//    URHO3D_ATTRIBUTE("Controls Pitch", float, controls_.pitch_, 0.0f, AM_DEFAULT);
//    URHO3D_ATTRIBUTE("On Ground", bool, onGround_, false, AM_DEFAULT);
//    URHO3D_ATTRIBUTE("OK To Jump", bool, okToJump_, true, AM_DEFAULT);
//    URHO3D_ATTRIBUTE("In Air Timer", float, inAirTimer_, 0.0f, AM_DEFAULT);
}

/*
void KinematicCharacterCtrl::Start()
{
    //  Cache engine components, in order to avoid getting them every frame.
    physics_world = node_->GetScene()->GetComponent<Urho3D::PhysicsWorld>();
    animation_ctrl = node_->GetComponent<Urho3D::AnimationController>(true);
    coll_shape = node_->GetComponent<Urho3D::CollisionShape>(true);

    camera_node = node_->GetChild("Camera", true);

    //  Component has been inserted into its scene node. Subscribe to events now
    //  ...
}
*/
void KinematicCharacterCtrl::Init(float spring_height)
{
    spring_height_ = spring_height;

    //  Cache engine components, in order to avoid getting them every frame.
    physics_world_ = node_->GetScene()->GetComponent<Urho3D::PhysicsWorld>();
    animation_ctrl_ = node_->GetComponent<Urho3D::AnimationController>(true);
    coll_shape_ = node_->GetComponent<Urho3D::CollisionShape>(true);

    camera_node_ = node_->GetChild("Camera", true);
}


void KinematicCharacterCtrl::Init(float forward_speed, float back_speed, float strafe_speed, float rotation_step,
                                  float jump_speed, float run_mul, float air_speed_mul,
                                  float gravity, float slope_factor)
{
    forward_speed_ = forward_speed;
    back_speed_ = back_speed;
    strafe_speed_ = strafe_speed;
    rotation_step_ = rotation_step;
    jump_speed_ = jump_speed;
    run_mul_ = run_mul;
    air_speed_mul_ = air_speed_mul;
    gravity_ = gravity;
    slope_factor_ = slope_factor;
}


void KinematicCharacterCtrl::Pull(const Urho3D::Vector3& pos, const Urho3D::Vector3& vel, Urho3D::Vector3& vpulled)
{
    MoveAndSlide(pos, vel, vpulled, char_state, true);
}


void KinematicCharacterCtrl::MoveAndSlide(
        const Urho3D::Vector3& pos, const Urho3D::Vector3& vel,
        Urho3D::Vector3& vm, CollisionState& cstate,
        bool stop_on_floor)
{
    Urho3D::Vector3 vwanted = vel;
    Urho3D::Vector3 vmoved;
    Urho3D::Vector3 vremain;
    vm = Urho3D::Vector3::ZERO;

    bool on_floor = false;
    bool on_wall = false;
    bool on_ceiling = false;

    bool move_completed = false;
    int slide_counter = 0;
    while (move_completed == false && slide_counter < 3)
    {
        float vel_length = vwanted.Length();

        //  If intended movement is too small, we can probably get away without moving at all.
        if (vel_length < 0.0001f)
        {
            move_completed = true;
            continue;
        }

        Urho3D::PhysicsRaycastResult raycast_result;
        physics_world_->ConvexCast(
                raycast_result,
                coll_shape_,
                pos + vm, Urho3D::Quaternion::IDENTITY,
                pos + vm + vwanted, Urho3D::Quaternion::IDENTITY,
                CL_TERRAIN | CL_STATIC);

        //  Shape has collided with another body
        if (raycast_result.body_)
        {
            float fvel = raycast_result.normal_.DotProduct(vwanted.Normalized());
            float fground = raycast_result.normal_.DotProduct(Urho3D::Vector3::UP);

            vmoved = (raycast_result.distance_ / vel_length) * vwanted;
            vremain = Toybox::Project(((vel_length - raycast_result.distance_) / vel_length) * vwanted, raycast_result.normal_);

            //  Tiny :) hack. We move the character away from the collided body by a small amount.
            //  Sometimes vremain vector would collide again, in the same spot.
            //  This way we try to avoid that. If necessary increase amount very slightly.
            vmoved = vmoved + (raycast_result.normal_ * 0.001f);


            //  Touched wall. Slide along movement vector.
            if (fground >= -0.05f && fground < 0.05f)
            {
                on_wall = true;
            }

            //  Touched high sloped ceiling.
            else if (fground >= -slope_factor_ && fground < -0.05f)
            {
                on_ceiling = true;
            }

            //  Touched ceiling.
            else if (fground < -slope_factor_)
            {
                on_ceiling = true;
                //cstate.state_ = KCS_FALLING;
                //move_completed = true;
            }
        }

        //  No collision detected.
        else
        {
            vmoved = vwanted;
            vremain = Urho3D::Vector3::ZERO;
            move_completed = true;
        }

        vm += vmoved;
//        body_pos += vmoved;
        vwanted = vremain;
        slide_counter++;

        //  In the end of normal movement stage it was necessary to adjust the character.
        //  This block tries to pull character up/down.
        if (stop_on_floor == true)
        {
            CollisionState cs;
            GroundCheck(pos, vm, cs);
            if (cs.ground_dist_ >= -0.001f && cs.ground_dist_ <= 0.001f)
            {
                move_completed = true;
            }

            //cstate = cs;
        }
    }

    //  Normal movement stage has ended. Check if special care is needed.
    if (stop_on_floor == false)
    {
        CollisionState cs;
        GroundCheck(pos, vm, cs);

        //  Started climbing a high slope. Push character down.
        if (cs.ground_level_ >= 0.05f && cs.ground_level_ < slope_factor_)
        {
            Urho3D::Vector3 vpulled;
            Pull(pos + vm, Urho3D::Vector3(0.0f, -0.4f, 0.0f), vpulled);
            vm += vpulled;
        }

        //  Collided with high sloped ceiling.
        //  If this happens while going up a slope, the character could bury it's feet, or start traversing wall.
        //  Try to pull character up, along the wall/ceiling, to stay above ground.
        else if (on_ceiling)
        {
            Urho3D::Vector3 vpulled;
            Pull(pos + vm, Urho3D::Vector3(0.0f, Urho3D::Abs(cs.ground_dist_), 0.0f) , vpulled);
            vm += vpulled;
        }

        //cstate = cs;
    }

    GroundCheck(pos, vm, cstate);
}


void KinematicCharacterCtrl::GroundCheck(const Urho3D::Vector3& pos, const Urho3D::Vector3& velocity, CollisionState& state, const Urho3D::Vector3& floor_normal)
{
    //  Raycasts down, with the collision capsule
    Urho3D::PhysicsRaycastResult raycast_result;
    physics_world_->ConvexCast(
            raycast_result,
            coll_shape_,
            pos + velocity, Urho3D::Quaternion::IDENTITY,
            pos + velocity + Urho3D::Vector3(0.0f, -spring_cast_length_, 0.0f), Urho3D::Quaternion::IDENTITY,
            CL_TERRAIN | CL_STATIC);

    state.Update(raycast_result, velocity, spring_height_, slope_factor_);
}


void KinematicCharacterCtrl::ProcessInput(float time_step)
{
    //  1st stage - calculate movement direction from inputs only.
    //  In this stage, velocity vector is treated as a direction only (it will be normalized in the end).
    //  Forward / backward.
    if (input.IsKeyDown(KM_CHARACTER_FORWARD))
    {
        velocity_.z_ = 1.0f;
    }
    else if (input.IsKeyDown(KM_CHARACTER_BACKWARD))
    {
        velocity_.z_ = -1.0f;
    }
    else
    {
        velocity_.z_ = 0.0f;
    }

    //  Strafe.
    if (input.IsKeyDown(KM_CHARACTER_STRAFE_LEFT))
    {
        velocity_.x_ = -1.0f;
    }
    else if (input.IsKeyDown(KM_CHARACTER_STRAFE_RIGHT))
    {
        velocity_.x_ = 1.0f;
    }
    else
    {
        velocity_.x_ = 0.0f;
    }

    //  2nd stage. Determine angle of direction vector.
    float angle = velocity_.Normalized().Angle(Urho3D::Vector3::RIGHT);
    if (velocity_.z_ < 0.0f)
    {
        angle *= -1;
    }

    //  3rd stage. Determine the velocity vector, still only based on the inputs.
    //  We used the angle obtained in stage 2, and the speeds the character can move on
    //  the different axis.
    //  If the character moved at the same speed in all axis, this would not be necessary.
    //  Forward / backward.
    if (input.IsKeyDown(KM_CHARACTER_FORWARD))
    {
        velocity_.z_ = Urho3D::Sin(angle) * forward_speed_;
    }
    else if (input.IsKeyDown(KM_CHARACTER_BACKWARD))
    {
        velocity_.z_ = Urho3D::Sin(angle) * back_speed_;
    }
    else
    {
        velocity_.z_ = 0.0f;
    }

    //  Strafe.
    if (input.IsKeyDown(KM_CHARACTER_STRAFE_LEFT) || input.IsKeyDown(KM_CHARACTER_STRAFE_RIGHT))
    {
        velocity_.x_ = Urho3D::Cos(angle) * strafe_speed_;
    }
    else
    {
        velocity_.x_ = 0.0f;
    }

    //  Jump.
    if (input.IsKeyDown(KM_CHARACTER_JUMP))
    {
        if (char_state.OnGround() || air_time_ < 0.2f)
        {
            velocity_.y_ = jump_speed_;
            char_state.state_ = KCS_JUMPING;
        }
    }
    else
    {
        velocity_.y_ = 0.0f;
    }

    //  Walk/run modifier.
    if (input.IsKeyDown(KM_CHARACTER_WALK))
    {
        velocity_.z_ *= run_mul_;
        velocity_.x_ *= run_mul_;
    }

    //  Rotation.
    if (input.IsKeyDown(KM_CHARACTER_TURN_LEFT))
    {
        rotation_ -= rotation_step_;
        if (rotation_ < -180)
            rotation_ = 360.0f + rotation_;
    }
    else if (input.IsKeyDown(KM_CHARACTER_TURN_RIGHT))
    {
        rotation_ += rotation_step_;
        if (rotation_ > 180)
            rotation_ = -360.0f + rotation_;
    }

    //  4th stage. Determines final velocity vector, based on character state.
    //  In this stage, if character is jumping, forward and strafe movement is not used.
    //  If it's walking, gravity is not applied. And so on...
    //  Ground contact.
    if (char_state.OnGround())
    {
        Urho3D::Quaternion rot = Urho3D::Quaternion(0.0f, rotation_, 0.0f);
        Urho3D::Vector3 forward_dir = (rot * Urho3D::Vector3::FORWARD).Normalized();
        Urho3D::Vector3 right_dir = (rot * Urho3D::Vector3::RIGHT).Normalized();

        momentum_ = (forward_dir * velocity_.z_) + (right_dir * velocity_.x_);
        velocity_ = momentum_ * time_step;
    }

    //  Jumping - also used to push the body (explosions, etc).
    else if (char_state.state_ == KCS_JUMPING)
    {
        velocity_ = (momentum_ * air_speed_mul_ * time_step) + (Urho3D::Vector3::UP * ((jump_speed_ * time_step) + down_vel_));
        //velocity_ = (momentum_ * air_speed_mul_ * time_step) + (Urho3D::Vector3::UP * ((velocity_.y_ * time_step) + down_vel));
    }

    //  Falling.
    else if (char_state.state_ == KCS_FALLING)
    {
        velocity_ = (momentum_ * air_speed_mul_ * time_step) + (Urho3D::Vector3::UP * down_vel_);
    }

    //  Sliding down on a surface.
    else if (char_state.state_ == KCS_SLIDING)
    {
        velocity_ = Toybox::Slide(Urho3D::Vector3::UP * down_vel_, char_state.ground_normal_);
    }

    //  Error - Should never happen.
    else
    {
        velocity_ = Urho3D::Vector3::ZERO;
    }
}


void KinematicCharacterCtrl::Move(Urho3D::Vector3& body_pos, float time_step)
{
    Urho3D::Vector3 vmoved;
    MoveAndSlide(body_pos, velocity_, vmoved, char_state);
    body_pos = body_pos + vmoved;

    if (char_state.OnGround())
    {
        node_->SetPosition(body_pos - Urho3D::Vector3(0.0f, char_state.ground_dist_, 0.0f));
        node_->SetRotation(Urho3D::Quaternion(0.0f, rotation_, 0.0f));
        air_time_ = 0.0f;
        down_vel_ = 0.0f;
    }
    else if (char_state.OnAir())
    {
        node_->SetPosition(body_pos);
        node_->SetRotation(Urho3D::Quaternion(0.0f, rotation_, 0.0f));
        air_time_ += time_step;
        down_vel_ += (gravity_ * time_step);
    }
}


void KinematicCharacterCtrl::FixedUpdate(float time_step)
{
    Urho3D::Vector3 pos = node_->GetPosition();
    //const char* WALK_ANI = "Models/Characters/Julia/Walk.ani";

    ProcessInput(time_step);
    Move(pos, time_step);

    PrintState();
    prev_char_state = char_state;
    ++frame_counter;
}


void KinematicCharacterCtrl::PrintState()
{
    if (char_state.state_ != prev_char_state.state_)
    {
        printf("%d - State changed to ", frame_counter);
        switch (char_state.state_)
        {
            case KCS_STANDING :
                printf("KCS_STANDING | ");
                break;
            case KCS_WALKING :
                printf("KCS_WALKING | ");
                break;
            case KCS_RUNNING :
                printf("KCS_RUNNING |");
                break;
            case KCS_JUMPING :
                printf("KCS_JUMPING | ");
                break;
            case KCS_FALLING :
                printf("KCS_FALLING | ");
                break;
            case KCS_SLIDING :
                printf("KCS_SLIDING | ");
                break;
            case KCS_LANDING :
                printf("KCS_LANDING | ");
                break;
            case KCS_SWIMMING :
                printf("KCS_SWIMMING | ");
                break;
            case KCS_DIVING :
                printf("KCS_DIVING | ");
                break;
            default:
                printf("UNKNOWN | ");
                break;
        }
        printf("%.6f ", air_time_);
        printf("\n");
    }

}

}
