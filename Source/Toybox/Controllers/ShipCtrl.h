#pragma once

#include "../Core/IncGuards.h"
#include "../Controllers/AxisCtrlSystem.h"
#include "../Controllers/VehicleWeaponCtrl.h"
#include "../Input/InputState.h"

THIRD_PARTY_GUARDS_BEGIN
#include <Urho3D/Core/Context.h>
//#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/Node.h>
//#include <Urho3D/Scene/Component.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Audio/Sound.h>
//#include <Urho3D/Audio/SoundSource.h>
//#include <Urho3D/Input/Controls.h>
//#include <Urho3D/Resource/ResourceCache.h>
//#include <Urho3D/Container/Vector.h>
THIRD_PARTY_GUARDS_END


namespace Toybox
{

class ShipCtrl : public Urho3D::LogicComponent
{
    URHO3D_OBJECT(ShipCtrl, LogicComponent);

    InputState input;
    AxisCtrlSystem thrust_x_, thrust_y_, thrust_z_;
    AxisCtrlSystem rotate_x_, rotate_y_, rotate_z_;  //  pitch, yaw, roll

    Urho3D::WeakPtr<Urho3D::RigidBody> body;
    Urho3D::WeakPtr<Urho3D::Material> material;
    Urho3D::WeakPtr<Urho3D::Node> exhausts_node;

    Urho3D::Sound* engine_sound;

    float last_thrust_factor;
    Urho3D::Vector3 last_pos;
    float speed;

public:
    ShipCtrl(Urho3D::Context* context);

    static void RegisterObject(Urho3D::Context* context);
    virtual void Start();
    virtual void FixedUpdate(float time_step);

	Urho3D::Node* GetCameraNode() { return node_; }

    float Thrust() { return thrust_z_.Value(); }
    float Speed() { return speed; }
    float SpeedKmh() { return speed * 3600 / 1000; }

    void AddWeapon(VehicleWeaponCtrl* weapon, const Urho3D::String& slot);
};

}
