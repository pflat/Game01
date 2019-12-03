#pragma once

#include "../Core/IncGuards.h"
#include "../Core/ToyboxDefs.h"
#include "../Input/InputState.h"

THIRD_PARTY_GUARDS_BEGIN
#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Graphics/AnimationController.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/CollisionShape.h>
THIRD_PARTY_GUARDS_END


namespace Toybox
{


///  Kinematic character possible states.
enum KinematicState
{
    KCS_STANDING = 1,
    KCS_WALKING,
    KCS_RUNNING,
    KCS_JUMPING,
    KCS_FALLING,
    KCS_SLIDING,
    KCS_LANDING,
    KCS_SWIMMING,
    //  Underwater swimming
    KCS_DIVING
};


///  Kinematic character controller.
class KinematicCharacterCtrl : public Urho3D::LogicComponent
{
    URHO3D_OBJECT(KinematicCharacterCtrl, LogicComponent);

public:
    ///  Construct.
    KinematicCharacterCtrl(Urho3D::Context* context);

    ///  Register object factory and attributes.
    static void RegisterObject(Urho3D::Context* context);

    ///  Handle startup. Called by LogicComponent base class.
    virtual void Start();

    ///  Initialize the component after creation.
    void Init(float forward_speed, float back_speed, float strafe_speed, float jump_speed, float rotation_step,
              float run_mul, float air_speed_mul, float gravity, float slope_factor, float spring_height);

    ///  \todo  Add methods to get/set all previous values individually.

    ///  Returns the camera node. This can be used as a target node for the camera.
    Urho3D::Node* GetCameraNode() { return camera_node_; }

    ///  Returns weather character is on floor (has contact and grip).
    bool OnGround() const { return (state_ == KCS_STANDING || state_ == KCS_WALKING || state_ == KCS_RUNNING); }

    ///  Returns weather character is on the air (either airborne or sliding down a surface).
    bool OnAir() const { return (state_ == KCS_JUMPING || state_ == KCS_FALLING || state_ == KCS_SLIDING); }

    ///  Returns weather character is in water (either on surface or underwater).
    bool OnWater() const { return (state_ == KCS_DIVING || state_ == KCS_SWIMMING); }

    ///  Returns weather character is jumping.
    bool IsJumping() const { return (state_ == KCS_JUMPING); }

    ///  Handle physics world update. Called by LogicComponent base class.
    virtual void FixedUpdate(float time_step);

private:
    ///  Determines the move vector and rotation value, based on the keyboard inputs and character state.
    void ProcessInput(float time_step);

    ///  Performs convex cast against the floor.
    ///  Updates state, ground_dist, ground_level and ground_normal.
    void GroundCheck(const Urho3D::Vector3& pos, const Urho3D::Vector3& velocity, const Urho3D::Vector3& floor_normal = Urho3D::Vector3::UP);

    ///  Try to move the character by the 'vel' vector, stopping if the character touches the floor.
    void Pull(const Urho3D::Vector3& pos, const Urho3D::Vector3& vel, Urho3D::Vector3& vpull);

    ///  Moves the character by the 'vel' vector. If the character collides with any surface, stops movement and slides on that surface.
    ///  At the end of movement, it tries to pull itself to the surface.
    void MoveAndSlide(const Urho3D::Vector3& pos, const Urho3D::Vector3& vel, Urho3D::Vector3& vslided, bool stop_on_floor = false);

    ///  Debug - TO BE REMOVED
    void PrintState();

public:
    ///  Input state class (keyboard and mouse).
    InputState input_;

private:
    ///  Length of the "leg" under the capsule shape.
    float spring_height_;
    ///  Length of the convex cast used to find the floor info.
    const float spring_cast_length_;
    ///  Forward walk speed.
    float forward_speed_;
    ///  Backward walk speed.
    float back_speed_;
    ///  Strafe walk speed.
    float strafe_speed_;
    ///  Jump speed.
    float jump_speed_;
    ///  Rotation speed.
    float rotation_step_;
    ///  Multiplier to apply to velocity. Can be used to disable or toggle the meaning of the "run" key.
    float run_mul_;
    ///  Multiplier to apply to velocity when character is in the air, jumping.
    float air_speed_mul_;
    ///  Gravity to apply to character.
    float gravity_;
    ///  The floor inclination the character can't walk on. It will slide instead.
    float slope_factor_;

    ///  Velocity to apply to the character. Determined from the inputs and from the character state.
    Urho3D::Vector3 velocity_;
    ///  Momentum gained from the inputs. When a jump is performed, it is in the direction of this momentum.
    Urho3D::Vector3 momentum_;
    ///  Rotation to apply to the character. Determined from the inputs.
    float rotation_;
    ///  Time spent in the air. Resets every time character touches the ground
    float air_time_;
    ///  Pull down speed. Gravity is applied to this speed every frame the character is in the air, and is reseted every time the character touches the ground.
    float down_speed_;

    ///  Character state flag. One of the KCS_xxx values.
    int state_;
    ///  Character state flag active in the previous frame. Used to measure air_time and calculate down_velocity.
    int prev_state;
    ///  Distance (from the end of the leg) to the floor. Updated after "leg" cast.
    ///  negative - ground above spring end - character must move up
    ///  positive - ground below spring end - character must move down
    float ground_dist_;
    ///  Ground level. Used to see if character slide. Updated after "leg" cast.
    float ground_level_;
    ///  Normal at the collision point. Updated after "leg" cast.
    Urho3D::Vector3 ground_normal_;

    ///  Cached Urho3D components. Physics world.
    Urho3D::WeakPtr<Urho3D::PhysicsWorld> physics_world_;
    ///  Cached Urho3D components. Character collision shape.
    Urho3D::WeakPtr<Urho3D::CollisionShape> coll_shape_;
    ///  Cached Urho3D components. Animation controller.
    Urho3D::SharedPtr<Urho3D::AnimationController> animation_ctrl_;
    ///  Cached Urho3D components. Camera node. Used by the engine to point the camera at the character.
    Urho3D::SharedPtr<Urho3D::Node> camera_node_;

    //  Debug - TO BE REMOVED
    unsigned frame_counter;

};

}
