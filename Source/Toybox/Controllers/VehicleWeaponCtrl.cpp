#include "../Controllers/VehicleWeaponCtrl.h"

THIRD_PARTY_GUARDS_BEGIN
#include <Urho3D/Graphics/StaticModel.h>
THIRD_PARTY_GUARDS_END


namespace Toybox
{

VehicleWeaponCtrl::VehicleWeaponCtrl(Urho3D::Context* context) :
    Urho3D::LogicComponent(context)
{
    SetUpdateEventMask(Urho3D::USE_FIXEDUPDATE);
}


void VehicleWeaponCtrl::RegisterObject(Urho3D::Context* context)
{
    context->RegisterFactory<VehicleWeaponCtrl>();

    // These macros register the class attributes to the Context for automatic load / save handling.
    // We specify the Default attribute mode which means it will be used both for saving into file, and network replication
//    URHO3D_ATTRIBUTE("Controls Yaw", float, controls_.yaw_, 0.0f, AM_DEFAULT);
//    URHO3D_ATTRIBUTE("Controls Pitch", float, controls_.pitch_, 0.0f, AM_DEFAULT);
//    URHO3D_ATTRIBUTE("On Ground", bool, onGround_, false, AM_DEFAULT);
//    URHO3D_ATTRIBUTE("OK To Jump", bool, okToJump_, true, AM_DEFAULT);
//    URHO3D_ATTRIBUTE("In Air Timer", float, inAirTimer_, 0.0f, AM_DEFAULT);
}


void VehicleWeaponCtrl::Start()
{
    // Component has been inserted into its scene node. Subscribe to events now
    //SubscribeToEvent(GetNode(), E_NODECOLLISION, URHO3D_HANDLER(Character, HandleNodeCollision));

    //Urho3D::RigidBody* body = node_->GetComponent<Urho3D::RigidBody>(true);

    Urho3D::StaticModel* model = node_->GetComponent<Urho3D::StaticModel>(true);
    material = model->GetMaterial();
}


void VehicleWeaponCtrl::FixedUpdate(float time_step)
{

}


void VehicleWeaponCtrl::ToogleCullMode()
{
    if (material->GetCullMode() == Urho3D::CULL_CCW)
        material->SetCullMode(Urho3D::CULL_CW);
    else
        material->SetCullMode(Urho3D::CULL_CCW);
}

}
