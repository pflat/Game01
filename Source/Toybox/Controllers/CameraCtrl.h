/// \file

#pragma once

#include "../Core/IncGuards.h"
#include "../Input/Controls.h"

THIRD_PARTY_GUARDS_BEGIN
#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Scene/LogicComponent.h>
THIRD_PARTY_GUARDS_END


namespace Toybox
{

class CameraCtrl : public Urho3D::LogicComponent
{
    URHO3D_OBJECT(CameraCtrl, LogicComponent);

    union CameraSettings
    {
		struct
		{
			float keyboard_sensitivity_;
			float mouse_sensitivity_;
			bool smooth_;
			float max_reset_time_;
			float max_retarget_time_;
		};

		struct
		{
			int invert_pitch_;
			float default_pitch_;
			float default_yaw_;
			float default_distance_;
			float pitch_;
			float yaw_;
			float roll_;
			float distance_;
		};
	};

public:

    CameraCtrl(Urho3D::Context* context);

    static void RegisterObject(Urho3D::Context* context);

    virtual void Start();
	virtual void Stop();

    ///  Documentation text here
    void SetType(unsigned type);  //  Space or ground camera
    void SwitchMode();  //  Free or targeted

    void Reset(bool immediate = false);
    void SetTarget(Urho3D::Node* target, bool immediate = false);
//	void Clear();

    void UpdateControls(Urho3D::Input* input, float time_step) { controls_.Update(input); }
    void SetControl(int control, bool active) { controls_.Set(control, active); }

    void UpdateCamera(float time_step);
    Urho3D::Node* GetLookAtNode() { return lookat_node_; }

private:

    Controls controls_;

private:

	///  Node the camera will try to point to
    Urho3D::WeakPtr<Urho3D::Node> target_node_;

	///  Node the camera points to (not in free mode).
	///  This node will follow the target node, for a smooth movement.
    Urho3D::SharedPtr<Urho3D::Node> lookat_node_;

	///  Camera type determines how it will follow and rotate around target
	///  (vehicle | character)
    unsigned camera_type_;

	///  Camera mode is used to switch to free movement, without changing camera type
	///  (free | follow)
    unsigned camera_mode_;

	CameraSettings general_settings_;
	CameraSettings vehicle_settings_;
    CameraSettings character_settings_;
    CameraSettings free_settings_;

	///  User pressed the reset camera button. Camera is moving to default position
    bool is_reseting_;

	///  User changed camera target. Camera is moving toward it
    bool is_retargeting_;

    float retarget_timer_;
    float reset_timer_;

};

}
