#pragma once

#include <vector>

#include "../Core/IncGuards.h"
#include "../Engine/InputCtrl.h"
#include "../Input/Controls.h"

THIRD_PARTY_GUARDS_BEGIN
#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/LogicComponent.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/RaycastVehicle.h>
#include <Urho3D/Resource/ResourceCache.h>
THIRD_PARTY_GUARDS_END

namespace Toybox
{


class CarWheel
{
public:
    int slot_;
    Urho3D::String class_;
    float radius_;

    ///  Godot: This determines how much grip this wheel has.
    ///  Godot N/A: It is combined with the friction setting of the surface the wheel is in contact with.
    ///  Godot N/A: 0.0 means no grip, 1.0 is normal grip.
    ///  Godot: For a drift car setup, try setting the grip of the rear wheels slightly lower than the front wheels, or use a lower value to simulate tire wear.
    ///  Godot N/A: It’s best to set this to 1.0 when starting out.
    ///  Default : 10.5
    float friction_slip_;

    bool is_front_;
    float traction_;
    float brake_;
    float handbrake_;
    float steer_;

    ///  Godot: The maximum force the spring can resist.
    ///  Godot: This value should be higher than a quarter of the RigidBody.mass of the VehicleBody or the spring will not carry the weight of the vehicle.
    ///  Godot: Good results are often obtained by a value that is about 3x to 4x this number.
    ///  Default : 6000.0
    float suspension_max_force_;

    ///  Godot: This is the distance the suspension can travel.
    ///  Godot N/A: As Godot measures are in meters keep this setting relatively low.
    ///  Godot N/A: Try a value between 0.1 and 0.3 depending on the type of car .
    ///  Default : 500.0
    float suspension_max_travel_;

    ///  Godot N/A: This is the distance in meters the wheel is lowered from its origin point.
    ///  Godot N/A: Don’t set this to 0.0 and move the wheel into position, instead move the origin point of your wheel (the gizmo in Godot)
    ///  Godot N/A: to the position the wheel will take when bottoming out, then use the rest length to move the wheel down to the position
    //   Godot N/A: it should be in when the car is in rest.
    float suspension_rest_length_;

    ///  Godot: This value defines the stiffness of the suspension.
    ///  Godot N/A: Use a value lower than 50 for an off-road car, a value between 50 and 100 for a race car
    ///  Godot N/A: and try something around 200 for something like a Formula 1 car.
    ///  Default : 5.88
    float suspension_stiffness_;

    ///  Godot: The damping applied to the spring when relaxing.
    ///  Godot N/A: This value should be between 0.0 (no damping) and 1.0.
    ///  Godot N/A: This value should always be slightly higher than the damping_compression property.
    ///  Godot N/A: For a damping_compression value of 0.3, try a relaxation value of 0.5
    ///  Default : 0.88
    float damping_relaxation_;

    ///  Godot: The damping applied to the spring when the spring is being compressed.
    ///  Godot N/A: This value should be between 0.0 (no damping) and 1.0.
    ///  Godot N/A: A value of 0.0 means the car will keep bouncing as the spring keeps its energy.
    ///  Godot N/A: A good value for this is around 0.3 for a normal car, 0.5 for a race car.
    ///  Default : 0.83
    float damping_compression_;

    ///  Godot: This value effects the roll of your vehicle.
    ///  Godot N/A: If set to 0.0 for all wheels your vehicle will be prone to rolling over while a value of 1.0 will resist body roll.
    ///  Default : 0.1
    float roll_influence_;

    void Load(const Urho3D::XMLElement& xml_elem);
};


class CarCtrl : public Urho3D::LogicComponent
{

    URHO3D_OBJECT(CarCtrl, LogicComponent);

public:

    ///  Construct.
    CarCtrl(Urho3D::Context* context);

    ///  Register object factory and attributes.
    static void RegisterObject(Urho3D::Context* context);

    ///  Perform post-load after deserialization. Acquire the components from the scene nodes.
    virtual void ApplyAttributes();

    virtual void Start();

    ///  This function is called only from the main program when initially creating the vehicle, not on scene load
    void Init();

    void UpdateControls(Urho3D::Input* input, float time_step);
    void SetControl(int control, bool active) { controls_.Set(control, active); }

    ///  Returns the camera node. This should be used as a target node for the camera.
	Urho3D::Node* GetCameraNode() { return node_; }

    //void InitWheel(const Urho3D::Vector3& offset, Urho3D::WeakPtr<Urho3D::Node>& wheelNode, unsigned& wheelNodeID);

    //virtual void FixedUpdate(float time_step);
    virtual void Update(float time_step);

private:

    ///  Input state class (keyboard and mouse).
    Controls controls_;

    void LoadNode(const Urho3D::XMLElement& xml_elem);
    void LoadWheel(int id, Urho3D::Node* wheel_node, const Urho3D::String& file);

public:
    float handbrake_;
    float in_air_rpm_;
    int frame_counter_;

    ///  Cached Urho3D components. Rigid body.
    Urho3D::WeakPtr<Urho3D::RigidBody> body_;

    float GetSpeed() { return speed_; }

public:

    Urho3D::RaycastVehicle* vehicle_;

    InputCtrl mov_z_, rot_y_;
    bool is_braking_, is_handbraking_;
    Urho3D::Vector3 last_pos_;
    float speed_, last_speed_, acceleration_;

    std::vector<CarWheel> wheels_;


    void Load(const Urho3D::XMLElement& xml_elem);


    void AddWheel(Urho3D::Node* wheel_node, const Urho3D::XMLElement& xml_elem);
    void RemoveWheel(int id);
};

}
