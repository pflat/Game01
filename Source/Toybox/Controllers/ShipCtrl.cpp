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

    body_ = node_->GetComponent<Urho3D::RigidBody>(true);

    Urho3D::StaticModel* model = node_->GetComponent<Urho3D::StaticModel>(true);
    material_ = model->GetMaterial();
}


void ShipCtrl::FixedUpdate(float time_step)
{
    //  Thrust in local X axis.
    if (thrust_x_.Enabled())
    {
        thrust_x_.Update(controls_.State() >> 0);
    }

    //  Thrust in local Y axis.
    if (thrust_y_.Enabled())
    {
        thrust_y_.Update(controls_.State() >> 10);
    }

    //  Thrust in local Z axis.
    if (thrust_z_.Enabled())
    {
        thrust_z_.Update(controls_.State() >> 20);
    }

    //  Rotate in local X axis - pitch.
    if (rotate_x_.Enabled())
    {
        rotate_x_.Update(controls_.State() >> 5);
    }

    //  Rotate in local Y axis - yaw.
    if (rotate_y_.Enabled())
    {
        rotate_y_.Update(controls_.State() >> 15);
    }

    //  Rotate in local Z axis - roll.
    if (rotate_z_.Enabled())
    {
        rotate_z_.Update(controls_.State() >> 25);
    }
/*
    Urho3D::Vector3 forward_dir = (body->GetRotation() * Urho3D::Vector3::FORWARD).Normalized();
    Urho3D::Vector3 ship_rot(pitch.current, yaw.current, roll.current);
    body->ApplyForce(forward_dir * thrust.current * time_step);
    body->ApplyTorque(body->GetRotation() * ship_rot * time_step);
*/
    Urho3D::Quaternion ship_rot = body_->GetRotation();
    ship_rot = ship_rot * Urho3D::Quaternion(rotate_x_.Value(), rotate_y_.Value(), rotate_z_.Value());

    body_->SetRotation(ship_rot * time_step);
    body_->SetLinearVelocity((thrust_x_.Velocity(Urho3D::Vector3::RIGHT, ship_rot) +
                             thrust_y_.Velocity(Urho3D::Vector3::UP, ship_rot) +
                             thrust_z_.Velocity(Urho3D::Vector3::FORWARD, ship_rot)) * time_step);
    //  Add some random variation to the movement.
    //  Controlled by pilot skill.
    speed = (body_->GetPosition() - last_pos).Length() / time_step;
    last_pos = body_->GetPosition();

    //  Change color of engine emissive faces
    float thrust_factor = thrust_z_.Factor();
    if (thrust_factor != last_thrust_factor)
    {
        material_->SetShaderParameter("MatEmissiveColor", Urho3D::Variant(Urho3D::Vector4(thrust_factor, thrust_factor, thrust_factor, 1.0f)));

        //  Update engine sound.
        Urho3D::SoundSource3D* sound_source = exhausts_node_->GetComponent<Urho3D::SoundSource3D>();
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

        //  Update engine particle generator emission properties.
        for (unsigned i = 0; i < exhausts_node_->GetNumChildren(); ++i)
        {
            if (exhausts_node_->GetChild(i)->GetVar("effect").GetString() == "particle")
            {
                Urho3D::ParticleEmitter* emitter = exhausts_node_->GetChild(i)->GetComponent<Urho3D::ParticleEmitter>();
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
            else if (exhausts_node_->GetChild(i)->GetVar("effect").GetString() == "ribbon")
            {
                Urho3D::RibbonTrail* ribbon = exhausts_node_->GetChild(i)->GetComponent<Urho3D::RibbonTrail>();

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
