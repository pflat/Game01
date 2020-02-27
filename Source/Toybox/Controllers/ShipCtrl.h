#pragma once

#include "../Core/IncGuards.h"
#include "../Engine/InputCtrl.h"
#include "../Input/Controls.h"
#include "../Controllers/VehicleWeaponCtrl.h"

THIRD_PARTY_GUARDS_BEGIN
#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Audio/Sound.h>
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

    ///  Returns the camera node. This should be used as a target node for the camera.
	Urho3D::Node* GetCameraNode() { return node_; }

	void UpdateControls(Urho3D::Input* input, float time_step);
	//void SetControl(int control, bool active) { controls_.Set(control, active); }
    float Thrust() { return mov_z_.Value(); }
    float Speed() { return speed; }
    float SpeedKmh() { return speed * 3600.0f / 1000.0f; }

    void AddWeapon(VehicleWeaponCtrl* weapon, const Urho3D::String& slot);

private:

    ///  Input state class (keyboard and mouse).
    Toybox::Controls controls_;

public:

    InputCtrl mov_x_, mov_y_, mov_z_;
    InputCtrl rot_x_, rot_y_, rot_z_;  //  pitch, yaw, roll

private:

    ///  Cached Urho3D components. Rigid body.
    Urho3D::WeakPtr<Urho3D::RigidBody> body_;
    ///  Cached Urho3D components. Ship material. Used to change exhaust color.
    Urho3D::WeakPtr<Urho3D::Material> material_x_;
    Urho3D::WeakPtr<Urho3D::Material> material_y_;
    Urho3D::WeakPtr<Urho3D::Material> material_z_;

public:

    ///  Cached Urho3D components. Exhausts node. Used to change engine sound and particle emissions.
    Urho3D::WeakPtr<Urho3D::Node> exhausts_x_;
    Urho3D::WeakPtr<Urho3D::Node> exhausts_y_;
    Urho3D::WeakPtr<Urho3D::Node> exhausts_z_;

    Urho3D::WeakPtr<Urho3D::Sound> sound_x_;
    Urho3D::WeakPtr<Urho3D::Sound> sound_y_;
    Urho3D::WeakPtr<Urho3D::Sound> sound_z_;

private:
    float last_thrust_factor;
    Urho3D::Vector3 last_pos;
    float speed;
};

}
