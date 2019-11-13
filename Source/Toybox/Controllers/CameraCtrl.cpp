#include "../Controllers/CameraCtrl.h"
#include "../Core/Settings.h"

THIRD_PARTY_GUARDS_BEGIN
#include <Urho3D/Engine/Application.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Audio/SoundListener.h>
THIRD_PARTY_GUARDS_END


namespace Toybox
{

CameraCtrl::CameraCtrl(Urho3D::Context* context) :
    Urho3D::LogicComponent(context),
    camera_type_(CAMERA_TYPE_CHARACTER),
    camera_mode_(CAMERA_MODE_FOLLOW),
//    free_pitch_(0.0f), free_yaw_(0.0f), free_roll_(0.0f),
//    camera_pitch_(0.0f), camera_yaw_(0.0f), camera_distance_(30.0f),
    is_reseting_(false),
    is_retargeting_(false),
    retarget_timer_(0.0f),
    reset_timer_(0.0f)
{
    SetUpdateEventMask(0);
}


void CameraCtrl::RegisterObject(Urho3D::Context* context)
{
    context->RegisterFactory<CameraCtrl>();

//    URHO3D_ATTRIBUTE("Controls Yaw", float, controls_.yaw_, 0.0f, AM_DEFAULT);
//    URHO3D_ATTRIBUTE("Controls Pitch", float, controls_.pitch_, 0.0f, AM_DEFAULT);
//    URHO3D_ATTRIBUTE("On Ground", bool, onGround_, false, AM_DEFAULT);
//    URHO3D_ATTRIBUTE("OK To Jump", bool, okToJump_, true, AM_DEFAULT);
//    URHO3D_ATTRIBUTE("In Air Timer", float, inAirTimer_, 0.0f, AM_DEFAULT);
}


void CameraCtrl::Start()
{
    lookat_node_ = new Urho3D::Node(context_);
    lookat_node_->CreateComponent<Urho3D::SoundListener>();

	general_settings_.keyboard_sensitivity_ = GetGlobalVar("camera.key_sensitivity").GetFloat();
    general_settings_.mouse_sensitivity_ = GetGlobalVar("camera.mouse_sensitivity").GetFloat();
	general_settings_.smooth_ = GetGlobalVar("camera.smooth").GetBool();
	general_settings_.max_reset_time_ = GetGlobalVar("camera.reset_time").GetFloat();
	general_settings_.max_retarget_time_ = GetGlobalVar("camera.target_time").GetFloat();

    character_settings_.invert_pitch_ = GetGlobalVar("camera.character.invert_pitch").GetInt();
    character_settings_.default_pitch_ = GetGlobalVar("camera.character.default_pitch").GetFloat();
    character_settings_.default_yaw_ = GetGlobalVar("camera.character.default_yaw").GetFloat();
    character_settings_.default_distance_ = GetGlobalVar("camera.character.default_distance").GetFloat();
	character_settings_.pitch_ = 0.0f;
    character_settings_.yaw_ = 0.0f;
    character_settings_.distance_ = character_settings_.default_distance_;

	vehicle_settings_.invert_pitch_ = GetGlobalVar("camera.vehicle.invert_pitch").GetInt();
    vehicle_settings_.default_pitch_ = GetGlobalVar("camera.vehicle.default_pitch").GetFloat();
    vehicle_settings_.default_yaw_ = GetGlobalVar("camera.vehicle.default_yaw").GetFloat();
    vehicle_settings_.default_distance_ = GetGlobalVar("camera.vehicle.default_distance").GetFloat();
	vehicle_settings_.pitch_ = 0.0f;
    vehicle_settings_.yaw_ = 0.0f;
    vehicle_settings_.distance_ = vehicle_settings_.default_distance_;

	free_settings_.invert_pitch_ = GetGlobalVar("camera.free.invert_pitch").GetInt();
    free_settings_.default_pitch_ = GetGlobalVar("camera.free.default_pitch").GetFloat();
    free_settings_.default_yaw_ = GetGlobalVar("camera.free.default_yaw").GetFloat();
    free_settings_.default_distance_ = GetGlobalVar("camera.free.default_distance").GetFloat();
	free_settings_.pitch_ = 0.0f;
    free_settings_.yaw_ = 0.0f;
    free_settings_.distance_ = free_settings_.default_distance_;
}


void CameraCtrl::Stop()
{
	delete lookat_node_;
}


void CameraCtrl::SetType(unsigned type)
{
	if (camera_mode_ == CAMERA_MODE_FREE)
		return;
    
	camera_type_ = type;
}


void CameraCtrl::SwitchMode()
{
    if (is_retargeting_ == true || is_reseting_ == true)
        return;

    if (camera_mode_ == CAMERA_MODE_FOLLOW)
    {
        camera_mode_ = CAMERA_MODE_FREE;
		free_settings_.pitch_ = node_->GetWorldRotation().PitchAngle();
        free_settings_.yaw_ = node_->GetWorldRotation().YawAngle();
        free_settings_.roll_ = node_->GetWorldRotation().RollAngle();
    }
    else
    {
        camera_mode_ = CAMERA_MODE_FOLLOW;
    }
}


void CameraCtrl::Reset(bool immediate)
{
    if (is_reseting_ == false)
    {
        if (immediate == true)
        {
            if (camera_type_ == CAMERA_TYPE_VEHICLE)
            {
				vehicle_settings_.pitch_ = vehicle_settings_.default_pitch_;
                vehicle_settings_.yaw_ = vehicle_settings_.default_yaw_;
                vehicle_settings_.distance_ = vehicle_settings_.default_distance_;
            }
            else if (camera_type_ == CAMERA_TYPE_CHARACTER)
            {
				character_settings_.pitch_ = character_settings_.default_pitch_;
                character_settings_.yaw_ = character_settings_.default_yaw_;
                character_settings_.distance_ = character_settings_.default_distance_;
            }
        }
        else
        {
            is_reseting_ = true;
            reset_timer_ = 0.0f;
        }
    }
}


void CameraCtrl::SetTarget(Urho3D::Node* target, bool immediate)
{
    target_node_ = target;
    //lookat_node_->Remove();
    //target_node_->GetScene()->AddChild(lookat_node_);

    if (immediate == true)
    {
        lookat_node_->SetWorldTransform(target_node_->GetWorldPosition(), target_node_->GetWorldRotation(), target_node_->GetWorldScale());
    }
    else
    {
        is_retargeting_ = true;
        retarget_timer_ = 0.0f;
    }
}


void CameraCtrl::UpdateCamera(float time_step)
{
    // Do not move if the UI has a focused element (the console)
//    if (GetSubsystem<UI>()->GetFocusElement())
//        return;
/*
    if (input.IsKeyJustDown(KEY_CAMERA_MODE))
    {
        if (camera_mode == CAM_SHIP || camera_mode == CAM_CHAR)
        {
            camera_mode = CAM_FREE;
            free_pitch = node_->GetWorldRotation().PitchAngle();
            free_yaw = node_->GetWorldRotation().YawAngle();
            free_roll = node_->GetWorldRotation().RollAngle();
        }
        else
        {
            if (camera_type == CAM_SPACE)
            {
                camera_mode = CAM_SHIP;
            }
            else if (camera_type == CAM_GROUND)
            {
                camera_mode = CAM_CHAR;
            }
        }

    }
*/
    if (is_retargeting_ == true)
    {
        if ((camera_type_ == CAMERA_TYPE_VEHICLE || camera_type_ == CAMERA_TYPE_CHARACTER) && general_settings_.smooth_ == true)
        {
            retarget_timer_ += time_step;
            if (retarget_timer_ < general_settings_.max_retarget_time_)
            {
                //float rate = 1 - Urho3D::Pow(0.001f, time_step);
                float rate = retarget_timer_ / general_settings_.max_retarget_time_;
                rate *= rate;
                lookat_node_->SetWorldPosition(Urho3D::Lerp(lookat_node_->GetWorldPosition(), target_node_->GetWorldPosition(), rate));
                lookat_node_->SetWorldRotation(Urho3D::Lerp(lookat_node_->GetWorldRotation(), target_node_->GetWorldRotation(), rate));
                lookat_node_->SetWorldScale(Urho3D::Lerp(lookat_node_->GetWorldScale(), target_node_->GetWorldScale(), rate));
            }
            else
            {
                is_retargeting_ = false;
            }
        }
        else
        {
            //  Don't set the word transform here. It will be done in the next frame.
            is_retargeting_ = false;
        }
    }
    else
    {
        lookat_node_->SetWorldTransform(target_node_->GetWorldPosition(), target_node_->GetWorldRotation(), target_node_->GetWorldScale());
    }

    if (is_reseting_ == true)
    {
        if ((camera_type_ == CAMERA_TYPE_VEHICLE || camera_type_ == CAMERA_TYPE_CHARACTER) && general_settings_.smooth_ == true)
        {
            reset_timer_ += time_step;
            if (reset_timer_ < general_settings_.max_reset_time_)
            {
                //float rate = 1 - Urho3D::Pow(0.0001f, time_step);
                float rate = reset_timer_ / general_settings_.max_reset_time_;
                rate *= rate;
                if (camera_type_ == CAMERA_TYPE_VEHICLE)
                {
					vehicle_settings_.pitch_ = Urho3D::Lerp(vehicle_settings_.pitch_, vehicle_settings_.default_pitch_, rate);
                    vehicle_settings_.yaw_ = Urho3D::Lerp(vehicle_settings_.yaw_, vehicle_settings_.default_yaw_, rate);
                    vehicle_settings_.distance_ = Urho3D::Lerp(vehicle_settings_.distance_, vehicle_settings_.default_distance_, rate);
                }
                else if (camera_type_ == CAMERA_TYPE_CHARACTER)
                {
					character_settings_.pitch_ = Urho3D::Lerp(character_settings_.pitch_, character_settings_.default_pitch_, rate);
                    character_settings_.yaw_ = Urho3D::Lerp(character_settings_.yaw_, character_settings_.default_yaw_, rate);
                    character_settings_.distance_ = Urho3D::Lerp(character_settings_.distance_, character_settings_.default_distance_, rate);
                }
            }
            else
            {
                is_reseting_ = false;
            }
        }
        else
        {
            if (camera_type_ == CAMERA_TYPE_VEHICLE)
            {
                vehicle_settings_.pitch_ = vehicle_settings_.default_pitch_;
                vehicle_settings_.yaw_ = vehicle_settings_.default_yaw_;
                vehicle_settings_.distance_ = vehicle_settings_.default_distance_;
            }
            else if (camera_type_ == CAMERA_TYPE_CHARACTER)
            {
                character_settings_.pitch_ = character_settings_.default_pitch_;
                character_settings_.yaw_ = character_settings_.default_yaw_;
                character_settings_.distance_ = character_settings_.default_distance_;
            }
            is_reseting_ = false;
        }
    }

    if (camera_mode_ == CAMERA_MODE_FREE)
    {
        // Use this frame's mouse motion to adjust camera node yaw and pitch. Clamp the pitch between -90 and 90 degrees
		free_settings_.pitch_ += input_.mouse_y * general_settings_.mouse_sensitivity_ * free_settings_.invert_pitch_ * time_step;
        free_settings_.pitch_ = Urho3D::Clamp(free_settings_.pitch_, -89.0f, 89.0f);
        free_settings_.yaw_ += input_.mouse_x * general_settings_.mouse_sensitivity_ * time_step;
        if (Urho3D::Abs(free_settings_.yaw_) > 180.0f)
        {
            free_settings_.yaw_ = free_settings_.yaw_ + (Urho3D::Sign(free_settings_.yaw_) * 360.0f *  -1.0f);
        }

        // Construct new orientation for the camera scene node from yaw and pitch. Roll is fixed to zero
        node_->SetWorldRotation(Urho3D::Quaternion(free_settings_.pitch_, free_settings_.yaw_, free_settings_.roll_));

        // Read control keys and move the camera scene node to the corresponding direction if they are pressed
        // Use the Translate() function (default local space) to move relative to the node's orientation.
        if (input_.IsKeyDown(KM_CAMERA_FORWARD))
        {
            node_->Translate(Urho3D::Vector3::FORWARD * general_settings_.keyboard_sensitivity_ * time_step);
        }
        else if (input_.IsKeyDown(KM_CAMERA_BACKWARD))
        {
            node_->Translate(Urho3D::Vector3::BACK * general_settings_.keyboard_sensitivity_ * time_step);
        }
        if (input_.IsKeyDown(KM_CAMERA_PAN_LEFT))
        {
            node_->Translate(Urho3D::Vector3::LEFT * general_settings_.keyboard_sensitivity_ * time_step);
        }
        else if (input_.IsKeyDown(KM_CAMERA_PAN_RIGHT))
        {
            node_->Translate(Urho3D::Vector3::RIGHT * general_settings_.keyboard_sensitivity_ * time_step);
        }
        if (input_.IsKeyDown(KM_CAMERA_ROLL_LEFT))
        {
            free_settings_.roll_ += general_settings_.keyboard_sensitivity_ * 5 * time_step;
        }
        else if (input_.IsKeyDown(KM_CAMERA_ROLL_RIGHT))
        {
            free_settings_.roll_ -= general_settings_.keyboard_sensitivity_ * 5 * time_step;
        }
    }

    else if (camera_mode_ == CAMERA_MODE_FOLLOW && camera_type_ == CAMERA_TYPE_VEHICLE)
    {
        if (input_.IsButtonDown(MM_BUTTON_RIGHT))
        {
            is_reseting_ = false;

            vehicle_settings_.pitch_ += input_.mouse_y * general_settings_.mouse_sensitivity_ * vehicle_settings_.invert_pitch_ * time_step;
            vehicle_settings_.pitch_ = Urho3D::Clamp(vehicle_settings_.pitch_, -89.0f, 89.0f);
            vehicle_settings_.yaw_ += input_.mouse_x * general_settings_.mouse_sensitivity_ * time_step;
            if (Urho3D::Abs(vehicle_settings_.yaw_) > 180.0f)
            {
                vehicle_settings_.yaw_ = vehicle_settings_.yaw_ + (Urho3D::Sign(vehicle_settings_.yaw_) * 360.0f *  -1.0f);
            }
        }

        vehicle_settings_.distance_ -= input_.mouse_wheel * 20 * general_settings_.mouse_sensitivity_ * time_step;

        Urho3D::Quaternion camera_rot(lookat_node_->GetWorldRotation());
        camera_rot = camera_rot * Urho3D::Quaternion(vehicle_settings_.yaw_, Urho3D::Vector3::UP);
        camera_rot = camera_rot * Urho3D::Quaternion(vehicle_settings_.pitch_, Urho3D::Vector3::RIGHT);

        Urho3D::Vector3 camera_pos = lookat_node_->GetWorldPosition() - camera_rot * Urho3D::Vector3(0.0f, 0.0f, vehicle_settings_.distance_);

        //  ray cast stuff

        node_->SetWorldPosition(camera_pos);
        node_->SetWorldRotation(camera_rot);
    }

    else if (camera_mode_ == CAMERA_MODE_FOLLOW && camera_type_ == CAMERA_TYPE_CHARACTER)
    {
//printf("%f\n", char_settings.default_distance);
//printf("(%f, %f, %f)\n", lookat_node->GetWorldRotation().YawAngle(), lookat_node->GetWorldRotation().PitchAngle(), lookat_node->GetWorldRotation().RollAngle());
        float p = lookat_node_->GetWorldRotation().PitchAngle();

        if (input_.IsButtonDown(MM_BUTTON_RIGHT))
        {
            is_reseting_ = false;

            character_settings_.pitch_ += input_.mouse_y * general_settings_.mouse_sensitivity_ * character_settings_.invert_pitch_ * time_step;
            character_settings_.pitch_ = Urho3D::Clamp(character_settings_.pitch_, -89.0f, 89.0f);
            character_settings_.yaw_ += input_.mouse_x * general_settings_.mouse_sensitivity_ * time_step;
            if (Urho3D::Abs(character_settings_.yaw_) > 180.0f)
            {
                character_settings_.yaw_ = character_settings_.yaw_ + (Urho3D::Sign(character_settings_.yaw_) * 360.0f *  -1.0f);
            }
        }

        character_settings_.distance_ -= input_.mouse_wheel * 20 * general_settings_.mouse_sensitivity_ * time_step;

        Urho3D::Quaternion camera_rot(lookat_node_->GetWorldRotation());
        camera_rot = camera_rot * Urho3D::Quaternion(-p, Urho3D::Vector3::RIGHT);
        camera_rot = camera_rot * Urho3D::Quaternion(character_settings_.yaw_, Urho3D::Vector3::UP);
        camera_rot = camera_rot * Urho3D::Quaternion(character_settings_.pitch_, Urho3D::Vector3::RIGHT);

        Urho3D::Vector3 camera_pos = lookat_node_->GetWorldPosition() - camera_rot * Urho3D::Vector3(0.0f, 0.0f, character_settings_.distance_);
//printf("(%f, %f, %f)\n", camera_rot.YawAngle(), camera_rot.PitchAngle(), camera_rot.RollAngle());
        //  ray cast stuff

        node_->SetWorldPosition(camera_pos);
        node_->SetWorldRotation(camera_rot);
    }

}

}
