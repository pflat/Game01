#pragma once

#include <vector>

#include "Toybox/Core/IncGuards.h"
#include "Toybox/Core/Toybox.h"
#include "Toybox/Core/Debug.h"

THIRD_PARTY_GUARDS_BEGIN
#include <Urho3D/Engine/Application.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Audio/SoundSource.h>
THIRD_PARTY_GUARDS_END


class Game01 : public Urho3D::Application
{

    URHO3D_OBJECT(Game01, Urho3D::Application);

    bool draw_debug;

    unsigned active_scene;
    unsigned change_scene;

	//  main objects - 2 scenes and 1 camera
	Urho3D::SharedPtr<Urho3D::Scene> scene_space;
    Urho3D::SharedPtr<Urho3D::Scene> scene_ground;
    Urho3D::SharedPtr<Toybox::CameraCtrl> camera;


	///  Space scene objects
    Urho3D::WeakPtr<Toybox::Scene> sector;
	std::vector<Urho3D::WeakPtr<Toybox::ShipCtrl> > ships;
	//Urho3D::Vector<Urho3D::WeakPtr<Toybox::SpaceshipCtrl> > ships;
    unsigned num_ships;
    unsigned active_ship;

	///  Ground scene objects
    Urho3D::WeakPtr<Toybox::Scene> zone;
    Urho3D::WeakPtr<Toybox::KinematicCharacterCtrl> kJulia;
    Urho3D::WeakPtr<Toybox::DynamicCharacter> dJulia;
	Urho3D::WeakPtr<Urho3D::Node> terrain_node;
    Urho3D::WeakPtr<Urho3D::CollisionShape> cube_shape1;
    Urho3D::WeakPtr<Urho3D::CollisionShape> cube_shape2;
    Urho3D::WeakPtr<Urho3D::CollisionShape> cube_shape3;

public:

    Game01(Urho3D::Context* context);

    virtual void Setup();
    virtual void Start();
    virtual void Stop();

private:

	void LoadSettings(const Urho3D::String& file_name);
	void SaveSettings(const Urho3D::String& file_name);
    void ListAllResources();

    void CreateSpaceScene();

	///  Used for creating/changing the HUD and saving the layout to a file.
	///  USED ONLY IN DEVELOPMENT.
    void CreateSpaceHUDLayout(const Urho3D::String& out_file);
    void LoadSpaceHUDLayout();
    void UpdateSpaceHUD();

    void CreateGroundScene();
    void CreateGroundHUDLayout(const Urho3D::String& out_file);
    void LoadGroundHUDLayout();
    void UpdateGroundHUD();

    void SwitchScene(unsigned scene);

    void HandleBeginFrame(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data);
    //void HandleKeyDown(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data);
    void HandleKeyUp(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data);
    void HandleUpdate(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data);
    void HandlePostUpdate(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data);
    void HandlePostRenderUpdate(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data);
};
