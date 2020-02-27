#include "../Controllers/CarCtrl.h"
#include "../Core/ToyboxDefs.h"
#include "../Core/IO.h"
#include "../Input/InputMap.h"
#include "../Core/Debug.h"

THIRD_PARTY_GUARDS_BEGIN
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/Node.h>
//#include <Urho3D/Physics/PhysicsWorld.h>
//#include <Urho3D/Physics/PhysicsEvents.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/Constraint.h>
THIRD_PARTY_GUARDS_END


namespace Toybox
{

void CarWheel::Load(const Urho3D::XMLElement& xml_elem)
{

}


CarCtrl::CarCtrl(Urho3D::Context* context) :
    Urho3D::LogicComponent(context)
{
    //SetUpdateEventMask(Urho3D::USE_FIXEDUPDATE);
    SetUpdateEventMask(Urho3D::USE_UPDATE);
}


void CarCtrl::RegisterObject(Urho3D::Context* context)
{
    context->RegisterFactory<CarCtrl>();

    //  These macros register the class attributes to the Context for automatic load / save handling.
    //  We specify the Default attribute mode which means it will be used both for saving into file, and network replication
    URHO3D_ATTRIBUTE("In Air RPM", float, in_air_rpm_, 0.0f, Urho3D::AM_DEFAULT);
}


///  Applying attributes
void CarCtrl::ApplyAttributes()
{
    Urho3D::RaycastVehicle* vehicle = node_->GetOrCreateComponent<Urho3D::RaycastVehicle>();
/*
    if (emittersCreated)
        return;
    for (int i = 0; i < 4; i++)
    {
        CreateEmitter(connectionPoints_[i]);
    }
    emittersCreated = true;
*/
}


void CarCtrl::Start()
{
    //  Component has been inserted into its scene node. Subscribe to events now
    body_ = node_->GetComponent<Urho3D::RigidBody>(true);
    frame_counter_ = 0;
}


void CarCtrl::Init()
{

}


void CarCtrl::UpdateControls(Urho3D::Input* input, float time_step)
{
    controls_.Update(input);

    controls_.Set(Toybox::KM_CAR_MOV_Z_INC, Toybox::InputMap::Test("Car.ZMovInc", input));
    controls_.Set(Toybox::KM_CAR_MOV_Z_DEC, Toybox::InputMap::Test("Car.ZMovDec", input));
    controls_.Set(Toybox::KM_CAR_MOV_Z_STP, Toybox::InputMap::Test("Car.ZMovStp", input));
    controls_.Set(Toybox::KM_CAR_ROT_Y_INC, Toybox::InputMap::Test("Car.YRotInc", input));
    controls_.Set(Toybox::KM_CAR_ROT_Y_DEC, Toybox::InputMap::Test("Car.YRotDec", input));
}


void CarCtrl::Update(float time_step)
{
    is_braking_ = false;
    is_handbraking_ = false;

    //  Thrust in local Z axis - power.
    if (mov_z_.Enabled())
    {
        if (mov_z_.GetMode() & InputCtrl::MODE_SET)
        {
            if (controls_.Test(Toybox::KM_CAR_MOV_Z_INC))
                mov_z_.Increase();
            else if (controls_.Test(Toybox::KM_CAR_MOV_Z_DEC))
                mov_z_.Decrease();
//            else if (controls_.Test(Toybox::KM_CAR_MOV_Z_MAX))
//                mov_z_.SetMax();
//            else if (controls_.Test(Toybox::KM_CAR_MOV_Z_MIN))
//                mov_z_.SetMin();
            else if (controls_.Test(Toybox::KM_CAR_MOV_Z_STP))
            {
                mov_z_.Stop();
                is_braking_ = true;
            }
        }
        else
        {
            if (controls_.Test(Toybox::KM_CAR_MOV_Z_INC))
                mov_z_.SetMax();
            else if (controls_.Test(Toybox::KM_CAR_MOV_Z_DEC))
            {
                if (speed_ > 0.0f)
                {
//                    printf("%.6f\n", speed_);
                    mov_z_.Stop();
                    is_braking_ = true;
                }
                else
                {
                    mov_z_.SetMin();
                }
            }
//            else if (controls_.Test(Toybox::KM_CAR_MOV_Z_MAX))
//                mov_z_.SetMax();
//            else if (controls_.Test(Toybox::KM_CAR_MOV_Z_MIN))
//                mov_z_.SetMin();
            else
                mov_z_.Stop();

            if (controls_.Test(Toybox::KM_CAR_MOV_Z_STP))
            {
                mov_z_.Stop();
                is_braking_ = true;
            }
        }

        mov_z_.Update();
    }

    //  Rotate in local Y axis - turn.
    if (rot_y_.Enabled())
    {
        if (rot_y_.GetMode() & InputCtrl::MODE_SET)
        {
            if (controls_.Test(Toybox::KM_CAR_ROT_Y_INC))
                rot_y_.Increase();
            else if (controls_.Test(Toybox::KM_CAR_ROT_Y_DEC))
                rot_y_.Decrease();
//            else if (controls_.Test(Toybox::KM_CAR_ROT_Y_MAX))
//                rot_y_.SetMax();
//            else if (controls_.Test(Toybox::KM_CAR_ROT_Y_MIN))
//                rot_y_.SetMin();
//            else if (controls_.Test(Toybox::KM_CAR_ROT_Y_STP))
//                rot_y_.Stop();
        }
        else
        {
            if (controls_.Test(Toybox::KM_CAR_ROT_Y_INC))
                rot_y_.SetMax();
            else if (controls_.Test(Toybox::KM_CAR_ROT_Y_DEC))
                rot_y_.SetMin();
//            else if (controls_.Test(Toybox::KM_CAR_ROT_Y_MAX))
//                rot_y_.SetMax();
//            else if (controls_.Test(Toybox::KM_CAR_ROT_Y_MIN))
//                rot_y_.SetMin();
//            else if (controls_.Test(Toybox::KM_CAR_ROT_Y_STP))
//                rot_y_.Stop();
            else
                rot_y_.Stop();
        }

        rot_y_.Update();
    }

    Urho3D::RaycastVehicle* vehicle = node_->GetComponent<Urho3D::RaycastVehicle>();

    vehicle->SetSteeringValue(0, rot_y_.Value());
    //vehicle->SetSteeringValue(1, rot_y_.Value());
    vehicle->SetSteeringValue(2, -rot_y_.Value());
    vehicle->SetSteeringValue(3, rot_y_.Value());
    //vehicle->SetSteeringValue(4, rot_y_.Value());
    vehicle->SetSteeringValue(5, -rot_y_.Value());

    vehicle->SetEngineForce(0, mov_z_.Value());
    vehicle->SetEngineForce(1, mov_z_.Value());
    vehicle->SetEngineForce(2, mov_z_.Value());
    vehicle->SetEngineForce(3, mov_z_.Value());
    vehicle->SetEngineForce(4, mov_z_.Value());
    vehicle->SetEngineForce(5, mov_z_.Value());

    if (is_braking_)
    {
        vehicle->SetEngineForce(0, 0.0f);
        vehicle->SetEngineForce(1, 0.0f);
        vehicle->SetEngineForce(2, 0.0f);
        vehicle->SetEngineForce(3, 0.0f);
        vehicle->SetEngineForce(4, 0.0f);
        vehicle->SetEngineForce(5, 0.0f);

        float brake_force = 150.0f;
        vehicle->SetBrake(0, brake_force);
        vehicle->SetBrake(1, brake_force);
        vehicle->SetBrake(2, brake_force);
        vehicle->SetBrake(3, brake_force);
        vehicle->SetBrake(4, brake_force);
        vehicle->SetBrake(5, brake_force);
    }
    else
    {
        vehicle->SetBrake(0, 0.0f);
        vehicle->SetBrake(1, 0.0f);
        vehicle->SetBrake(2, 0.0f);
        vehicle->SetBrake(3, 0.0f);
        vehicle->SetBrake(4, 0.0f);
        vehicle->SetBrake(5, 0.0f);
    }


    Urho3D::Vector3 front_dir = body_->GetRotation() * Urho3D::Vector3::FORWARD;
    front_dir.Normalize();
    Urho3D::Vector3 vel = body_->GetPosition() - last_pos_;
    speed_ = vel.Length() / time_step;

    if (front_dir.DotProduct(vel.Normalized()) < 0.0f)
    {
        speed_ = -speed_;
    }

//    printf("%d - %.6f\n", frame_counter_, speed_);
    frame_counter_++;


    //speed_ = (body_->GetPosition() - last_pos_).Length() / time_step;
    last_pos_ = body_->GetPosition();
    acceleration_ = speed_ - last_speed_;

}




void CarCtrl::Load(const Urho3D::XMLElement& xml_elem)
{
    LoadNode(xml_elem);
}


void CarCtrl::LoadNode(const Urho3D::XMLElement& xml_elem)
{
    if (xml_elem.HasChild("wheel"))
    {
        Urho3D::Node* node = node_->GetChild(xml_elem.GetAttribute("name"), true);
        AddWheel(node, xml_elem.GetChild("wheel"));
    }

    for (Urho3D::XMLElement child_source = xml_elem.GetChild("node"); child_source; child_source = child_source.GetNext("node"))
    {
        LoadNode(child_source);
    }
}


void CarCtrl::AddWheel(Urho3D::Node* wheel_node, const Urho3D::XMLElement& xml_elem)
{
    //  Load wheel properties
    CarWheel wheel_data;

    wheel_data.slot_ = xml_elem.GetInt("slot");
    wheel_data.class_ = xml_elem.GetAttribute("class");
    wheel_data.radius_ = wheel_node->GetScale().z_ / 2.0f;
    wheel_data.is_front_ = xml_elem.GetBool("front");
    wheel_data.traction_ = xml_elem.GetBool("traction");
    wheel_data.brake_ = xml_elem.GetBool("brake");
    wheel_data.handbrake_ = xml_elem.GetBool("handbrake");
    wheel_data.steer_ = xml_elem.GetBool("steer");
    wheel_data.suspension_max_travel_ = xml_elem.GetFloat("suspension_max_travel");
    wheel_data.suspension_rest_length_ = xml_elem.GetFloat("suspension_rest_length");
    wheel_data.suspension_stiffness_ = xml_elem.GetFloat("suspension_stiffness");
    wheel_data.damping_relaxation_ = xml_elem.GetFloat("damping_relaxation");
    wheel_data.damping_compression_ = xml_elem.GetFloat("damping_compression");
    wheel_data.roll_influence_ = xml_elem.GetFloat("roll_influence");

    wheels_.insert(wheels_.begin() + wheel_data.slot_, wheel_data);

    //  Apply transforms to the wheel node
    Urho3D::Vector3 connection_point = wheel_node->GetPosition();
    Urho3D::Quaternion connection_rot = wheel_node->GetRotation();
    wheel_node->SetRotation(connection_rot * Urho3D::Quaternion(0.0f, 180.0f, 0.0f));
    if (connection_point.x_ > 0.0f)
    {
        wheel_node->SetScale(wheel_node->GetScale() * Urho3D::Vector3(1.0f, 1.0f, -1.0f));
    }

    //  Add RaycastVehicle wheel
    Urho3D::Vector3 wheel_direction(0.0f, -1.0f, 0.0f);
    Urho3D::Vector3 wheel_axle(-1.0f, 0.0f, 0.0f);
    vehicle_->AddWheel(wheel_node, wheel_direction, wheel_axle, wheel_data.suspension_rest_length_, wheel_data.radius_, wheel_data.is_front_);

    //  Load wheel model
    LoadWheel(wheel_data.slot_, wheel_node, xml_elem.GetAttribute("model"));

    //  Set wheel properties
    //vehicle->SetMaxSuspensionForce(i, mass * 8.0f);
    vehicle_->SetMaxSuspensionTravel(wheel_data.slot_, wheel_data.radius_ * 40.0f);
    vehicle_->SetWheelSuspensionStiffness(wheel_data.slot_, wheel_data.suspension_stiffness_);
    vehicle_->SetWheelDampingRelaxation(wheel_data.slot_, wheel_data.damping_relaxation_);
    vehicle_->SetWheelDampingCompression(wheel_data.slot_, wheel_data.damping_compression_);
    vehicle_->SetWheelRollInfluence(wheel_data.slot_, wheel_data.roll_influence_);
}


void CarCtrl::RemoveWheel(int id)
{

}


void CarCtrl::LoadWheel(int id, Urho3D::Node* wheel_node, const Urho3D::String& file)
{
    Urho3D::ResourceCache* cache = GetSubsystem<Urho3D::ResourceCache>();

    Urho3D::XMLFile* xml_file = cache->GetResource<Urho3D::XMLFile>(file);
    const Urho3D::XMLElement& xml_elem = xml_file->GetRoot();

    wheels_[id].friction_slip_ = xml_elem.GetChild("friction_slip").GetFloat("value");
    vehicle_->SetWheelFrictionSlip(id, wheels_[id].friction_slip_);

    IO::LoadModel(wheel_node, xml_elem, cache);
}

}
