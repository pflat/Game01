#include "../Controllers/SpaceshipCtrl.h"
#include "../Core/Settings.h"

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

SpaceshipCtrl::SpaceshipCtrl(Urho3D::Context* context) :
    Urho3D::LogicComponent(context),
    last_thrust_factor(0),
    last_pos(0,0,0),
    speed(0)
{
    SetUpdateEventMask(Urho3D::USE_FIXEDUPDATE);
}


void SpaceshipCtrl::RegisterObject(Urho3D::Context* context)
{
    context->RegisterFactory<SpaceshipCtrl>();

    // These macros register the class attributes to the Context for automatic load / save handling.
    // We specify the Default attribute mode which means it will be used both for saving into file, and network replication
//    URHO3D_ATTRIBUTE("Controls Yaw", float, controls_.yaw_, 0.0f, AM_DEFAULT);
//    URHO3D_ATTRIBUTE("Controls Pitch", float, controls_.pitch_, 0.0f, AM_DEFAULT);
//    URHO3D_ATTRIBUTE("On Ground", bool, onGround_, false, AM_DEFAULT);
//    URHO3D_ATTRIBUTE("OK To Jump", bool, okToJump_, true, AM_DEFAULT);
//    URHO3D_ATTRIBUTE("In Air Timer", float, inAirTimer_, 0.0f, AM_DEFAULT);
}


void SpaceshipCtrl::Start()
{
    // Component has been inserted into its scene node. Subscribe to events now
    //SubscribeToEvent(GetNode(), E_NODECOLLISION, URHO3D_HANDLER(Character, HandleNodeCollision));
    //UnsubscribeFromEvent(Urho3D::E_)

    body = node_->GetComponent<Urho3D::RigidBody>(true);

    Urho3D::StaticModel* model = node_->GetComponent<Urho3D::StaticModel>(true);
    material = model->GetMaterial();
}


void SpaceshipCtrl::FixedUpdate(float time_step)
{
    bool key_ctrl = input.keyboard_qualifiers && Urho3D::QUAL_CTRL;
    //  Speed control
    if (input.IsKeyDown(KM_VEHICLE_SPEED_MAX) && !key_ctrl)
    {
        //thrust.target_ = thrust.max_;
        thrust.SetMax();
    }
    else if (input.IsKeyDown(KM_VEHICLE_SPEED_STOP) && !key_ctrl)
    {
        //thrust.target_ = 0;
        thrust.Stop();
    }
    else if (input.IsKeyDown(KM_VEHICLE_SPEED_INC) && !key_ctrl)
    {
        //thrust.target_ += thrust.step_;
        thrust.Increase();
    }
    else if (input.IsKeyDown(KM_VEHICLE_SPEED_DEC) && !key_ctrl)
    {
        //thrust.target_ -= thrust.step_;
        thrust.Decrease();
    }

    //  Pitch control
    if (input.IsKeyDown(KM_VEHICLE_PITCH_UP) && !key_ctrl)
    {
        //pitch.target_ = pitch.min_;
        pitch.SetMin();
    }
    else if (input.IsKeyDown(KM_VEHICLE_PITCH_DOWN) && !key_ctrl)
    {
        //pitch.target_ = pitch.max_;
        pitch.SetMax();
    }
    else
    {
        //pitch.target_ = 0;
        pitch.Stop();
    }

    //  Yaw control
    if (input.IsKeyDown(KM_VEHICLE_YAW_LEFT) && !key_ctrl)
    {
        //yaw.target_ = yaw.min_;
        yaw.SetMin();
    }
    else if (input.IsKeyDown(KM_VEHICLE_YAW_RIGHT) && !key_ctrl)
    {
        //yaw.target_ = yaw.max_;
        yaw.SetMax();
    }
    else
    {
        //yaw.target_ = 0;
        yaw.Stop();
    }

    //  Roll control
    if (input.IsKeyDown(KM_VEHICLE_ROLL_LEFT) && !key_ctrl)
    {
        //roll.target_ = roll.max_;
        roll.SetMax();
    }
    else if (input.IsKeyDown(KM_VEHICLE_ROLL_RIGHT) && !key_ctrl)
    {
        //roll.target_ = roll.min_;
        roll.SetMin();
    }
    else
    {
        //roll.target_ = 0;
        roll.Stop();
    }

    thrust.Update();
    pitch.Update();
    yaw.Update();
    roll.Update();
/*
    Urho3D::Vector3 forward_dir = (body->GetRotation() * Urho3D::Vector3::FORWARD).Normalized();
    Urho3D::Vector3 ship_rot(pitch.current, yaw.current, roll.current);
    body->ApplyForce(forward_dir * thrust.current * time_step);
    body->ApplyTorque(body->GetRotation() * ship_rot * time_step);
*/
    Urho3D::Vector3 forward_dir = (body->GetRotation() * Urho3D::Vector3::FORWARD).Normalized();
    //Urho3D::Quaternion ship_rot(pitch.current_, yaw.current_, roll.current_);
    Urho3D::Quaternion ship_rot(pitch.Value(), yaw.Value(), roll.Value());
    body->SetLinearVelocity(forward_dir * thrust.Value() * time_step);
    body->SetRotation(body->GetRotation() * ship_rot * time_step);

    //printf("%s\n", node_->GetName().CString());
    //if (node_->GetName() == "Buzzard")
    //    printf("%f %f %f\n", body->GetRotation().PitchAngle(), body->GetRotation().YawAngle(), body->GetRotation().RollAngle());

    speed = (body->GetPosition() - last_pos).Length() / time_step;
    last_pos = body->GetPosition();

    //  Change color of engine emissive faces
    float thrust_factor = thrust.Factor();
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

void SpaceshipCtrl::AddWeapon(VehicleWeaponCtrl* weapon, const Urho3D::String& slot)
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
