#pragma once

#include "../Core/IncGuards.h"
#include "../Core/Settings.h"
#include "../Input/InputState.h"
#include "../Controllers/KinematicCharState.h"
#include "../Controllers/KinematicCharControls.h"

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

class KinematicCharacterCtrl : public Urho3D::LogicComponent
{
    URHO3D_OBJECT(KinematicCharacterCtrl, LogicComponent);

    InputState input;
    KinematicCharControls char_control;
    CollisionState char_state;

//  xxx
    const float spring_cast_length;
//  end xxx
//  constants
    float gravity_;
    float jump_strength_;
    float slope_factor_;
//  end
//  collision shape settings
    float capsule_height_;
    float capsule_diameter_;
    float spring_height_;
    float spring_diameter_;
//  end collision settings


    float air_time;
    float down_vel;
    //Urho3D::Vector3 walk_dir;
    //Urho3D::Vector3 jump_dir;
    //Urho3D::Vector3 fall_dir;

    Urho3D::Vector3 ground_velocity;
    Urho3D::Vector3 air_velocity;

/*
//  cached urho3d components
    Urho3D::WeakPtr<Urho3D::PhysicsWorld> physics_world;
//    Urho3D::WeakPtr<Urho3D::RigidBody> body;
    Urho3D::WeakPtr<Urho3D::CollisionShape> coll_shape;
    Urho3D::SharedPtr<Urho3D::AnimationController> animation_ctrl;
    Urho3D::WeakPtr<Urho3D::Node> camera_node;
//  end of cached
*/
    Urho3D::WeakPtr<Urho3D::PhysicsWorld> physics_world;
//    Urho3D::WeakPtr<Urho3D::RigidBody> body;
    Urho3D::WeakPtr<Urho3D::CollisionShape> coll_shape;
    Urho3D::SharedPtr<Urho3D::AnimationController> animation_ctrl;
    Urho3D::SharedPtr<Urho3D::Node> camera_node;

//  debug - TO BE REMOVED
    unsigned frame_counter;
//  end debug

public:
    KinematicCharacterCtrl(Urho3D::Context* context);

    static void RegisterObject(Urho3D::Context* context);

    virtual void Start();

    virtual void FixedUpdate(float time_step);

    //void Init(float shape_height, float shape_width, float max_step_height, float jump_strength, float gravity);
    void Init(float capsule_height, float capsule_diameter, float spring_height, float spring_radius, float jump_strength, float gravity);

    Urho3D::Node* GetCameraNode() { return camera_node; }

    bool MoveAndCollide(const Urho3D::Vector3& pos, const Urho3D::Vector3& vel,
                        Urho3D::Vector3& vmoved, Urho3D::Vector3& vremain,
                        CollisionState& cstate);
    //void MoveAndSlide(const Urho3D::Vector3& pos, const Urho3D::Vector3& vel,
    //                    Urho3D::Vector3& vmoved, Urho3D::Vector3& vremain);

    void SpringCast(const Urho3D::Vector3& pos, CollisionState& curr_state, const Urho3D::Vector3& floor_normal = Urho3D::Vector3::UP);
    void SpringCast(const Urho3D::Vector3& pos, const Urho3D::Vector3& velocity, CollisionState& curr_state, const Urho3D::Vector3& floor_normal = Urho3D::Vector3::UP);

    Urho3D::Vector3 CalcVelocity(const Urho3D::Quaternion& rotation, float time_step);

    void Move(Urho3D::Vector3& body_pos, Urho3D::Vector3& velocity);
};

}
