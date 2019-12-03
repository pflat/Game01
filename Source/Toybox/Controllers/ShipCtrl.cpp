#include "../Controllers/ShipCtrl.h"
#include "../Core/ToyboxDefs.h"
#include "../Core/Debug.h"

THIRD_PARTY_GUARDS_BEGIN
//#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Graphics/StaticModel.h>
//#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/ParticleEmitter.h>
#include <Urho3D/Graphics/ParticleEffect.h>
#include <Urho3D/Graphics/RibbonTrail.h>
//#include <Urho3D/Audio/Sound.h>
#include <Urho3D/Audio/SoundSource3D.h>
THIRD_PARTY_GUARDS_END


namespace Toybox
{

ShipCtrl::ShipCtrl(Urho3D::Context* context) :
    Urho3D::LogicComponent(context),
    last_thrust_factor(0),
    last_pos(0,0,0),
    speed(0)
{
    SetUpdateEventMask(Urho3D::USE_FIXEDUPDATE);
}


void ShipCtrl::RegisterObject(Urho3D::Context* context)
{
    context->RegisterFactory<ShipCtrl>();

    // These macros register the class attributes to the Context for automatic load / save handling.
    // We specify the Default attribute mode which means it will be used both for saving into file, and network replication
//    URHO3D_ATTRIBUTE("Controls Yaw", float, controls_.yaw_, 0.0f, AM_DEFAULT);
//    URHO3D_ATTRIBUTE("Controls Pitch", float, controls_.pitch_, 0.0f, AM_DEFAULT);
//    URHO3D_ATTRIBUTE("On Ground", bool, onGround_, false, AM_DEFAULT);
//    URHO3D_ATTRIBUTE("OK To Jump", bool, okToJump_, true, AM_DEFAULT);
//    URHO3D_ATTRIBUTE("In Air Timer", float, inAirTimer_, 0.0f, AM_DEFAULT);
}


void ShipCtrl::Start()
{
    // Component has been inserted into its scene node. Subscribe to events now
    //SubscribeToEvent(GetNode(), E_NODECOLLISION, URHO3D_HANDLER(Character, HandleNodeCollision));
    //UnsubscribeFromEvent(Urho3D::E_)

    body = node_->GetComponent<Urho3D::RigidBody>(true);

    Urho3D::StaticModel* model = node_->GetComponent<Urho3D::StaticModel>(true);
    material = model->GetMaterial();
}


void ShipCtrl::FixedUpdate(float time_step)
{
    bool key_ctrl = input.keyboard_qualifiers && Urho3D::QUAL_CTRL;

    //  Thrust in local X axis.
    if (thrust_x_.Enabled())
    {
        if (input.IsKeyDown(KM_VEHICLE_THRUST_X_MAX) && !key_ctrl)
        {
            thrust_x_.SetMax();
        }
        else if (input.IsKeyDown(KM_VEHICLE_THRUST_X_STOP) && !key_ctrl)
        {
            thrust_x_.Stop();
        }
        else if (input.IsKeyDown(KM_VEHICLE_THRUST_X_INC) && !key_ctrl)
        {
            thrust_x_.Increase();
        }
        else if (input.IsKeyDown(KM_VEHICLE_THRUST_X_DEC) && !key_ctrl)
        {
            thrust_x_.Decrease();
        }
        thrust_x_.Update();
    }

    //  Thrust in local Y axis.
    if (thrust_y_.Enabled())
    {
        if (input.IsKeyDown(KM_VEHICLE_THRUST_Y_MAX) && !key_ctrl)
        {
            thrust_y_.SetMax();
        }
        else if (input.IsKeyDown(KM_VEHICLE_THRUST_Y_STOP) && !key_ctrl)
        {
            thrust_y_.Stop();
        }
        else if (input.IsKeyDown(KM_VEHICLE_THRUST_Y_INC) && !key_ctrl)
        {
            thrust_y_.Increase();
        }
        else if (input.IsKeyDown(KM_VEHICLE_THRUST_Y_DEC) && !key_ctrl)
        {
            thrust_y_.Decrease();
        }
        thrust_y_.Update();
    }

    //  Thrust in local Z axis.
    if (thrust_z_.Enabled())
    {
        if (input.IsKeyDown(KM_VEHICLE_THRUST_Z_MAX) && !key_ctrl)
        {
            thrust_z_.SetMax();
        }
        else if (input.IsKeyDown(KM_VEHICLE_THRUST_Z_STOP) && !key_ctrl)
        {
            thrust_z_.Stop();
        }
        else if (input.IsKeyDown(KM_VEHICLE_THRUST_Z_INC) && !key_ctrl)
        {
            thrust_z_.Increase();
        }
        else if (input.IsKeyDown(KM_VEHICLE_THRUST_Z_DEC) && !key_ctrl)
        {
            thrust_z_.Decrease();
        }
        thrust_z_.Update();
    }

    //  Rotate in local X axis - pitch.
    if (rotate_x_.Enabled())
    {
        if (input.IsKeyDown(KM_VEHICLE_ROT_X_DEC) && !key_ctrl)
        {
            rotate_x_.SetMin();
        }
        else if (input.IsKeyDown(KM_VEHICLE_ROT_X_INC) && !key_ctrl)
        {
            rotate_x_.SetMax();
        }
        else
        {
            rotate_x_.Stop();
        }
        rotate_x_.Update();
    }

    //  Rotate in local Y axis - yaw.
    if (rotate_y_.Enabled())
    {
        if (input.IsKeyDown(KM_VEHICLE_ROT_Y_DEC) && !key_ctrl)
        {
            rotate_y_.SetMin();
        }
        else if (input.IsKeyDown(KM_VEHICLE_ROT_Y_INC) && !key_ctrl)
        {
            rotate_y_.SetMax();
        }
        else
        {
            rotate_y_.Stop();
        }
        rotate_y_.Update();
    }

    //  Rotate in local Z axis - roll.
    if (rotate_z_.Enabled())
    {
        if (input.IsKeyDown(KM_VEHICLE_ROT_Z_DEC) && !key_ctrl)
        {
            rotate_z_.SetMax();
        }
        else if (input.IsKeyDown(KM_VEHICLE_ROT_Z_INC) && !key_ctrl)
        {
            rotate_z_.SetMin();
        }
        else
        {
            rotate_z_.Stop();
        }
        rotate_z_.Update();
    }
/*
    Urho3D::Vector3 forward_dir = (body->GetRotation() * Urho3D::Vector3::FORWARD).Normalized();
    Urho3D::Vector3 ship_rot(pitch.current, yaw.current, roll.current);
    body->ApplyForce(forward_dir * thrust.current * time_step);
    body->ApplyTorque(body->GetRotation() * ship_rot * time_step);
*/
    Urho3D::Quaternion ship_rot = body->GetRotation();
    ship_rot = ship_rot * Urho3D::Quaternion(rotate_x_.Value(), rotate_y_.Value(), rotate_z_.Value());

    Urho3D::Vector3 local_x = (ship_rot * Urho3D::Vector3::RIGHT).Normalized();
    Urho3D::Vector3 local_y = (ship_rot * Urho3D::Vector3::UP).Normalized();
    Urho3D::Vector3 local_z = (ship_rot * Urho3D::Vector3::FORWARD).Normalized();


    body->SetRotation(ship_rot * time_step);
    body->SetLinearVelocity(((local_x * thrust_x_.Value()) + (local_y * thrust_y_.Value()) + (local_z * thrust_z_.Value())) * time_step);

    speed = (body->GetPosition() - last_pos).Length() / time_step;
    last_pos = body->GetPosition();

    //  Change color of engine emissive faces
    float thrust_factor = thrust_z_.Factor();
    if (thrust_factor != last_thrust_factor)
    {
        material->SetShaderParameter("MatEmissiveColor", Urho3D::Variant(Urho3D::Vector4(thrust_factor, thrust_factor, thrust_factor, 1.0f)));

        //  Update engine sound volume
        Urho3D::SoundSource3D* sound_source = exhausts_node->GetComponent<Urho3D::SoundSource3D>();
        if (sound_source)
        {
            if (thrust_factor == 0)
            {
                if (sound_source->IsPlaying())
                    sound_source->Stop();
            }
            else
            {
                if (!sound_source->IsPlaying() && engine_sound)
                    sound_source->Play(engine_sound);

                sound_source->SetFrequency(((thrust_factor * 10.0f * 0.04f) + 0.6f) * engine_sound->GetFrequency());
            }
            //sound_source->SetGain(Urho3D::Clamp(thrust_factor / 15.0f, 0.0f, 0.75f));
        }

        //  Update engine particle generator emission properties
        for (unsigned i = 0; i < exhausts_node->GetNumChildren(); ++i)
        {
            if (exhausts_node->GetChild(i)->GetVar("effect").GetString() == "particle")
            {
                Urho3D::ParticleEmitter* emitter = exhausts_node->GetChild(i)->GetComponent<Urho3D::ParticleEmitter>();
                Urho3D::ParticleEffect* effect = emitter->GetEffect();

                if (thrust_factor == 0)
                {
                    emitter->SetEmitting(false);
                }
                else
                {
                    emitter->SetEmitting(true);
                    effect->SetMinEmissionRate(Urho3D::Abs(thrust_factor * 800));
                    effect->SetMaxEmissionRate(Urho3D::Abs(thrust_factor * 600));
                }
            }
            else if (exhausts_node->GetChild(i)->GetVar("effect").GetString() == "ribbon")
            {
                Urho3D::RibbonTrail* ribbon = exhausts_node->GetChild(i)->GetComponent<Urho3D::RibbonTrail>();

                if (thrust_factor == 0)
                {
                    ribbon->SetEmitting(false);
                }
                else
                {
                    ribbon->SetEmitting(true);
                }
            }
        }
/*
        Urho3D::Vector<Urho3D::WeakPtr<Urho3D::Node> >::ConstIterator exhaust;
        for(exhaust = exhaust_nodes.Begin(); exhaust != exhaust_nodes.End(); exhaust++)
        {
            Urho3D::ParticleEmitter* emitter = (*exhaust)->GetComponent<Urho3D::ParticleEmitter>();
            Urho3D::ParticleEffect* effect = emitter->GetEffect();

            if (thrust.current != 0)
            {
                emitter->SetEmitting(true);
                effect->SetMinEmissionRate(Urho3D::Abs(thrust.current * 800 / thrust.maximum));
                effect->SetMaxEmissionRate(Urho3D::Abs(thrust.current * 600 / thrust.maximum));
            }
            else
            {
                emitter->SetEmitting(false);
            }
        }
*/
        last_thrust_factor = thrust_factor;
    }
}

void ShipCtrl::AddWeapon(VehicleWeaponCtrl* weapon, const Urho3D::String& slot)
{
    Urho3D::Node* slot_node = node_->GetChild("nSlots")->GetChild(slot);
    if (slot_node)
    {
        if ((slot_node->GetScale().x_ * slot_node->GetScale().y_ * slot_node->GetScale().z_) < 0)
        {
            //weapon->material->SetCullMode(Urho3D::CULL_CW);
        }
        //weapon->GetNode()->Remove();
        slot_node->AddChild(weapon->GetNode());
        //weapon->GetNode()->SetParent(slot_node);
    }
}

}
