#pragma once

#include "../Core/IncGuards.h"

THIRD_PARTY_GUARDS_BEGIN
#include <Urho3D/Core/Context.h>
//#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/Node.h>
//#include <Urho3D/Scene/Component.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Physics/RigidBody.h>
THIRD_PARTY_GUARDS_END


namespace Toybox
{

class VehicleWeaponCtrl : public Urho3D::LogicComponent
{
    URHO3D_OBJECT(VehicleWeaponCtrl, LogicComponent);

    //InputState input;
    //ShipControls pitch, yaw, roll, thrust;

    //Urho3D::WeakPtr<Urho3D::RigidBody> body;
    Urho3D::WeakPtr<Urho3D::Material> material;
    //Urho3D::WeakPtr<Urho3D::Node> exhausts_node;

    //Urho3D::Sound* engine_sound;

public:
    VehicleWeaponCtrl(Urho3D::Context* context);

    static void RegisterObject(Urho3D::Context* context);

    virtual void Start();

    virtual void FixedUpdate(float time_step);

    void ToogleCullMode();
};

}
