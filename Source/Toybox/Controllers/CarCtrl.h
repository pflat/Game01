#pragma once

#include "../Core/IncGuards.h"
#include "../Core/ToyboxDefs.h"
#include "../Input/Controls.h"

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

class CarCtrl : public Urho3D::LogicComponent
{

    URHO3D_OBJECT(CarCtrl, LogicComponent);

public:

    public:
    CarCtrl(Urho3D::Context* context);

    static void RegisterObject(Urho3D::Context* context);

    virtual void Start();

    virtual void FixedUpdate(float time_step);

};

}
