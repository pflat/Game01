#include "../Controllers/KinematicCharacterCtrl.h"
#include "../Math/Vector3Ext.h"
#include "../Core/Settings.h"
#include "../Core/Debug.h"

THIRD_PARTY_GUARDS_BEGIN
//#include <Urho3D/IO/MemoryBuffer.h>
//#include <Urho3D/Math/Ray.h>
//#include <Urho3D/Math/MathDefs.h>
//#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/PhysicsEvents.h>

//#include <Bullet/BulletCollision/BroadphaseCollision/btDbvtBroadphase.h>
//#include <Bullet/BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h>
//#include <Bullet/BulletCollision/CollisionDispatch/btInternalEdgeUtility.h>
//#include <Bullet/BulletCollision/CollisionShapes/btBoxShape.h>
//#include <Bullet/BulletCollision/CollisionShapes/btSphereShape.h>
#include <Bullet/BulletCollision/CollisionShapes/btCapsuleShape.h>
//#include <Bullet/BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h>
//#include <Bullet/BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
THIRD_PARTY_GUARDS_END


namespace Toybox
{

KinematicCharacterCtrl::KinematicCharacterCtrl(Urho3D::Context* context) :
    Urho3D::LogicComponent(context),

    spring_cast_length(5.0f),

    gravity_(0.0f),
    jump_strength_(0.0f),
    slope_factor_(Urho3D::Cos(45.0f)),

    capsule_height_(0.0f),
    capsule_diameter_(0.0f),
    spring_height_(0.0f),
    spring_diameter_(0.0f),

    air_time(0.0f),
    down_vel(0.0f),

    ground_velocity(Urho3D::Vector3::ZERO),
    air_velocity(Urho3D::Vector3::ZERO),

//    body(0),
    coll_shape(0),
    animation_ctrl(0),
    camera_node(0),
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


void KinematicCharacterCtrl::Start()
{
    physics_world = node_->GetScene()->GetComponent<Urho3D::PhysicsWorld>();
    animation_ctrl = node_->GetComponent<Urho3D::AnimationController>(true);
    coll_shape = node_->GetComponent<Urho3D::CollisionShape>(true);

    camera_node = node_->GetChild("Camera", true);

    // Component has been inserted into its scene node. Subscribe to events now
}

void KinematicCharacterCtrl::Init(
        float capsule_height, float capsule_diameter,
        float spring_height, float spring_diameter,
        float jump_strength, float gravity)
{
    capsule_height_ = capsule_height;
    capsule_diameter_ = capsule_diameter;
    spring_height_ = spring_height;
    spring_diameter_ = spring_diameter;
    jump_strength_ = jump_strength;
    gravity_ = gravity;
}


bool KinematicCharacterCtrl::MoveAndCollide(
        const Urho3D::Vector3& pos, const Urho3D::Vector3& vel,
        Urho3D::Vector3& vmoved, Urho3D::Vector3& vremain,
        CollisionState& cstate)
{
//PrintVector(vel);
    float vel_length = vel.Length();

    Urho3D::PhysicsRaycastResult raycast_result;
    physics_world->ConvexCast(
                raycast_result,
                coll_shape,
                pos, Urho3D::Quaternion::IDENTITY,
                pos + vel, Urho3D::Quaternion::IDENTITY,
                CL_TERRAIN | CL_STATIC);

    if (raycast_result.body_)  //  shape has collided with another body
    {
        float collison_margin = raycast_result.body_->GetNode()->GetComponent<Urho3D::CollisionShape>()->GetMargin();
        float adjusted_distance = raycast_result.distance_ + collison_margin;
//printf("%d %f %f\n", frame_counter, raycast_result.distance_, collison_margin);
        vmoved = (adjusted_distance / vel_length) * vel;
        vremain = Slide(((vel_length - adjusted_distance) / vel_length) * vel, raycast_result.normal_);

        //  TODO stuff here
        float xx = raycast_result.normal_.DotProduct(Urho3D::Vector3::UP);

        //  Hit a (near) perpendicular surface. No need to slide. Maybe problematic  ??? i don't know yet
        if (xx >= 0.95f)
        {
            vremain = Urho3D::Vector3::ZERO;
            SpringCast(pos, vmoved, cstate);
            return true;
        }
        //  Touched floor or low slope. Can walk
        else if (xx >= slope_factor_)
        {
            SpringCast(pos, vmoved, cstate);
            return false;
        }

        //  Touched wall or high slope. Can't walk. Maybe slide on wall
        else if (xx >= 0.0f && xx < slope_factor_)
        {
//PrintVector(raycast_result.normal_);
//printf("3. %f \n", xx);
            SpringCast(pos, vmoved, cstate);
        }

        //  Touched ceiling. Not yet implemented
        else if (xx < 0.0f)
        {
//printf("4. %f \n", xx);
            SpringCast(pos, vmoved, cstate);
        }

        return false;
    }
    else  //  no hit
    {
        vmoved = vel;
        vremain = Urho3D::Vector3::ZERO;

        if (char_state.OnGround())
        {
            //  Add pull down force
            //vmoved = vmoved + Urho3D::Vector3(0.0f, -1.0f, 0.0f);
        }

        SpringCast(pos, vmoved, cstate);
		if (cstate.state_ == KCS_STANDING)
		{
//printf("%d - VOU TOCAR - ", frame_counter);
//cstate.Print();
		}

        return true;
    }
}


void KinematicCharacterCtrl::SpringCast(const Urho3D::Vector3& pos, CollisionState& state, const Urho3D::Vector3& floor_normal)
{
    SpringCast(pos, Urho3D::Vector3::ZERO, state, floor_normal);
}

/// need to recheck last statements
void KinematicCharacterCtrl::SpringCast(const Urho3D::Vector3& pos, const Urho3D::Vector3& velocity, CollisionState& state, const Urho3D::Vector3& floor_normal)
{
    float ground_dist;
    Urho3D::PhysicsRaycastResult raycast_result;

    int tipo = 1;
    if (tipo == 1)
    {
        //  Raycasts down, with a single ray, from the base of the capsule
        physics_world->RaycastSingle(
                raycast_result,
                Urho3D::Ray(pos + velocity + Urho3D::Vector3(0.0f, spring_height_ , 0.0f), -floor_normal),
                spring_cast_length,
                CL_TERRAIN | CL_STATIC);

        if (raycast_result.body_)
        {
            ground_dist = raycast_result.distance_ - spring_height_;
        }
        else
        {
            ground_dist = Urho3D::M_INFINITY;
        }
    }
    else if (tipo == 2)
    {
        //  Raycasts down, with a sphere, from the base of the capsule minus the sphere radius
        physics_world->SphereCast(
                raycast_result,
                Urho3D::Ray(pos + velocity + Urho3D::Vector3(0.0f, spring_height_ - (spring_diameter_ * 0.5f), 0.0f), -floor_normal),
                spring_diameter_ * 0.5f,
                spring_cast_length,
                CL_TERRAIN | CL_STATIC);

        if (raycast_result.body_)
        {
            float collison_margin = raycast_result.body_->GetNode()->GetComponent<Urho3D::CollisionShape>()->GetMargin();
            ground_dist = raycast_result.distance_ - spring_height_ + spring_diameter_ + collison_margin;
        }
        else
        {
            ground_dist = Urho3D::M_INFINITY;
        }
    }
    else if (tipo == 3)
    {
        //  Raycasts down, with the collision capsule
        physics_world->ConvexCast(
                raycast_result,
                coll_shape,
                pos + velocity, Urho3D::Quaternion::IDENTITY,
                pos + velocity + Urho3D::Vector3(0.0f, -spring_cast_length, 0.0f), Urho3D::Quaternion::IDENTITY,
                CL_TERRAIN | CL_STATIC);

        if (raycast_result.body_)
        {
            float collison_margin = raycast_result.body_->GetNode()->GetComponent<Urho3D::CollisionShape>()->GetMargin();
            ground_dist = raycast_result.distance_ - spring_height_ + collison_margin;
        }
        else
        {
            ground_dist = Urho3D::M_INFINITY;
        }
    }

    state.Update(raycast_result, velocity, ground_dist, spring_height_, spring_diameter_ * 0.5f, slope_factor_);

//    if (char_state.state_ == KCS_JUMPING && state.state_ == KCS_FALLING)
//        state.state_ = KCS_JUMPING;

//    if ((char_state.state_ == KCS_JUMPING || char_state.state_ == KCS_FALLING || char_state.state_ == KCS_SLIDING) && state.state_ == KCS_STANDING)
//        state.state_ = KCS_LANDING;
}


Urho3D::Vector3 KinematicCharacterCtrl::CalcVelocity(const Urho3D::Quaternion& rotation, float time_step)
{
//PrintMessage(frame_counter, "CalcVelocity");
    Urho3D::Vector3 forward_dir = (rotation * Urho3D::Vector3::FORWARD).Normalized();
    Urho3D::Vector3 right_dir = (rotation * Urho3D::Vector3::RIGHT).Normalized();
    Urho3D::Vector3 upward_dir = (rotation * Urho3D::Vector3::UP).Normalized();
    //Urho3D::Vector3 pull_force = Urho3D::Vector3(0.0f, -1.0f, 0.0f);

    Urho3D::Vector3 velocity = Urho3D::Vector3::ZERO;

    //  ground contact
    if (char_state.OnGround())
    {
//char_state.Print();
//printf("%d - CalcVelocity - OnGrounf\n", frame_counter);
        air_time = 0.0f;
        down_vel = 0.0f;
        ground_velocity  = (forward_dir * char_control.forward_vel_) + (right_dir * char_control.strafe_vel_);
        //ground_velocity = ground_velocity + pull_force;
        air_velocity = ground_velocity;
        velocity = ground_velocity * time_step;
    }

    //  landing
    //  dont' expect to ever occur
    //  expect to usually happen in the middle of frame
//    else if (char_state.state_ == KCS_LANDING)
//    {
//        PrintMessage(frame_counter, "LANDIND - SHOULD NOT HAVE HAPPENED HERE !!!");
//        air_time = 0.0f;
//        down_vel = 0.0f;
//        velocity = Urho3D::Vector3::ZERO;
//    }

    //  jumping - should also be used to push the body (explosions, etc)
    else if (char_state.state_ == KCS_JUMPING)
    {
        air_time += time_step;
        down_vel += (gravity_ * time_step);
        velocity = (air_velocity * 1.0f * time_step) + (Urho3D::Vector3::UP * ((jump_strength_ * time_step) + (down_vel * 1.0f)));
    }

    //  falling
    else if (char_state.state_ == KCS_FALLING)
    {
        air_time += time_step;
        down_vel += (gravity_ * time_step);
        velocity = (air_velocity * 0.15f * time_step) + (Urho3D::Vector3::UP * down_vel);
    }

    //  sliding on a surface
    else if (char_state.state_ == KCS_SLIDING)
    {
        air_time += time_step;
        down_vel += (gravity_ * time_step);
        velocity = Slide(Urho3D::Vector3::UP * down_vel, char_state.ground_normal_);
    }

    //  should never happen
    else
    {
        PrintMessage(frame_counter, "VELOCITY - SHOULD NOT HAVE HAPPENED !!!");
        velocity = Urho3D::Vector3::ZERO;
    }

    return velocity;
}


void KinematicCharacterCtrl::Move(Urho3D::Vector3& body_pos, Urho3D::Vector3& velocity)
{
//PrintMessage(frame_counter, "Move");

    Urho3D::Vector3 vmoved, vremain;

/*
    int i=0;
    while (MoveAndCollide(body_pos, velocity, vmoved, vremain, char_state) == false || i < 3)
    {
        i++;
    }
*/
//PrintVector(velocity);
    MoveAndCollide(body_pos, velocity, vmoved, vremain, char_state);
char_state.Print();
    if (char_state.OnGround())
    {
printf("%d - ", frame_counter);
char_state.Print();
//PrintVector(body_pos);
//PrintVector(vmoved);
        node_->SetPosition(body_pos + vmoved - Urho3D::Vector3(0.0f, char_state.ground_dist_, 0.0f));
    }

    if (char_state.OnAir())
    {
//printf("2\n");
        node_->SetPosition(body_pos + vmoved);

    }


    return;
    CollisionState new_state;
    //  ground -> ground
    if (char_state.OnGround() &&  new_state.OnGround())
    {
//PrintMessage(frame_counter, "move - on_ground");
        velocity.y_ = velocity.y_ + new_state.ground_dist_;

        body_pos = body_pos + velocity;
        node_->SetPosition(body_pos);

        char_state = new_state;
        velocity = Urho3D::Vector3::ZERO;

        //animation_ctrl->PlayExclusive("Models/Characters/Julia/Walk.ani", 0, true, 0.2f);
    }

    else if (new_state.state_ == KCS_LANDING)  //  stops for the rest of the frame duration
    {
//PrintMessage(frame_counter, "TODO - move - landing");
        Urho3D::Vector3 vmoved;
        Urho3D::Vector3 vremain;
        CollisionState cstate;
        MoveAndCollide(body_pos, velocity, vmoved, vremain, cstate);
//PrintVector(velocity);
//PrintVector(vmoved);
//PrintVector(vremain);
        //vmoved += Urho3D::Vector3(0.0f, spring_radius, 0.0f);

        body_pos = body_pos + vmoved;
        node_->SetPosition(body_pos);

        char_state = new_state;
        char_state.state_ = KCS_STANDING;
        velocity = Urho3D::Vector3::ZERO;
    }

    //  ground -> too much ground slope
    else if ((char_state.state_ == KCS_STANDING || char_state.state_ == KCS_WALKING || char_state.state_ == KCS_RUNNING) &&
             new_state.state_ == KCS_SLIDING &&
             new_state.move_pitch_ < 0.0f)  //  wall hug missing
    {
//PrintMessage(frame_counter, "Too steep - angle");
        velocity = Urho3D::Vector3::ZERO;
    }

    //  ground -> too much high move height
    else if ((char_state.state_ == KCS_STANDING || char_state.state_ == KCS_WALKING || char_state.state_ == KCS_RUNNING) &&
             new_state.ground_dist_ < Urho3D::M_INFINITY &&
             new_state.ground_dist_ > spring_height_)
    {
//PrintMessage(frame_counter, "Too steep - height");
        velocity = Urho3D::Vector3::ZERO;
    }
/*
    else if (char_state.OnAir() && new_state.state == KCS_SLIDING)  //  sliding (after a jump or fall)
    {
PrintMessage(frame_counter, "TODO - move - sliding after fall");
    }
*/
    else if (new_state.state_ == KCS_JUMPING || new_state.state_ == KCS_FALLING || new_state.state_ == KCS_SLIDING)
    {
//PrintMessage(frame_counter, "move - flying");
//velocity = Urho3D::Vector3(0.0f, -10.0f, 0.0f);
//velocity = Urho3D::Vector3(10.0f, 0.0f, 0.0f);
        Urho3D::Vector3 vmoved;
        Urho3D::Vector3 vremain;
        CollisionState cstate;
        MoveAndCollide(body_pos, velocity, vmoved, vremain, cstate);
//PrintVector(velocity);
//PrintVector(vmoved);
//PrintVector(vremain);
/*
        if (new_state.state == KCS_LANDING)
        {

        }
        else */if (vremain.LengthSquared() > 0.0f)
        {
//PrintMessage(frame_counter, "on air (hit)");
            //vmoved += Urho3D::Vector3(0.0f, spring_radius, 0.0f);
            //vremain += Urho3D::Vector3(0.0f, spring_radius, 0.0f);
//printf("%f %f %f\n", velocity.Length(), vmoved.Length(), vremain.Length());

            body_pos = body_pos + vmoved;
            node_->SetPosition(body_pos);

            char_state = new_state;
            velocity = vremain;

            Move(body_pos, velocity);

            velocity = Urho3D::Vector3::ZERO;
        }
        else
        {
//PrintMessage(frame_counter, "on air (full move)");
            //vmoved += Urho3D::Vector3(0.0f, spring_radius, 0.0f);

            body_pos = body_pos + vmoved;
            //body->SetPosition(body_pos);
            node_->SetPosition(body_pos);

            char_state = new_state;
            velocity = Urho3D::Vector3::ZERO;
        }
    }

    else
    {
        PrintMessage(frame_counter, "move - UNDEFINED");
        velocity = Urho3D::Vector3::ZERO;
    }
}


void KinematicCharacterCtrl::FixedUpdate(float time_step)
{
//PrintMessage(frame_counter, "FixedUpdate");
    Urho3D::Vector3 pos = node_->GetPosition();
    //const char* WALK_ANI = "Models/Characters/Julia/Walk.ani";

    char_control.Update(&input);

    node_->SetRotation(Urho3D::Quaternion(0.0f, char_control.rotation_, 0.0f));
/*
    if (input.IsKeyDown(KM_CHARACTER_JUMP))
    {
        if ((char_state.state_ == KCS_STANDING || char_state.state_ == KCS_WALKING || char_state.state_ == KCS_RUNNING ) && air_time < 0.2f)
        {
            char_state.state_ = KCS_JUMPING;
        }
    }
*/
    Urho3D::Vector3 velocity = CalcVelocity(node_->GetRotation(), time_step);
//PrintVector(velocity);
    Move(pos, velocity);

    ++frame_counter;
}

}
