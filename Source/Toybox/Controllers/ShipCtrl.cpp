#include "../Controllers/ShipCtrl.h"
#include "../Core/ToyboxDefs.h"
#include "../Input/InputMap.h"

THIRD_PARTY_GUARDS_BEGIN
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/ParticleEmitter.h>
#include <Urho3D/Graphics/ParticleEffect.h>
#include <Urho3D/Graphics/RibbonTrail.h>
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
    material_z_ = model->GetMaterial();
}


void ShipCtrl::UpdateControls(Urho3D::Input* input, float time_step)
{
    controls_.Update(input);

    controls_.Set(Toybox::KM_SHIP_MOV_X_INC, Toybox::InputMap::Test("Ship.XMovInc", input));
    controls_.Set(Toybox::KM_SHIP_MOV_X_DEC, Toybox::InputMap::Test("Ship.XMovDec", input));
    controls_.Set(Toybox::KM_SHIP_MOV_X_MAX, Toybox::InputMap::Test("Ship.XMovMax", input));
    controls_.Set(Toybox::KM_SHIP_MOV_X_MIN, Toybox::InputMap::Test("Ship.XMovMin", input));
    controls_.Set(Toybox::KM_SHIP_MOV_X_STP, Toybox::InputMap::Test("Ship.XMovStp", input));
    controls_.Set(Toybox::KM_SHIP_ROT_X_INC, Toybox::InputMap::Test("Ship.XRotInc", input));
    controls_.Set(Toybox::KM_SHIP_ROT_X_DEC, Toybox::InputMap::Test("Ship.XRotDec", input));
    controls_.Set(Toybox::KM_SHIP_ROT_X_MAX, Toybox::InputMap::Test("Ship.XRotMax", input));
    controls_.Set(Toybox::KM_SHIP_ROT_X_MIN, Toybox::InputMap::Test("Ship.XRotMin", input));
    controls_.Set(Toybox::KM_SHIP_ROT_X_STP, Toybox::InputMap::Test("Ship.XRotStp", input));

    controls_.Set(Toybox::KM_SHIP_MOV_Y_INC, Toybox::InputMap::Test("Ship.YMovInc", input));
    controls_.Set(Toybox::KM_SHIP_MOV_Y_DEC, Toybox::InputMap::Test("Ship.YMovDec", input));
    controls_.Set(Toybox::KM_SHIP_MOV_Y_MAX, Toybox::InputMap::Test("Ship.YMovMax", input));
    controls_.Set(Toybox::KM_SHIP_MOV_Y_MIN, Toybox::InputMap::Test("Ship.YMovMin", input));
    controls_.Set(Toybox::KM_SHIP_MOV_Y_STP, Toybox::InputMap::Test("Ship.YMovStp", input));
    controls_.Set(Toybox::KM_SHIP_ROT_Y_DEC, Toybox::InputMap::Test("Ship.YRotInc", input));
    controls_.Set(Toybox::KM_SHIP_ROT_Y_INC, Toybox::InputMap::Test("Ship.YRotDec", input));
    controls_.Set(Toybox::KM_SHIP_ROT_Y_MAX, Toybox::InputMap::Test("Ship.YRotMax", input));
    controls_.Set(Toybox::KM_SHIP_ROT_Y_MIN, Toybox::InputMap::Test("Ship.YRotMin", input));
    controls_.Set(Toybox::KM_SHIP_ROT_Y_STP, Toybox::InputMap::Test("Ship.YRotStp", input));

    controls_.Set(Toybox::KM_SHIP_MOV_Z_INC, Toybox::InputMap::Test("Ship.ZMovInc", input));
    controls_.Set(Toybox::KM_SHIP_MOV_Z_DEC, Toybox::InputMap::Test("Ship.ZMovDec", input));
    controls_.Set(Toybox::KM_SHIP_MOV_Z_MAX, Toybox::InputMap::Test("Ship.ZMovMax", input));
    controls_.Set(Toybox::KM_SHIP_MOV_Z_MIN, Toybox::InputMap::Test("Ship.ZMovMin", input));
    controls_.Set(Toybox::KM_SHIP_MOV_Z_STP, Toybox::InputMap::Test("Ship.ZMovStp", input));
    controls_.Set(Toybox::KM_SHIP_ROT_Z_INC, Toybox::InputMap::Test("Ship.ZRotInc", input));
    controls_.Set(Toybox::KM_SHIP_ROT_Z_DEC, Toybox::InputMap::Test("Ship.ZRotDec", input));
    controls_.Set(Toybox::KM_SHIP_ROT_Z_MAX, Toybox::InputMap::Test("Ship.ZRotMax", input));
    controls_.Set(Toybox::KM_SHIP_ROT_Z_MIN, Toybox::InputMap::Test("Ship.ZRotMin", input));
    controls_.Set(Toybox::KM_SHIP_ROT_Z_STP, Toybox::InputMap::Test("Ship.ZRotStp", input));
}


void ShipCtrl::FixedUpdate(float time_step)
{
    //  Thrust in local X axis.
    if (mov_x_.Enabled())
    {
        if (mov_x_.GetMode() & InputCtrl::MODE_SET)
        {
            if (controls_.Test(Toybox::KM_SHIP_MOV_X_INC))
                mov_x_.Increase();
            else if (controls_.Test(Toybox::KM_SHIP_MOV_X_DEC))
                mov_x_.Decrease();
            else if (controls_.Test(Toybox::KM_SHIP_MOV_X_MAX))
                mov_x_.SetMax();
            else if (controls_.Test(Toybox::KM_SHIP_MOV_X_MIN))
                mov_x_.SetMin();
            else if (controls_.Test(Toybox::KM_SHIP_MOV_X_STP))
                mov_x_.Stop();
        }
        else
        {
            if (controls_.Test(Toybox::KM_SHIP_MOV_X_INC))
                mov_x_.SetMax();
            else if (controls_.Test(Toybox::KM_SHIP_MOV_X_DEC))
                mov_x_.SetMin();
            else if (controls_.Test(Toybox::KM_SHIP_MOV_X_MAX))
                mov_x_.SetMax();
            else if (controls_.Test(Toybox::KM_SHIP_MOV_X_MIN))
                mov_x_.SetMin();
            else
                mov_x_.Stop();

            if (controls_.Test(Toybox::KM_SHIP_MOV_X_STP))
                mov_x_.Stop();
        }

        mov_x_.Update();
    }

    //  Thrust in local Y axis.
    if (mov_y_.Enabled())
    {
        if (mov_y_.GetMode() & InputCtrl::MODE_SET)
        {
            if (controls_.Test(Toybox::KM_SHIP_MOV_Y_INC))
                mov_y_.Increase();
            else if (controls_.Test(Toybox::KM_SHIP_MOV_Y_DEC))
                mov_y_.Decrease();
            else if (controls_.Test(Toybox::KM_SHIP_MOV_Y_MAX))
                mov_y_.SetMax();
            else if (controls_.Test(Toybox::KM_SHIP_MOV_Y_MIN))
                mov_y_.SetMin();
            else if (controls_.Test(Toybox::KM_SHIP_MOV_Y_STP))
                mov_y_.Stop();
        }
        else
        {
            if (controls_.Test(Toybox::KM_SHIP_MOV_Y_INC))
                mov_y_.SetMax();
            else if (controls_.Test(Toybox::KM_SHIP_MOV_Y_DEC))
                mov_y_.SetMin();
            else if (controls_.Test(Toybox::KM_SHIP_MOV_Y_MAX))
                mov_y_.SetMax();
            else if (controls_.Test(Toybox::KM_SHIP_MOV_Y_MIN))
                mov_y_.SetMin();
            else
                mov_y_.Stop();

            if (controls_.Test(Toybox::KM_SHIP_MOV_Y_STP))
                mov_y_.Stop();
        }

        mov_y_.Update();
    }

    //  Thrust in local Z axis.
    if (mov_z_.Enabled())
    {
        if (mov_z_.GetMode() & InputCtrl::MODE_SET)
        {
            if (controls_.Test(Toybox::KM_SHIP_MOV_Z_INC))
                mov_z_.Increase();
            else if (controls_.Test(Toybox::KM_SHIP_MOV_Z_DEC))
                mov_z_.Decrease();
            else if (controls_.Test(Toybox::KM_SHIP_MOV_Z_MAX))
                mov_z_.SetMax();
            else if (controls_.Test(Toybox::KM_SHIP_MOV_Z_MIN))
                mov_z_.SetMin();
            else if (controls_.Test(Toybox::KM_SHIP_MOV_Z_STP))
                mov_z_.Stop();
        }
        else
        {
            if (controls_.Test(Toybox::KM_SHIP_MOV_Z_INC))
                mov_z_.SetMax();
            else if (controls_.Test(Toybox::KM_SHIP_MOV_Z_DEC))
                mov_z_.SetMin();
            else if (controls_.Test(Toybox::KM_SHIP_MOV_Z_MAX))
                mov_z_.SetMax();
            else if (controls_.Test(Toybox::KM_SHIP_MOV_Z_MIN))
                mov_z_.SetMin();
            else
                mov_z_.Stop();

            if (controls_.Test(Toybox::KM_SHIP_MOV_Z_STP))
                mov_z_.Stop();
        }

        mov_z_.Update();
    }

    //  Rotate in local X axis - pitch.
    if (rot_x_.Enabled())
    {
        if (rot_x_.GetMode() & InputCtrl::MODE_SET)
        {
            if (controls_.Test(Toybox::KM_SHIP_ROT_X_INC))
                rot_x_.Increase();
            else if (controls_.Test(Toybox::KM_SHIP_ROT_X_DEC))
                rot_x_.Decrease();
            else if (controls_.Test(Toybox::KM_SHIP_ROT_X_MAX))
                rot_x_.SetMax();
            else if (controls_.Test(Toybox::KM_SHIP_ROT_X_MIN))
                rot_x_.SetMin();
            else if (controls_.Test(Toybox::KM_SHIP_ROT_X_STP))
                rot_x_.Stop();
        }
        else
        {
            if (controls_.Test(Toybox::KM_SHIP_ROT_X_INC))
                rot_x_.SetMax();
            else if (controls_.Test(Toybox::KM_SHIP_ROT_X_DEC))
                rot_x_.SetMin();
            else if (controls_.Test(Toybox::KM_SHIP_ROT_X_MAX))
                rot_x_.SetMax();
            else if (controls_.Test(Toybox::KM_SHIP_ROT_X_MIN))
                rot_x_.SetMin();
            else
                rot_x_.Stop();

            if (controls_.Test(Toybox::KM_SHIP_ROT_X_STP))
                rot_x_.Stop();
        }

        rot_x_.Update();
    }

    //  Rotate in local Y axis - yaw.
    if (rot_y_.Enabled())
    {
        if (rot_y_.GetMode() & InputCtrl::MODE_SET)
        {
            if (controls_.Test(Toybox::KM_SHIP_ROT_Y_INC))
                rot_y_.Increase();
            else if (controls_.Test(Toybox::KM_SHIP_ROT_Y_DEC))
                rot_y_.Decrease();
            else if (controls_.Test(Toybox::KM_SHIP_ROT_Y_MAX))
                rot_y_.SetMax();
            else if (controls_.Test(Toybox::KM_SHIP_ROT_Y_MIN))
                rot_y_.SetMin();
            else if (controls_.Test(Toybox::KM_SHIP_ROT_Y_STP))
                rot_y_.Stop();
        }
        else
        {
            if (controls_.Test(Toybox::KM_SHIP_ROT_Y_INC))
                rot_y_.SetMax();
            else if (controls_.Test(Toybox::KM_SHIP_ROT_Y_DEC))
                rot_y_.SetMin();
            else if (controls_.Test(Toybox::KM_SHIP_ROT_Y_MAX))
                rot_y_.SetMax();
            else if (controls_.Test(Toybox::KM_SHIP_ROT_Y_MIN))
                rot_y_.SetMin();
            else
                rot_y_.Stop();

            if (controls_.Test(Toybox::KM_SHIP_ROT_Y_STP))
                rot_y_.Stop();
        }

        rot_y_.Update();
    }

    //  Rotate in local Z axis - roll.
    if (rot_z_.Enabled())
    {
        if (rot_z_.GetMode() & InputCtrl::MODE_SET)
        {
            if (controls_.Test(Toybox::KM_SHIP_ROT_Z_INC))
                rot_z_.Increase();
            else if (controls_.Test(Toybox::KM_SHIP_ROT_Z_DEC))
                rot_z_.Decrease();
            else if (controls_.Test(Toybox::KM_SHIP_ROT_Z_MAX))
                rot_z_.SetMax();
            else if (controls_.Test(Toybox::KM_SHIP_ROT_Z_MIN))
                rot_z_.SetMin();
            else if (controls_.Test(Toybox::KM_SHIP_ROT_Z_STP))
                rot_z_.Stop();
        }
        else
        {
            if (controls_.Test(Toybox::KM_SHIP_ROT_Z_INC))
                rot_z_.SetMax();
            else if (controls_.Test(Toybox::KM_SHIP_ROT_Z_DEC))
                rot_z_.SetMin();
            else if (controls_.Test(Toybox::KM_SHIP_ROT_Z_MAX))
                rot_z_.SetMax();
            else if (controls_.Test(Toybox::KM_SHIP_ROT_Z_MIN))
                rot_z_.SetMin();
            else
                rot_z_.Stop();

            if (controls_.Test(Toybox::KM_SHIP_ROT_Z_STP))
                rot_z_.Stop();
        }

        rot_z_.Update();
    }
/*
    Urho3D::Vector3 forward_dir = (body->GetRotation() * Urho3D::Vector3::FORWARD).Normalized();
    Urho3D::Vector3 ship_rot(pitch.current, yaw.current, roll.current);
    body->ApplyForce(forward_dir * thrust.current * time_step);
    body->ApplyTorque(body->GetRotation() * ship_rot * time_step);
*/
    Urho3D::Quaternion ship_rot = body_->GetRotation();
    ship_rot = ship_rot * Urho3D::Quaternion(rot_x_.Value(), rot_y_.Value(), rot_z_.Value());

    body_->SetRotation(ship_rot * time_step);
    body_->SetLinearVelocity((mov_x_.Velocity(Urho3D::Vector3::RIGHT, ship_rot) +
                             mov_y_.Velocity(Urho3D::Vector3::UP, ship_rot) +
                             mov_z_.Velocity(Urho3D::Vector3::FORWARD, ship_rot)) * time_step);
    //  Add some random variation to the movement.
    //  Controlled by pilot skill.
    speed = (body_->GetPosition() - last_pos).Length() / time_step;
    last_pos = body_->GetPosition();

    //  Change color of engine emissive faces
    float thrust_factor = mov_z_.Factor();
    if (thrust_factor != last_thrust_factor)
    {
        material_z_->SetShaderParameter("MatEmissiveColor", Urho3D::Variant(Urho3D::Vector4(thrust_factor, thrust_factor, thrust_factor, 1.0f)));

        //  Update engine sound.
        Urho3D::SoundSource3D* sound_source = exhausts_z_->GetComponent<Urho3D::SoundSource3D>();
        if (sound_source)
        {
            if (thrust_factor == 0)
            {
                if (sound_source->IsPlaying())
                    sound_source->Stop();
            }
            else
            {
                if (!sound_source->IsPlaying() && sound_z_)
                    sound_source->Play(sound_z_);

                sound_source->SetFrequency(((thrust_factor * 10.0f * 0.04f) + 0.6f) * sound_z_->GetFrequency());
            }
            //sound_source->SetGain(Urho3D::Clamp(thrust_factor / 15.0f, 0.0f, 0.75f));
        }

        //  Update engine particle generator emission properties.
        for (unsigned i = 0; i < exhausts_z_->GetNumChildren(); ++i)
        {
            if (exhausts_z_->GetChild(i)->GetVar("effect").GetString() == "particle")
            {
                Urho3D::ParticleEmitter* emitter = exhausts_z_->GetChild(i)->GetComponent<Urho3D::ParticleEmitter>();
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
            else if (exhausts_z_->GetChild(i)->GetVar("effect").GetString() == "ribbon")
            {
                Urho3D::RibbonTrail* ribbon = exhausts_z_->GetChild(i)->GetComponent<Urho3D::RibbonTrail>();

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
    Urho3D::Node* slot_node = node_->GetChild("slots")->GetChild(slot);
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
