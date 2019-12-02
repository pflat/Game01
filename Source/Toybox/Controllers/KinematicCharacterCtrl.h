#pragma once

#include "../Core/IncGuards.h"
#include "../Core/Settings.h"
#include "../Input/InputState.h"
#include "../Controllers/KinematicCharState.h"

THIRD_PARTY_GUARDS_BEGIN
#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Graphics/AnimationController.h>
#include <Urho3D/Physics/RigidBody.h>

#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/Math/Ray.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/PhysicsEvents.h>
#include <Urho3D/Physics/CollisionShape.h>
THIRD_PARTY_GUARDS_END


namespace Toybox
{

/// Kinematic character controller.
class KinematicCharacterCtrl : public Urho3D::LogicComponent
{
    URHO3D_OBJECT(KinematicCharacterCtrl, LogicComponent);

    InputState input;
    CollisionState char_state;

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
    ///  Gravity to apply to character.
    float gravity_;
    ///  Multiplier to apply to velocity. Can be used to disable or toggle the meaning of the "run" key.
    float run_mul_;
    ///  Multiplier to apply to velocity when character is in the air, jumping.
    float air_speed_mul_;
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
    float down_vel_;

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
    CollisionState prev_char_state;

public:
    ///  Construct.
    KinematicCharacterCtrl(Urho3D::Context* context);

    ///  Register object factory and attributes.
    static void RegisterObject(Urho3D::Context* context);

    ///  Perform fixed step pre-update.
    void FixedUpdate(float time_step);

    ///  Initialize the component after creation.
    void Init(float spring_height);
    void Init(float forward_speed, float back_speed, float strafe_speed, float rotation_step,
              float jump_speed, float run_mul, float air_speed_mul,
              float gravity, float slope_factor);

    Urho3D::Node* GetCameraNode() { return camera_node_; }

private:

    void MoveAndSlide(const Urho3D::Vector3& pos, const Urho3D::Vector3& vel,
                        Urho3D::Vector3& vm, CollisionState& cstate,
                        bool stop_on_floor = false);

    void GroundCheck(const Urho3D::Vector3& pos, const Urho3D::Vector3& velocity, CollisionState& cstate, const Urho3D::Vector3& floor_normal = Urho3D::Vector3::UP);

    void Pull(const Urho3D::Vector3& pos, const Urho3D::Vector3& vel, Urho3D::Vector3& vpull);

    void ProcessInput(float time_step);

    void Move(Urho3D::Vector3& body_pos, float time_step);

    ///  Debug - TO BE REMOVED
    void PrintState();
};

}
