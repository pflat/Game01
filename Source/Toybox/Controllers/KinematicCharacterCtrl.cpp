#include "../Controllers/KinematicCharacterCtrl.h"
#include "../Math/Vector3Ext.h"
#include "../Core/ToyboxDefs.h"
//#include "../Core/Debug.h"

THIRD_PARTY_GUARDS_BEGIN
//  ...
THIRD_PARTY_GUARDS_END


namespace Toybox
{

KinematicCharacterCtrl::KinematicCharacterCtrl(Urho3D::Context* context) :
    Urho3D::LogicComponent(context),

    spring_height_(0.5f),
    spring_cast_length_(5.0f),

    forward_speed_(5.0f),
    back_speed_(2.0f),
    strafe_speed_(3.0f),
    jump_speed_(10.0f),
    rotation_step_(20.0f),
    run_mul_(2.0f),
    air_speed_mul_(1.0f),
    gravity_(-9.81f),
    slope_factor_(0.707f),

    velocity_(Urho3D::Vector3::ZERO),
    momentum_(Urho3D::Vector3::ZERO),
    rotation_(0.0f),
    air_time_(0.0f),
    down_speed_(0.0f),

    state_(KCS_FALLING),
    prev_state(KCS_FALLING),
    ground_dist_(0.0f),
    ground_level_(1.0f),
    ground_normal_(Urho3D::Vector3::UP),

    frame_counter(0)
{
    SetUpdateEventMask(Urho3D::USE_FIXEDUPDATE);
}


void KinematicCharacterCtrl::RegisterObject(Urho3D::Context* context)
{
    context->RegisterFactory<KinematicCharacterCtrl>();

    // These macros register the class attributes to the Context for automatic load / save handling.
    // We specify the Default attribute mode which means it will be used both for saving into file, and network replication
    //URHO3D_ATTRIBUTE("Controls Yaw", float, controls_.yaw_, 0.0f, AM_DEFAULT);
    //URHO3D_ATTRIBUTE("Controls Pitch", float, controls_.pitch_, 0.0f, AM_DEFAULT);
    //URHO3D_ATTRIBUTE("On Ground", bool, onGround_, false, AM_DEFAULT);
    //URHO3D_ATTRIBUTE("OK To Jump", bool, okToJump_, true, AM_DEFAULT);
    //URHO3D_ATTRIBUTE("In Air Timer", float, inAirTimer_, 0.0f, AM_DEFAULT);
}


void KinematicCharacterCtrl::Start()
{
    //  Cache engine components, in order to avoid getting them every frame.
    physics_world_ = node_->GetScene()->GetComponent<Urho3D::PhysicsWorld>();
    coll_shape_ = node_->GetComponent<Urho3D::CollisionShape>(true);
    animation_ctrl_ = node_->GetComponent<Urho3D::AnimationController>(true);

    camera_node_ = node_->GetChild("Camera", true);

    //  Component has been inserted into its scene node. Subscribe to events now
    //  ...
}


void KinematicCharacterCtrl::Init(float forward_speed, float back_speed, float strafe_speed, float jump_speed, float rotation_step,
                                  float run_mul, float air_speed_mul, float gravity, float slope_factor, float spring_height)
{
    forward_speed_ = forward_speed;
    back_speed_ = back_speed;
    strafe_speed_ = strafe_speed;
    jump_speed_ = jump_speed;
    rotation_step_ = rotation_step;
    run_mul_ = run_mul;
    air_speed_mul_ = air_speed_mul;
    gravity_ = gravity;
    slope_factor_ = slope_factor;
    spring_height_ = spring_height;
}


void KinematicCharacterCtrl::ProcessInput(float time_step)
{
    //  1st stage - calculate movement direction from inputs only.
    //  In this stage, velocity vector is treated as a direction only (it will be normalized in the end).
    //  Forward / backward.
    if (controls_.IsDown(KM_CHARACTER_FORWARD))
    {
        velocity_.z_ = 1.0f;
    }
    else if (controls_.IsDown(KM_CHARACTER_BACKWARD))
    {
        velocity_.z_ = -1.0f;
    }
    else
    {
        velocity_.z_ = 0.0f;
    }

    //  Strafe.
    if (controls_.IsDown(KM_CHARACTER_STRAFE_LEFT))
    {
        velocity_.x_ = -1.0f;
    }
    else if (controls_.IsDown(KM_CHARACTER_STRAFE_RIGHT))
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
    if (controls_.IsDown(KM_CHARACTER_FORWARD))
    {
        velocity_.z_ = Urho3D::Sin(angle) * forward_speed_;
    }
    else if (controls_.IsDown(KM_CHARACTER_BACKWARD))
    {
        velocity_.z_ = Urho3D::Sin(angle) * back_speed_;
    }
    else
    {
        velocity_.z_ = 0.0f;
    }

    //  Strafe.
    if (controls_.IsDown(KM_CHARACTER_STRAFE_LEFT) || controls_.IsDown(KM_CHARACTER_STRAFE_RIGHT))
    {
        velocity_.x_ = Urho3D::Cos(angle) * strafe_speed_;
    }
    else
    {
        velocity_.x_ = 0.0f;
    }

    //  Jump.
    if (controls_.IsDown(KM_CHARACTER_JUMP))
    {
        if (OnGround() || air_time_ < 0.2f)
        {
            velocity_.y_ = jump_speed_;
            state_ = KCS_JUMPING;
        }
    }
    else
    {
        velocity_.y_ = 0.0f;
    }

    //  Walk/run modifier.
    if (controls_.IsDown(KM_CHARACTER_WALK))
    {
        velocity_.z_ *= run_mul_;
        velocity_.x_ *= run_mul_;
    }

    //  Rotation.
    if (controls_.IsDown(KM_CHARACTER_TURN_LEFT))
    {
        rotation_ -= rotation_step_;
        if (rotation_ < -180)
            rotation_ = 360.0f + rotation_;
    }
    else if (controls_.IsDown(KM_CHARACTER_TURN_RIGHT))
    {
        rotation_ += rotation_step_;
        if (rotation_ > 180)
            rotation_ = -360.0f + rotation_;
    }

    //  4th stage. Determines final velocity vector, based on character state.
    //  In this stage, if character is jumping, forward and strafe movement is not used.
    //  If it's walking, gravity is not applied. And so on...
    //  Ground contact.
    if (OnGround())
    {
        Urho3D::Quaternion rot = Urho3D::Quaternion(0.0f, rotation_, 0.0f);
        Urho3D::Vector3 local_x = (rot * Urho3D::Vector3::RIGHT).Normalized();
        Urho3D::Vector3 local_z = (rot * Urho3D::Vector3::FORWARD).Normalized();

        momentum_ = (local_z * velocity_.z_) + (local_x * velocity_.x_);
        velocity_ = momentum_ * time_step;
    }

    //  Jumping - also used to push the body (explosions, etc).
    else if (state_ == KCS_JUMPING)
    {
        velocity_ = (momentum_ * air_speed_mul_ * time_step) + (Urho3D::Vector3::UP * ((jump_speed_ * time_step) + down_speed_));
        //velocity_ = (momentum_ * air_speed_mul_ * time_step) + (Urho3D::Vector3::UP * ((velocity_.y_ * time_step) + down_speed_));
    }

    //  Falling.
    else if (state_ == KCS_FALLING)
    {
        velocity_ = (momentum_ * air_speed_mul_ * time_step) + (Urho3D::Vector3::UP * down_speed_);
    }

    //  Sliding down on a surface.
    else if (state_ == KCS_SLIDING)
    {
        velocity_ = Toybox::Slide(Urho3D::Vector3::UP * down_speed_, ground_normal_);
    }

    //  Error - Should never happen.
    else
    {
        velocity_ = Urho3D::Vector3::ZERO;
    }
}


void KinematicCharacterCtrl::GroundCheck(const Urho3D::Vector3& pos, const Urho3D::Vector3& velocity, const Urho3D::Vector3& floor_normal)
{
    //  Raycasts down, with the collision capsule
    Urho3D::PhysicsRaycastResult raycast_result;
    physics_world_->ConvexCast(
            raycast_result,
            coll_shape_,
            pos + velocity, Urho3D::Quaternion::IDENTITY,
            pos + velocity + Urho3D::Vector3(0.0f, -spring_cast_length_, 0.0f), Urho3D::Quaternion::IDENTITY,
            CL_TERRAIN | CL_STATIC);


    if (raycast_result.body_)
        ground_dist_ = raycast_result.distance_ - spring_height_;
    else
        ground_dist_ = Urho3D::M_INFINITY;

    ground_normal_ = raycast_result.normal_;
    ground_level_ = raycast_result.normal_.DotProduct(Urho3D::Vector3::UP);

    float hysteresis = 0.0001f;
    if (ground_dist_ > -(spring_height_ + hysteresis) && ground_dist_ < (spring_height_ + hysteresis))
    {
        //  Small hack to allow character liftoff. When starting a jump, if the amount moved isn't enough
        //  to get some distance from the floor the character would stay on the ground.
        if (state_ == KCS_JUMPING && velocity_.y_ >= 0.0f)
            state_ = KCS_JUMPING;
        else if (ground_level_ < slope_factor_)
            state_ = KCS_SLIDING;
        else
            state_ = KCS_STANDING;
    }
    else
    {
        if (state_ != KCS_JUMPING)
            state_ = KCS_FALLING;
    }
}


void KinematicCharacterCtrl::Pull(const Urho3D::Vector3& pos, const Urho3D::Vector3& vel, Urho3D::Vector3& vpulled)
{
    MoveAndSlide(pos, vel, vpulled, true);
}


void KinematicCharacterCtrl::MoveAndSlide(const Urho3D::Vector3& pos, const Urho3D::Vector3& vel, Urho3D::Vector3& vslided, bool stop_on_floor)
{
    Urho3D::Vector3 vwanted = vel;
    Urho3D::Vector3 vmoved;
    Urho3D::Vector3 vremain;
    vslided = Urho3D::Vector3::ZERO;

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
                pos + vslided, Urho3D::Quaternion::IDENTITY,
                pos + vslided + vwanted, Urho3D::Quaternion::IDENTITY,
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

        vslided += vmoved;
//        body_pos += vmoved;
        vwanted = vremain;
        slide_counter++;

        //  In the end of normal movement stage it was necessary to adjust the character.
        //  This block tries to pull character up/down.
        if (stop_on_floor == true)
        {
            GroundCheck(pos, vslided);
            if (ground_dist_ >= -0.001f && ground_dist_ <= 0.001f)
            {
                move_completed = true;
            }
        }
    }

    //  Normal movement stage has ended. Check if special care is needed.
    if (stop_on_floor == false)
    {
        GroundCheck(pos, vslided);

        //  Started climbing a high slope. Push character down.
        if (ground_level_ >= 0.05f && ground_level_ < slope_factor_)
        {
            Urho3D::Vector3 vpulled;
            Pull(pos + vslided, Urho3D::Vector3(0.0f, -0.4f, 0.0f), vpulled);
            vslided += vpulled;
        }

        //  Collided with high sloped ceiling.
        //  If this happens while going up a slope, the character could bury it's feet, or start traversing wall.
        //  Try to pull character up, along the wall/ceiling, to stay above ground.
        else if (on_ceiling)
        {
            Urho3D::Vector3 vpulled;
            Pull(pos + vslided, Urho3D::Vector3(0.0f, Urho3D::Abs(ground_dist_), 0.0f) , vpulled);
            vslided += vpulled;
        }
    }

    //  In case of probles try uncommenting this line.
    //GroundCheck(pos, vm, cstate);
}


void KinematicCharacterCtrl::FixedUpdate(float time_step)
{
    Urho3D::Vector3 pos = node_->GetPosition();
    //const char* WALK_ANI = "Models/Characters/Julia/Walk.ani";

    //  Prepare movement parameters.
    ProcessInput(time_step);

    //  Perfrm the movement.
    Urho3D::Vector3 vmoved;
    MoveAndSlide(pos, velocity_, vmoved);
    pos += vmoved;

    if (OnGround())
    {
        node_->SetPosition(pos - Urho3D::Vector3(0.0f, ground_dist_, 0.0f));
        node_->SetRotation(Urho3D::Quaternion(0.0f, rotation_, 0.0f));
        if (prev_state == KCS_STANDING)
        {
            air_time_ = 0.0f;
            down_speed_ = 0.0f;
        }
        else
        {
            air_time_ += time_step;
            down_speed_ += (gravity_ * time_step);
        }

    }
    else if (OnAir())
    {
        node_->SetPosition(pos);
        node_->SetRotation(Urho3D::Quaternion(0.0f, rotation_, 0.0f));
        air_time_ += time_step;
        down_speed_ += (gravity_ * time_step);
    }

    //  Debug - TO BE REMOVED
    //PrintState();
    prev_state = state_;
    ++frame_counter;
}


void KinematicCharacterCtrl::PrintState()
{
    if (state_ != prev_state)
    {
        printf("%d - State changed to ", frame_counter);
        switch (state_)
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
