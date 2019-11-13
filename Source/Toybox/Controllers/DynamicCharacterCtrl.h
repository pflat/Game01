#pragma once

#include "../Core/IncGuards.h"
#include "../Core/Settings.h"
#include "../Input/InputState.h"

THIRD_PARTY_GUARDS_BEGIN
#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/PhysicsEvents.h>
THIRD_PARTY_GUARDS_END


namespace Toybox
{

class DynamicCharacter : public Urho3D::LogicComponent
{
    URHO3D_OBJECT(DynamicCharacter, LogicComponent);

    InputState input;

    unsigned frame_counter;

    Urho3D::WeakPtr<Urho3D::RigidBody> body;
    Urho3D::WeakPtr<Urho3D::Node> camera_node;

public:
    DynamicCharacter(Urho3D::Context* context);

    static void RegisterObject(Urho3D::Context* context);

    virtual void Start();

    virtual void FixedUpdate(float time_step);

    void HandleNodeCollision(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data);

    Urho3D::Node* GetCameraNode() { return camera_node; }
};

}
