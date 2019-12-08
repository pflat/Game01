#pragma once

#include "../Core/IncGuards.h"
#include "../Controllers/AxisCtrlSystem.h"
#include "../Controllers/VehicleWeaponCtrl.h"
#include "../Input/Controls.h"

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

///  Ship controller. Can be used in airplanes, spaceships, submarines, hot air balloons, ...
class ShipCtrl : public Urho3D::LogicComponent
{

    URHO3D_OBJECT(ShipCtrl, LogicComponent);

public:
    ///  Construct.
    ShipCtrl(Urho3D::Context* context);

    ///  Register object factory and attributes.
    static void RegisterObject(Urho3D::Context* context);

    ///  Handle startup. Called by LogicComponent base class.
    virtual void Start();

    ///  Handle physics world update. Called by LogicComponent base class.
    virtual void FixedUpdate(float time_step);

    ///  Returns the camera node. This can be used as a target node for the camera.
	Urho3D::Node* GetCameraNode() { return node_; }

	void UpdateControls(Urho3D::Input* input) { controls_.Update(input); }
	void SetControl(int control, bool active) { controls_.Set(control, active); }
    float Thrust() { return thrust_z_.Value(); }
    float Speed() { return speed; }
    float SpeedKmh() { return speed * 3600 / 1000; }

    void AddWeapon(VehicleWeaponCtrl* weapon, const Urho3D::String& slot);

private:
    ///  Input state class (keyboard and mouse).
    Controls controls_;
public:
    AxisCtrlSystem thrust_x_, thrust_y_, thrust_z_;
    AxisCtrlSystem rotate_x_, rotate_y_, rotate_z_;  //  pitch, yaw, roll
private:
    ///  Cached Urho3D components. Rigid body.
    Urho3D::WeakPtr<Urho3D::RigidBody> body_;
    ///  Cached Urho3D components. Ship material. Used to change exhaust color.
    Urho3D::WeakPtr<Urho3D::Material> material_;
public:
    ///  Cached Urho3D components. Exhausts node. Used to change engine sound and particle emissions.
    Urho3D::WeakPtr<Urho3D::Node> exhausts_node_;

    Urho3D::Sound* engine_sound;
private:
    float last_thrust_factor;
    Urho3D::Vector3 last_pos;
    float speed;
};

}
