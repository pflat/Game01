#pragma once

#include "../Core/IncGuards.h"
#include "../Controllers/VehicleCtrlSystem.h"
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
/*
const unsigned KEY_SHIP_SPEED_INC = 1 << 0;
const unsigned KEY_SHIP_SPEED_DEC = 1 << 1;
const unsigned KEY_SHIP_PITCH_UP = 1 << 2;
const unsigned KEY_SHIP_PITCH_DOWN = 1 << 3;
const unsigned KEY_SHIP_YAW_LEFT = 1 << 4;
const unsigned KEY_SHIP_YAW_RIGHT = 1 << 5;
const unsigned KEY_SHIP_ROLL_LEFT = 1 << 6;
const unsigned KEY_SHIP_ROLL_RIGHT = 1 << 7;
const unsigned KEY_SHIP_SPEED_MAX = 1 << 8;
const unsigned KEY_SHIP_SPEED_STOP = 1 << 9;
*/

class SpaceshipCtrl : public Urho3D::LogicComponent
{
    URHO3D_OBJECT(SpaceshipCtrl, LogicComponent);

    InputState input;
    VehicleCtrlSystem pitch, yaw, roll, thrust;

    Urho3D::WeakPtr<Urho3D::RigidBody> body;
    Urho3D::WeakPtr<Urho3D::Material> material;
    Urho3D::WeakPtr<Urho3D::Node> exhausts_node;

    Urho3D::Sound* engine_sound;

    float last_thrust_factor;
    Urho3D::Vector3 last_pos;
    float speed;

public:
    SpaceshipCtrl(Urho3D::Context* context);

    static void RegisterObject(Urho3D::Context* context);
    virtual void Start();
    virtual void FixedUpdate(float time_step);

	Urho3D::Node* GetCameraNode() { return node_; }

    float Thrust() { return thrust.Value(); } // return thrust.current_; }
    float Speed() { return speed; }
    float Speedkmh() { return speed * 3600 / 1000; }

    void AddWeapon(VehicleWeaponCtrl* weapon, const Urho3D::String& slot);
};

}
