#include "Game01.h"
#include "Settings.h"
#include "Toybox/Input/InputMap.h"
#include "Toybox/Math/Vector3Ext.h"

THIRD_PARTY_GUARDS_BEGIN
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Core/StringUtils.h>
//#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Engine/EngineDefs.h>
#include <Urho3D/Engine/DebugHud.h>  //  To remove
#include <Urho3D/Scene/SceneEvents.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/AnimationController.h>
#include <Urho3D/Graphics/Terrain.h>
#include <Urho3D/Graphics/Skybox.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/CollisionShape.h> //  To remove
#include <Urho3D/Audio/Audio.h>
#include <Urho3D/Audio/Sound.h>
#include <Urho3D/Audio/SoundSource3D.h>
#include <Urho3D/Audio/SoundListener.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/Font.h>
//#include <Urho3D/UI/Cursor.h>
//#include <Urho3D/UI/Sprite.h>
//#include <Urho3D/Engine/Console.h>
#include <Urho3D/Graphics/DebugRenderer.h> //  To remove
//#include <Urho3D/Input/Input.h>
//#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Resource/ResourceCache.h>
//#include <Urho3D/Graphics/Texture2D.h>
//#include <Urho3D/Core/Timer.h>
//#include <Urho3D/Resource/XMLFile.h>
//#include <Urho3D/IO/Log.h>
THIRD_PARTY_GUARDS_END


URHO3D_DEFINE_APPLICATION_MAIN(Game01);


Game01::Game01(Urho3D::Context* context) :
    Urho3D::Application(context),
    draw_debug(DRAW_DEBUG_GEOMETRY),
    active_scene(0),
    change_scene(0),
    //scene_space(0),
    //scene_ground(0),
    //camera(0),
    num_ships(0),
    active_ship(0)
    //kJulia(0),
    //dJulia(0)
{
    Toybox::Init(context);
/*
    Toybox::CameraCtrl::RegisterObject(context);
    Toybox::ShipCtrl::RegisterObject(context);
    Toybox::CarCtrl::RegisterObject(context);
    Toybox::VehicleWeaponCtrl::RegisterObject(context);
    Toybox::KinematicCharacterCtrl::RegisterObject(context);
    Toybox::DynamicCharacter::RegisterObject(context);
    Toybox::Scene::RegisterObject(context);
*/
}


void Game01::Setup()
{
	CreateDefaultControlMap();
	LoadSettings("game01.settings");

    engineParameters_[Urho3D::EP_WINDOW_TITLE] = GetTypeName();
    //engineParameters_[Urho3D::EP_LOG_NAME] = GetSubsystem<Urho3D::FileSystem>()->GetAppPreferencesDir("urho3d", "logs") + GetTypeName() + ".log";
    engineParameters_[Urho3D::EP_LOG_NAME] = GetSubsystem<Urho3D::FileSystem>()->GetCurrentDir() + "/" + GetTypeName() + ".log";
    engineParameters_[Urho3D::EP_HEADLESS] = false;
    engineParameters_[Urho3D::EP_FULL_SCREEN] = GetGlobalVar("display.fullscreen").GetBool();
    engineParameters_[Urho3D::EP_WINDOW_WIDTH] = GetGlobalVar("display.width").GetInt();
    engineParameters_[Urho3D::EP_WINDOW_HEIGHT] = GetGlobalVar("display.height").GetInt();
    engineParameters_[Urho3D::EP_SOUND] = true;
    engineParameters_[Urho3D::EP_RESOURCE_PREFIX_PATHS] = ".;../../../";
}


void Game01::Start()
{
    if (Urho3D::GetPlatform() == "Android" || Urho3D::GetPlatform() == "iOS" || Urho3D::GetPlatform() == "Web")
        engine_->Exit();

    Urho3D::Renderer* renderer = GetSubsystem<Urho3D::Renderer>();

    int texture_quality = GetGlobalVar("video.texture_quality").GetInt();
    if (texture_quality == 1)
        renderer->SetTextureQuality(Urho3D::QUALITY_LOW);
    else if (texture_quality == 2)
        renderer->SetTextureQuality(Urho3D::QUALITY_MEDIUM);
    else if (texture_quality == 3)
        renderer->SetTextureQuality(Urho3D::QUALITY_HIGH);

    renderer->SetDrawShadows(GetGlobalVar("video.shadows").GetBool());
    int shadow_quality = GetGlobalVar("video.shadows_quality").GetInt();
    if (shadow_quality == 1)
        renderer->SetShadowMapSize(512);
    else if (shadow_quality == 2)
        renderer->SetShadowMapSize(1024);
    else if (shadow_quality == 3)
        renderer->SetShadowMapSize(2048);

    CreateSpaceScene();
	CreateGroundScene();

	//  Set camera
    camera = Toybox::CreateCamera(context_);
    GetSubsystem<Urho3D::Audio>()->SetListener(camera->GetLookAtNode()->GetComponent<Urho3D::SoundListener>());

	SwitchScene(Toybox::SCENE_GROUND);

	//Toybox::SaveTree(context_, apc->GetNode());

//    CreateConsoleAndDebugHud();
    //engine_->CreateDebugHud();

    SubscribeToEvent(Urho3D::E_BEGINFRAME, URHO3D_HANDLER(Game01, HandleBeginFrame));
    //SubscribeToEvent(Urho3D::E_KEYDOWN, URHO3D_HANDLER(Game01, HandleKeyDown));
    SubscribeToEvent(Urho3D::E_KEYUP, URHO3D_HANDLER(Game01, HandleKeyUp));
    //  Subscribe to Update event for setting the vehicle controls before physics simulation
    SubscribeToEvent(Urho3D::E_UPDATE, URHO3D_HANDLER(Game01, HandleUpdate));
    //  Subscribe to PostUpdate event for updating the camera position after physics simulation
    SubscribeToEvent(Urho3D::E_POSTUPDATE, URHO3D_HANDLER(Game01, HandlePostUpdate));
    SubscribeToEvent(Urho3D::E_POSTRENDERUPDATE, URHO3D_HANDLER(Game01, HandlePostRenderUpdate));
    //  Unsubscribe the SceneUpdate event from base class as the camera node is being controlled in HandlePostUpdate() in this sample
    UnsubscribeFromEvent(Urho3D::E_SCENEUPDATE);
}


void Game01::Stop()
{
	camera->Stop();
    //engine_->DumpResources(true);
}


void Game01::LoadSettings(const Urho3D::String& file_name)
{
	SetGlobalVar("display.width", 800);
	SetGlobalVar("display.height", 600);
	SetGlobalVar("display.fullscreen", false);

	SetGlobalVar("video.texture_quality", 2);
	SetGlobalVar("video.shadows", true);
	SetGlobalVar("video.shadows_quality", 2);

	SetGlobalVar("camera.key_sensitivity", 1.0f);
    SetGlobalVar("camera.mouse_sensitivity", 1.0f);
    SetGlobalVar("camera.smooth", true);
	SetGlobalVar("camera.reset_time", 0.2f);
    SetGlobalVar("camera.target_time", 0.2f);

    SetGlobalVar("camera.vehicle.invert_pitch", 1);
    SetGlobalVar("camera.vehicle.default_pitch", 0.0f);
    SetGlobalVar("camera.vehicle.default_yaw", 0.0f);
    SetGlobalVar("camera.vehicle.default_distance", 0.0f);

    SetGlobalVar("camera.character.invert_pitch", 1);
    SetGlobalVar("camera.character.default_pitch", 0.0f);
    SetGlobalVar("camera.character.default_yaw", 0.0f);
    SetGlobalVar("camera.character.default_distance", 0.0f);

    SetGlobalVar("camera.free.invert_pitch", 1);
    SetGlobalVar("camera.free.default_pitch", 0.0f);
    SetGlobalVar("camera.free.default_yaw", 0.0f);
    SetGlobalVar("camera.free.default_distance", 0.0f);

    Urho3D::XMLFile xml_file(context_);
    xml_file.LoadFile(file_name);
	const Urho3D::XMLElement& xml_elem1 = xml_file.GetRoot();

	if (xml_elem1.HasChild("display"))
	{
		const Urho3D::XMLElement& xml_elem2 = xml_elem1.GetChild("display");

		if (xml_elem2.HasAttribute("width"))
			SetGlobalVar("display.width", xml_elem2.GetInt("width"));

		if (xml_elem2.HasAttribute("height"))
			SetGlobalVar("display.height", xml_elem2.GetInt("height"));

		if (xml_elem2.HasAttribute("fullscreen"))
			SetGlobalVar("display.fullscreen", xml_elem2.GetBool("fullscreen"));
	}

	if (xml_elem1.HasChild("video"))
	{
		const Urho3D::XMLElement& xml_elem2 = xml_elem1.GetChild("video");

		if (xml_elem2.HasAttribute("texture_quality"))
			SetGlobalVar("video.texture_quality", xml_elem2.GetInt("texture_quality"));

		if (xml_elem2.HasAttribute("shadows"))
			SetGlobalVar("video.shadows", xml_elem2.GetBool("shadows"));

		if (xml_elem2.HasAttribute("shadows_quality"))
			SetGlobalVar("video.shadows_quality", xml_elem2.GetInt("shadows_quality"));
	}

	if (xml_elem1.HasChild("camera"))
    {
		const Urho3D::XMLElement& xml_elem2 = xml_elem1.GetChild("camera");

		if (xml_elem2.HasAttribute("key_sensitivity"))
				SetGlobalVar("camera.key_sensitivity", xml_elem2.GetFloat("key_sensitivity"));

		if (xml_elem2.HasAttribute("mouse_sensitivity"))
				SetGlobalVar("camera.mouse_sensitivity", xml_elem2.GetFloat("mouse_sensitivity"));

		if (xml_elem2.HasAttribute("smooth"))
				SetGlobalVar("camera.smooth", xml_elem2.GetBool("smooth"));

		if (xml_elem2.HasAttribute("reset_time"))
			SetGlobalVar("camera.reset_time", xml_elem2.GetFloat("reset_time"));

		if (xml_elem2.HasAttribute("target_time"))
			SetGlobalVar("camera.target_time", xml_elem2.GetFloat("target_time"));

		if (xml_elem2.HasChild("vehicle"))
		{
			const Urho3D::XMLElement& xml_elem3 = xml_elem2.GetChild("vehicle");

			if (xml_elem3.HasAttribute("invert_pitch"))
				if (xml_elem3.GetBool("invert_pitch") == true)
					SetGlobalVar("camera.vehicle.invert_pitch", -1);

			if (xml_elem3.HasAttribute("default_pitch"))
				SetGlobalVar("camera.vehicle.default_pitch", xml_elem3.GetFloat("default_pitch"));

			if (xml_elem3.HasAttribute("default_yaw"))
				SetGlobalVar("camera.vehicle.default_yaw", xml_elem3.GetFloat("default_yaw"));

			if (xml_elem3.HasAttribute("default_distance"))
				SetGlobalVar("camera.vehicle.default_distance", xml_elem3.GetFloat("default_distance"));
		}

		if (xml_elem2.HasChild("character"))
		{
			const Urho3D::XMLElement& xml_elem3 = xml_elem2.GetChild("character");

			if (xml_elem3.HasAttribute("invert_pitch"))
				if (xml_elem3.GetBool("invert_pitch") == true)
					SetGlobalVar("camera.character.invert_pitch", -1);

			if (xml_elem3.HasAttribute("default_pitch"))
				SetGlobalVar("camera.character.default_pitch", xml_elem3.GetFloat("default_pitch"));

			if (xml_elem3.HasAttribute("default_yaw"))
				SetGlobalVar("camera.character.default_yaw", xml_elem3.GetFloat("default_yaw"));

			if (xml_elem3.HasAttribute("default_distance"))
				SetGlobalVar("camera.character.default_distance", xml_elem3.GetFloat("default_distance"));
		}

		if (xml_elem2.HasChild("free"))
		{
			const Urho3D::XMLElement& xml_elem3 = xml_elem2.GetChild("free");

			if (xml_elem3.HasAttribute("invert_pitch"))
				if (xml_elem3.GetBool("invert_pitch") == true)
					SetGlobalVar("camera.free.invert_pitch", -1);

			if (xml_elem3.HasAttribute("default_pitch"))
				SetGlobalVar("camera.free.default_pitch", xml_elem3.GetFloat("default_pitch"));

			if (xml_elem3.HasAttribute("default_yaw"))
				SetGlobalVar("camera.free.default_yaw", xml_elem3.GetFloat("default_yaw"));

			if (xml_elem3.HasAttribute("default_distance"))
				SetGlobalVar("camera.free.default_distance", xml_elem3.GetFloat("default_distance"));
		}
    }
}


void Game01::SaveSettings(const Urho3D::String& file_name)
{
}


void Game01::CreateDefaultControlMap()
{
	//  Debug controls
    Toybox::InputMap::Set("Debug.TextureQuality", Urho3D::QUAL_CTRL, Urho3D::KEY_F1);
    Toybox::InputMap::Set("Debug.MaterialQuality", Urho3D::QUAL_CTRL, Urho3D::KEY_F2);
    Toybox::InputMap::Set("Debug.SpecularLighting", Urho3D::QUAL_CTRL, Urho3D::KEY_F3);
    Toybox::InputMap::Set("Debug.ShadowRendering", Urho3D::QUAL_CTRL, Urho3D::KEY_F4);
    Toybox::InputMap::Set("Debug.ShadowResolution", Urho3D::QUAL_CTRL, Urho3D::KEY_F5);
    Toybox::InputMap::Set("Debug.ShadowFiltering", Urho3D::QUAL_CTRL, Urho3D::KEY_F6);
    Toybox::InputMap::Set("Debug.OcclusionCulling", Urho3D::QUAL_CTRL, Urho3D::KEY_F7);
    Toybox::InputMap::Set("Debug.DynamicInstancing", Urho3D::QUAL_CTRL, Urho3D::KEY_F8);
    Toybox::InputMap::Set("Debug.DebugGeometry", Urho3D::QUAL_CTRL, Urho3D::KEY_F9);
    Toybox::InputMap::Set("Debug.DebugHUD", Urho3D::QUAL_CTRL, Urho3D::KEY_F10);

    //  General controls
    Toybox::InputMap::Set("General.SceneSwitch", Urho3D::QUAL_CTRL, Urho3D::KEY_TAB);
	Toybox::InputMap::Set("General.CameraReset", Urho3D::QUAL_NONE, Urho3D::KEY_C);
	Toybox::InputMap::Set("General.CameraFree", Urho3D::QUAL_NONE, Urho3D::KEY_F);
	Toybox::InputMap::Set("General.ScreenShot", Urho3D::QUAL_NONE, Urho3D::KEY_PRINTSCREEN);
	Toybox::InputMap::Set("General.Quit", Urho3D::QUAL_NONE, Urho3D::KEY_ESCAPE);

	Toybox::InputMap::Set("General.PrevShip", Urho3D::QUAL_NONE, Urho3D::KEY_PAGEDOWN);
	Toybox::InputMap::Set("General.NextShip", Urho3D::QUAL_NONE, Urho3D::KEY_PAGEUP);

	//  Ship controls
	Toybox::InputMap::Set("Ship.XMovDec", Urho3D::QUAL_SHIFT, Urho3D::KEY_Q);  //  Strafe left
	Toybox::InputMap::Set("Ship.XMovInc", Urho3D::QUAL_SHIFT, Urho3D::KEY_E);  //  Strafe right
	//Toybox::InputMap::Set("Ship.XMovMax", Urho3D::QUAL_NONE, Urho3D::KEY_UNKNOWN);
	//Toybox::InputMap::Set("Ship.XMovMin", Urho3D::QUAL_NONE, Urho3D::KEY_UNKNOWN);
	//Toybox::InputMap::Set("Ship.XMovStp", Urho3D::QUAL_NONE, Urho3D::KEY_UNKNOWN);
	Toybox::InputMap::Set("Ship.XRotInc", Urho3D::QUAL_NONE, Urho3D::KEY_S);  //  Pitch down
	Toybox::InputMap::Set("Ship.XRotDec", Urho3D::QUAL_NONE, Urho3D::KEY_W);  //  Pitch up
	//Toybox::InputMap::Set("Ship.XRotMax", Urho3D::QUAL_NONE, Urho3D::KEY_UNKNOWN);
	//Toybox::InputMap::Set("Ship.XRotMin", Urho3D::QUAL_NONE, Urho3D::KEY_UNKNOWN);
	//Toybox::InputMap::Set("Ship.XRotStp", Urho3D::QUAL_NONE, Urho3D::KEY_UNKNOWN);
	Toybox::InputMap::Set("Ship.YMovInc", Urho3D::QUAL_SHIFT, Urho3D::KEY_KP_PLUS);  //  Lift up
	Toybox::InputMap::Set("Ship.YMovDec", Urho3D::QUAL_SHIFT, Urho3D::KEY_KP_MINUS);  //  Lift down
	Toybox::InputMap::Set("Ship.YRotInc", Urho3D::QUAL_NONE, Urho3D::KEY_A);  //  Yaw left
	Toybox::InputMap::Set("Ship.YRotDec", Urho3D::QUAL_NONE, Urho3D::KEY_D);  //  Yaw right
	Toybox::InputMap::Set("Ship.ZMovInc", Urho3D::QUAL_NONE, Urho3D::KEY_KP_PLUS);  //  Thrust up
	Toybox::InputMap::Set("Ship.ZMovDec", Urho3D::QUAL_NONE, Urho3D::KEY_KP_MINUS);  //  Thrust down
	Toybox::InputMap::Set("Ship.ZMovMax", Urho3D::QUAL_NONE, Urho3D::KEY_TAB);  //  Thrust max
	//Toybox::InputMap::Set("Ship.ZMovMin", Urho3D::QUAL_NONE, Urho3D::KEY_UNKNOWN);  //  Thrust min
	Toybox::InputMap::Set("Ship.ZMovStp", Urho3D::QUAL_NONE, Urho3D::KEY_BACKSPACE);  //  Stop
	Toybox::InputMap::Set("Ship.ZRotInc", Urho3D::QUAL_NONE, Urho3D::KEY_Q);  //  Roll left
	Toybox::InputMap::Set("Ship.ZRotDec", Urho3D::QUAL_NONE, Urho3D::KEY_E);  //  Roll right

	//  Car controls
	Toybox::InputMap::Set("Car.ZMovInc", Urho3D::QUAL_NONE, Urho3D::KEY_W, Urho3D::MOUSEB_NONE, true);  //  Accelerate
	Toybox::InputMap::Set("Car.ZMovDec", Urho3D::QUAL_NONE, Urho3D::KEY_S, Urho3D::MOUSEB_NONE, true);  //  De-accelerate
	Toybox::InputMap::Set("Car.ZMovStp", Urho3D::QUAL_NONE, Urho3D::KEY_SPACE, Urho3D::MOUSEB_NONE, true);  //  Handbrake
	Toybox::InputMap::Set("Car.YRotInc", Urho3D::QUAL_NONE, Urho3D::KEY_D, Urho3D::MOUSEB_NONE, true);  //  Turn right
	Toybox::InputMap::Set("Car.YRotDec", Urho3D::QUAL_NONE, Urho3D::KEY_A, Urho3D::MOUSEB_NONE, true);  //  Turn left

	//  Character controls
	Toybox::InputMap::Set("Character.Forward", Urho3D::QUAL_NONE, Urho3D::KEY_W, Urho3D::MOUSEB_NONE, true);
	Toybox::InputMap::Set("Character.Backward", Urho3D::QUAL_NONE, Urho3D::KEY_S, Urho3D::MOUSEB_NONE, true);
	Toybox::InputMap::Set("Character.TurnLeft", Urho3D::QUAL_NONE, Urho3D::KEY_A, Urho3D::MOUSEB_NONE, true);
	Toybox::InputMap::Set("Character.TurnRight", Urho3D::QUAL_NONE, Urho3D::KEY_D, Urho3D::MOUSEB_NONE, true);
	Toybox::InputMap::Set("Character.StrafeLeft", Urho3D::QUAL_NONE, Urho3D::KEY_Q, Urho3D::MOUSEB_NONE, true);
	Toybox::InputMap::Set("Character.StrafeRight", Urho3D::QUAL_NONE, Urho3D::KEY_E, Urho3D::MOUSEB_NONE, true);
	Toybox::InputMap::Set("Character.Jump", Urho3D::QUAL_NONE, Urho3D::KEY_SPACE, Urho3D::MOUSEB_NONE, true);
	Toybox::InputMap::Set("Character.Walk", Urho3D::QUAL_SHIFT, Urho3D::KEY_UNKNOWN);
	Toybox::InputMap::Set("Character.Crouch", Urho3D::QUAL_NONE, Urho3D::KEY_C);

	//  Camera controls
	Toybox::InputMap::Set("Camera.Move", Urho3D::QUAL_NONE, Urho3D::KEY_UNKNOWN, Urho3D::MOUSEB_RIGHT, true);
	Toybox::InputMap::Set("Camera.Forward", Urho3D::QUAL_NONE, Urho3D::KEY_I);
	Toybox::InputMap::Set("Camera.Backward", Urho3D::QUAL_NONE, Urho3D::KEY_K);
	Toybox::InputMap::Set("Camera.PanLeft", Urho3D::QUAL_NONE, Urho3D::KEY_J);
	Toybox::InputMap::Set("Camera.PanRight", Urho3D::QUAL_NONE, Urho3D::KEY_L);
	Toybox::InputMap::Set("Camera.RollLeft", Urho3D::QUAL_NONE, Urho3D::KEY_U);
	Toybox::InputMap::Set("Camera.RollRight", Urho3D::QUAL_NONE, Urho3D::KEY_O);
}


void Game01::CreateSpaceScene()
{
    Urho3D::ResourceCache* cache = GetSubsystem<Urho3D::ResourceCache>();

    scene_space = new Urho3D::Scene(context_);
    scene_space->CreateComponent<Urho3D::DebugRenderer>();
    scene_space->CreateComponent<Urho3D::Octree>();
    Urho3D::PhysicsWorld* world = scene_space->CreateComponent<Urho3D::PhysicsWorld>();
    world->SetGravity(Urho3D::Vector3(0.0f, 0.0f, 0.0f));

    sector = Toybox::LoadScene("sectors/space/corona.xml", scene_space, cache);

    //Toybox::ShipCtrl* ship1 = Toybox::LoadShip("vehicles/space/fighter_a01.xml", cache, scene_space));
    //ship1->GetNode()->SetPosition(Urho3D::Vector3(-20.0f, 0.0f, 10.0f));

    Toybox::ShipCtrl* ship2 = Toybox::LoadShip("vehicles/space/fighter_a02.xml", cache, scene_space);
    ship2->GetNode()->SetPosition(Urho3D::Vector3(-10.0f, 0.0f, 10.0f));

    //Toybox::ShipCtrl* ship3(Toybox::LoadShip("vehicles/space/fighter_a03.xml", cache, scene_space));
    //ship3->GetNode()->SetPosition(Urho3D::Vector3(0.0f, 0.0f, 10.0f));

    Toybox::ShipCtrl* ship4(Toybox::LoadShip("vehicles/space/fighter_a04.xml", cache, scene_space));
    ship4->GetNode()->SetPosition(Urho3D::Vector3(10.0f, 0.0f, 10.0f));

    //Toybox::ShipCtrl* ship5(Toybox::LoadShip("vehicles/space/fighter_a05.xml", cache, scene_space));
    //ship5->GetNode()->SetPosition(Urho3D::Vector3(20.0f, 0.0f, 10.0f));

	//ships.Push(Urho3D::WeakPtr<Toybox::ShipCtrl>(ship1));
	ships.Push(Urho3D::WeakPtr<Toybox::ShipCtrl>(ship2));
    //ships.Push(Urho3D::WeakPtr<Toybox::ShipCtrl>(ship3));
	ships.Push(Urho3D::WeakPtr<Toybox::ShipCtrl>(ship4));
	//ships.Push(Urho3D::WeakPtr<Toybox::ShipCtrl>(ship5));

	//ships.push_back(Urho3D::WeakPtr<Toybox::ShipCtrl>(ship1));
	//ships.push_back(Urho3D::WeakPtr<Toybox::ShipCtrl>(ship2));
    //ships.push_back(Urho3D::WeakPtr<Toybox::ShipCtrl>(ship3));
	//ships.push_back(Urho3D::WeakPtr<Toybox::ShipCtrl>(ship4));
	//ships.push_back(Urho3D::WeakPtr<Toybox::ShipCtrl>(ship5));

    active_ship = 0;
	num_ships = 2;

//    Toybox::VehicleWeaponCtrl* weapon01 = Toybox::LoadWeapon("weapons/space/cannon_a01.xml", scene_space, cache);
//    ship1->AddWeapon(weapon01, "nSlot01");

//    Toybox::VehicleWeaponCtrl* weapon02 = Toybox::LoadWeapon("weapons/space/cannon_a01.xml", scene_space, cache);
//    ship1->AddWeapon(weapon02, "nSlot02");

//    Toybox::VehicleWeaponCtrl* weapon03 = Toybox::LoadWeapon("weapons/space/cannon_a02.xml", scene_space, cache);
//    ship2->AddWeapon(weapon03, "nSlot03");

//    Toybox::VehicleWeaponCtrl* weapon04 = Toybox::LoadWeapon("weapons/space/cannon_a02.xml", scene_space, cache);
//    ship2->AddWeapon(weapon04, "nSlot04");

    //PrintNodeTree(0, ships[active_ship]->GetNode());
}


void Game01::CreateSpaceHUDLayout(const Urho3D::String& out_file)
{
    Urho3D::ResourceCache* cache = GetSubsystem<Urho3D::ResourceCache>();
    Urho3D::UI* ui = GetSubsystem<Urho3D::UI>();

    Urho3D::UIElement* main_canvas = ui->GetRoot()->CreateChild<Urho3D::UIElement>();
    main_canvas->SetName("ctnMain");
    main_canvas->SetMinOffset(Urho3D::IntVector2(0, 0));
    main_canvas->SetMaxOffset(Urho3D::IntVector2(0, 0));
    main_canvas->SetMinAnchor(0.0f, 0.0f);
    main_canvas->SetMaxAnchor(1.0f, 1.0f);
    main_canvas->SetEnableAnchor(true);


    Urho3D::Text* lblSpeed = main_canvas->CreateChild<Urho3D::Text>();
    lblSpeed->SetName("lblSpeed");
    lblSpeed->SetText("TXT_SPEED");
    lblSpeed->SetFont(cache->GetResource<Urho3D::Font>("fonts/orbitron-medium.ttf"), 12);
    lblSpeed->SetMinOffset(Urho3D::IntVector2(10, 10));
    lblSpeed->SetMaxOffset(Urho3D::IntVector2(10, 10));
    lblSpeed->SetMinAnchor(0.0f, 0.0f);
    lblSpeed->SetMaxAnchor(0.2f, 0.0f);
    lblSpeed->SetEnableAnchor(true);



    Urho3D::Text* lblHelp = main_canvas->CreateChild<Urho3D::Text>();
    lblHelp->SetName("lblHelp");
    lblHelp->SetText(
		"W/S - Ship pitch      A/S - Ship yaw      Q/E - Ship roll\n"
		"Keypad+ / Keypad- Ship speed      TAB - Full speed      BACKSPACE - Stop\n"
		"Mouse move + RMB - Move camera      Mouse wheel - Zoom camera\n"
		"PageUp/PageDown - Change ship\n"
		"ESC - Exit"
    );
    lblHelp->SetFont(cache->GetResource<Urho3D::Font>("fonts/orbitron-medium.ttf"), 12);
    lblHelp->SetTextAlignment(Urho3D::HA_CENTER);
    lblHelp->SetMinOffset(Urho3D::IntVector2(10, 10));
    lblHelp->SetMaxOffset(Urho3D::IntVector2(10, 10));
    lblHelp->SetMinAnchor(0.0f, 0.8f);
    lblHelp->SetMaxAnchor(1.0f, 1.0f);
    lblHelp->SetEnableAnchor(true);


    Urho3D::File file(context_);
    file.Open(out_file, Urho3D::FILE_WRITE);
    ui->SaveLayout(file, main_canvas);
    file.Close();
}


void Game01::LoadSpaceHUDLayout()
{
    Urho3D::ResourceCache* cache = GetSubsystem<Urho3D::ResourceCache>();
    Urho3D::UI* ui = GetSubsystem<Urho3D::UI>();

    ui->Clear();
    Urho3D::SharedPtr<Urho3D::UIElement> loaded_root = ui->LoadLayout(cache->GetResource<Urho3D::XMLFile>("ui/hud_space.xml"));
    ui->GetRoot()->AddChild(loaded_root);
}


void Game01::UpdateSpaceHUD()
{
    Urho3D::UIElement* ui = GetSubsystem<Urho3D::UI>()->GetRoot();
    Urho3D::Text* lblSpeed = static_cast<Urho3D::Text*>(ui->GetChild("lblSpeed", true));
    //lblSpeed->SetText(Urho3D::String(ships[active_ship]->Thrust()));
    lblSpeed->SetText(Urho3D::String((int)ships[active_ship]->SpeedKmh()));
}


void Game01::CreateGroundScene()
{
    Urho3D::ResourceCache* cache = GetSubsystem<Urho3D::ResourceCache>();

    scene_ground = new Urho3D::Scene(context_);
    scene_ground->CreateComponent<Urho3D::DebugRenderer>();
    scene_ground->CreateComponent<Urho3D::Octree>();
    Urho3D::PhysicsWorld* world = scene_ground->CreateComponent<Urho3D::PhysicsWorld>();
    world->SetGravity(Urho3D::Vector3(0.0f, -9.81f, 0.0f));

    zone = Toybox::LoadScene("sectors/ground/interstellar.xml", scene_ground, cache);

    //  terrain surface
    terrain_node = scene_ground->CreateChild("zone001");
    Urho3D::Terrain* terrain = terrain_node->CreateComponent<Urho3D::Terrain>();

    terrain->SetPatchSize(64);
    terrain->SetSpacing(Urho3D::Vector3(1.0f, 0.5f, 1.0f)); // Spacing between vertices and vertical resolution of the height map
    //terrain->SetSmoothing(true);
    terrain->SetOccluder(true);

    terrain->SetHeightMap(cache->GetResource<Urho3D::Image>("areas/zone001/heightmap.png"));
    terrain->SetMaterial(cache->GetResource<Urho3D::Material>("materials/terrain/zone001.xml"));

    Urho3D::RigidBody* terrain_body = terrain_node->CreateComponent<Urho3D::RigidBody>();
    terrain_body->SetCollisionLayer(Toybox::CL_TERRAIN);
    Urho3D::CollisionShape* terrain_shape = terrain_node->CreateComponent<Urho3D::CollisionShape>();
    terrain_shape->SetTerrain();

    //Urho3D::CollisionShape* coll_shape = julia->GetComponent<Urho3D::CollisionShape>();
    terrain_shape->DrawDebugGeometry(scene_ground->GetComponent<Urho3D::DebugRenderer>(), false);



//----------------------------------------------------------
//----------------------------------------------------------
    Urho3D::Node* cube_node1 = scene_ground->CreateChild("cube1");
    cube_node1->SetPosition(Urho3D::Vector3(20.0f, 1.8f, 25.0f));

    Urho3D::StaticModel* cube_model1 = cube_node1->CreateComponent<Urho3D::StaticModel>();
    cube_model1->SetModel(cache->GetResource<Urho3D::Model>("models/architecture/cube/cube.mdl"));
    cube_model1->SetMaterial(cache->GetResource<Urho3D::Material>("materials/architecture/cube.xml"));
    cube_model1->SetCastShadows(true);

    Urho3D::RigidBody* cube_body1 = cube_node1->CreateComponent<Urho3D::RigidBody>();
    cube_body1->SetCollisionLayer(Toybox::CL_STATIC);
    cube_body1->SetMass(0.0f);
    /*Urho3D::CollisionShape**/ cube_shape1 = cube_node1->CreateComponent<Urho3D::CollisionShape>();
    cube_shape1->SetTriangleMesh(cache->GetResource<Urho3D::Model>("models/architecture/cube/collision.mdl"));
//----------------------------------------------------------
//----------------------------------------------------------
    Urho3D::Node* cube_node2 = scene_ground->CreateChild("cube2");
    cube_node2->SetPosition(Urho3D::Vector3(20.0f, 4.8f, 25.0f));

    Urho3D::StaticModel* cube_model2 = cube_node2->CreateComponent<Urho3D::StaticModel>();
    cube_model2->SetModel(cache->GetResource<Urho3D::Model>("models/architecture/cube/cube.mdl"));
    cube_model2->SetMaterial(cache->GetResource<Urho3D::Material>("materials/architecture/cube.xml"));
    cube_model2->SetCastShadows(true);

    Urho3D::RigidBody* cube_body2 = cube_node2->CreateComponent<Urho3D::RigidBody>();
    cube_body2->SetCollisionLayer(Toybox::CL_STATIC);
    cube_body2->SetMass(0.0f);
    /*Urho3D::CollisionShape**/ cube_shape2 = cube_node2->CreateComponent<Urho3D::CollisionShape>();
    cube_shape2->SetTriangleMesh(cache->GetResource<Urho3D::Model>("models/architecture/cube/collision.mdl"));
//----------------------------------------------------------
//----------------------------------------------------------
    Urho3D::Node* cube_node3 = scene_ground->CreateChild("cube3");
    cube_node3->SetPosition(Urho3D::Vector3(25.0f, 1.8f, 30.0f));
    cube_node3->SetRotation(Urho3D::Quaternion(-75.0f, 0.0f, 0.0f));

    Urho3D::StaticModel* cube_model3 = cube_node3->CreateComponent<Urho3D::StaticModel>();
    cube_model3->SetModel(cache->GetResource<Urho3D::Model>("models/architecture/cube/cube.mdl"));
    cube_model3->SetMaterial(cache->GetResource<Urho3D::Material>("materials/architecture/cube.xml"));
    cube_model3->SetCastShadows(true);

    Urho3D::RigidBody* cube_body3 = cube_node3->CreateComponent<Urho3D::RigidBody>();
    cube_body3->SetCollisionLayer(Toybox::CL_STATIC);
    cube_body3->SetMass(0.0f);
    /*Urho3D::CollisionShape**/ cube_shape3 = cube_node3->CreateComponent<Urho3D::CollisionShape>();
    cube_shape3->SetTriangleMesh(cache->GetResource<Urho3D::Model>("models/architecture/cube/collision.mdl"));
//----------------------------------------------------------
//----------------------------------------------------------
    Urho3D::Node* cube_node4 = scene_ground->CreateChild("cube4");
    cube_node4->SetPosition(Urho3D::Vector3(30.0f, 0.4f, 35.0f));
    cube_node4->SetScale(Urho3D::Vector3(2.0f, 1.0f, 2.0f));

    Urho3D::StaticModel* cube_model4 = cube_node4->CreateComponent<Urho3D::StaticModel>();
    cube_model4->SetModel(cache->GetResource<Urho3D::Model>("models/architecture/cube/cube.mdl"));
    cube_model4->SetMaterial(cache->GetResource<Urho3D::Material>("materials/architecture/cube.xml"));
    cube_model4->SetCastShadows(true);

    Urho3D::RigidBody* cube_body4 = cube_node4->CreateComponent<Urho3D::RigidBody>();
    cube_body4->SetCollisionLayer(Toybox::CL_STATIC);
    cube_body4->SetMass(0.0f);
    /*Urho3D::CollisionShape**/ cube_shape4 = cube_node4->CreateComponent<Urho3D::CollisionShape>();
    cube_shape4->SetTriangleMesh(cache->GetResource<Urho3D::Model>("models/architecture/cube/collision.mdl"));
//----------------------------------------------------------
//----------------------------------------------------------

    if (CHAR_CONTROLLER == Toybox::CHAR_CONTROLLER_KINEMATIC)
    {
        kJulia = Toybox::LoadKinematicCharacter("characters/human/julia.xml", cache, scene_ground);
        //kJulia->GetNode()->SetPosition(Urho3D::Vector3(0.0f, 35.04f, 0.0f));
        kJulia->GetNode()->SetPosition(Urho3D::Vector3(20.0f, 3.0f, 25.0f));
        //kJulia->GetNode()->SetPosition(Urho3D::Vector3(21.518015f, 2.433016f, 22.795750f));
        //kJulia->GetNode()->SetPosition(Urho3D::Vector3(15.0f, 5.2f, 25.0f));
        //PrintNodeTree(0, kJulia->GetNode());
        //kJulia->GetNode()->SetRotation(Urho3D::Quaternion(0.0f, 0.0f, 15.0f));
    }
    else
    {
        dJulia = Toybox::LoadDynamicCharacter("characters/human/julia.xml", scene_ground, cache);
        dJulia->GetNode()->SetPosition(Urho3D::Vector3(20.0f, 5.0f, 25.0f));
        //PrintNodeTree(0, dJulia->GetNode());
    }
/*
    // spin node
    //Urho3D::Node* adjustNode = objectNode->CreateChild("AdjNode");
    //adjustNode->SetRotation(Urho3D::Quaternion(180, Urho3D::Vector3(0,1,0) ) );
*/



    // Set the head bone for manual control
    //object->GetSkeleton().GetBone("Mutant:Head")->animated_ = false;
    //Urho3D::Skeleton skel = object->GetSkeleton();

    // Set the rigidbody to signal collision also when in rest, so that we get ground collisions properly
    //body->SetCollisionEventMode(Urho3D::COLLISION_ALWAYS);


    apc = Toybox::LoadCar("vehicles/ground/apc_a01.xml", cache, scene_ground);
    apc->GetNode()->SetPosition(Urho3D::Vector3(30.0f, 1.4f, 35.0f));

}


void Game01::CreateGroundHUDLayout(const Urho3D::String& out_file)
{
    Urho3D::ResourceCache* cache = GetSubsystem<Urho3D::ResourceCache>();
    Urho3D::UI* ui = GetSubsystem<Urho3D::UI>();

    ui->Clear();

    Urho3D::UIElement* main_canvas = ui->GetRoot()->CreateChild<Urho3D::UIElement>();
    main_canvas->SetName("ctnMain");
    main_canvas->SetMinOffset(Urho3D::IntVector2(0, 0));
    main_canvas->SetMaxOffset(Urho3D::IntVector2(0, 0));
    main_canvas->SetMinAnchor(0.0f, 0.0f);
    main_canvas->SetMaxAnchor(1.0f, 1.0f);
    main_canvas->SetEnableAnchor(true);


    Urho3D::Text* lblSpeed = main_canvas->CreateChild<Urho3D::Text>();
    lblSpeed->SetName("lblSpeed");
    lblSpeed->SetText("TXT_SPEED");
    lblSpeed->SetFont(cache->GetResource<Urho3D::Font>("fonts/baloo-regular.ttf"), 12);
    lblSpeed->SetMinOffset(Urho3D::IntVector2(10, 10));
    lblSpeed->SetMaxOffset(Urho3D::IntVector2(10, 10));
    lblSpeed->SetMinAnchor(0.0f, 0.0f);
    lblSpeed->SetMaxAnchor(0.2f, 0.0f);
    lblSpeed->SetEnableAnchor(true);

    //Urho3D::File file(context_);
    //file.Open(out_file, Urho3D::FILE_WRITE);
    //ui->SaveLayout(file, main_canvas);
    //file.Close();
}


void Game01::LoadGroundHUDLayout()
{
    Urho3D::ResourceCache* cache = GetSubsystem<Urho3D::ResourceCache>();
    Urho3D::UI* ui = GetSubsystem<Urho3D::UI>();

    ui->Clear();
    Urho3D::SharedPtr<Urho3D::UIElement> loaded_root = ui->LoadLayout(cache->GetResource<Urho3D::XMLFile>("ui/hud_ground.xml"));
    ui->GetRoot()->AddChild(loaded_root);
}


void Game01::UpdateGroundHUD()
{
    Urho3D::UIElement* ui = GetSubsystem<Urho3D::UI>()->GetRoot();
    Urho3D::Text* lblSpeed = static_cast<Urho3D::Text*>(ui->GetChild("lblSpeed", true));
    //lblSpeed->SetText(Urho3D::String(ships[active_ship]->Thrust()));
    //lblSpeed->SetText(Urho3D::String((int)ships[active_ship]->SpeedKmh()));
    lblSpeed->SetText(Urho3D::String((int)apc->GetSpeed()));
}


void Game01::SwitchScene(unsigned scene)
{
    active_scene = scene;
    if (active_scene == Toybox::SCENE_SPACE)
    {
        zone->SendToBack(false);
        sector->BringToFront();

        //CreateSpaceHUDLayout("C:/Data/UI/SpaceHUD2.xml");
        LoadSpaceHUDLayout();

        camera->SetType(Toybox::CAMERA_TYPE_VEHICLE);
        camera->SetTarget(ships[active_ship]->GetCameraNode(), true);
        camera->Reset(true);
        GetSubsystem<Urho3D::Renderer>()->SetViewport(0, new Urho3D::Viewport(context_, scene_space, camera->GetNode()->GetComponent<Urho3D::Camera>()));
    }
    else if (active_scene == Toybox::SCENE_GROUND)
    {
        sector->SendToBack(false);
        zone->BringToFront();

        //CreateGroundHUDLayout("C:/Data/UI/SpaceHUD2.xml");
        LoadGroundHUDLayout();

        camera->SetType(Toybox::CAMERA_TYPE_CHARACTER);
        if (CHAR_CONTROLLER == Toybox::CHAR_CONTROLLER_KINEMATIC)
            //camera->SetTarget(kJulia->GetCameraNode(), true);
            camera->SetTarget(apc->GetCameraNode(), true);
        else
            camera->SetTarget(dJulia->GetCameraNode(), true);
        camera->Reset(true);
        GetSubsystem<Urho3D::Renderer>()->SetViewport(0, new Urho3D::Viewport(context_, scene_ground, camera->GetNode()->GetComponent<Urho3D::Camera>()));
    }
}


void Game01::HandleBeginFrame(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data)
{
    if (change_scene == 1)
    {
        change_scene = 0;
        if (active_scene == Toybox::SCENE_GROUND)
        {
            SwitchScene(Toybox::SCENE_SPACE);
        }
        else
        {
            SwitchScene(Toybox::SCENE_GROUND);
        }
    }
}


//void Game01::HandleKeyDown(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data)
//{
//}


void Game01::HandleKeyUp(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data)
{
    Urho3D::Input* input = GetSubsystem<Urho3D::Input>();
    Urho3D::Renderer* renderer = GetSubsystem<Urho3D::Renderer>();

    Urho3D::QualifierFlags qualifiers = input->GetQualifiers();
    Urho3D::Key key = static_cast<Urho3D::Key>(event_data[Urho3D::KeyUp::P_KEY].GetInt());

    //  Texture quality
	if (Toybox::InputMap::Test("Debug.TextureQuality", qualifiers, key))
    {
        Urho3D::MaterialQuality quality = renderer->GetTextureQuality();
        if (quality == Urho3D::QUALITY_LOW)
            quality = Urho3D::QUALITY_MEDIUM;
        else if (quality == Urho3D::QUALITY_MEDIUM)
            quality = Urho3D::QUALITY_HIGH;
        else if (quality == Urho3D::QUALITY_HIGH)
            quality = Urho3D::QUALITY_LOW;
        renderer->SetTextureQuality(quality);
    }

    //  Material quality
    else if (Toybox::InputMap::Test("Debug.MaterialQuality", qualifiers, key))
    {
        Urho3D::MaterialQuality quality = renderer->GetMaterialQuality();
        if (quality == Urho3D::QUALITY_LOW)
            quality = Urho3D::QUALITY_MEDIUM;
        else if (quality == Urho3D::QUALITY_MEDIUM)
            quality = Urho3D::QUALITY_HIGH;
        else if (quality == Urho3D::QUALITY_HIGH)
            quality = Urho3D::QUALITY_LOW;
        renderer->SetMaterialQuality(quality);
    }

    //  Specular lighting
	else if (Toybox::InputMap::Test("Debug.SpecularLighting", qualifiers, key))
    {
        renderer->SetSpecularLighting(!renderer->GetSpecularLighting());
    }

    //  Shadow rendering
    else if (Toybox::InputMap::Test("Debug.ShadowRendering", qualifiers, key))
    {
        renderer->SetDrawShadows(!renderer->GetDrawShadows());
    }

    //  Shadow map resolution
    else if (Toybox::InputMap::Test("Debug.ShadowResolution", qualifiers, key))
    {
        int shadowMapSize = renderer->GetShadowMapSize();
        shadowMapSize *= 2;
        if (shadowMapSize > 2048)
            shadowMapSize = 512;
        renderer->SetShadowMapSize(shadowMapSize);
    }

    //  Shadow depth and filtering quality
    else if (Toybox::InputMap::Test("Debug.ShadowFiltering", qualifiers, key))
    {
        Urho3D::ShadowQuality quality = renderer->GetShadowQuality();
        quality = (Urho3D::ShadowQuality)(quality + 1);
        if (quality > Urho3D::SHADOWQUALITY_BLUR_VSM)
            quality = Urho3D::SHADOWQUALITY_SIMPLE_16BIT;
        renderer->SetShadowQuality(quality);
    }

    //  Occlusion culling
    else if (Toybox::InputMap::Test("Debug.OcclusionCulling", qualifiers, key))
    {
        bool occlusion = renderer->GetMaxOccluderTriangles() > 0;
        occlusion = !occlusion;
        renderer->SetMaxOccluderTriangles(occlusion ? 5000 : 0);
    }

    //  Instancing
    else if (Toybox::InputMap::Test("Debug.DynamicInstancing", qualifiers, key))
    {
        renderer->SetDynamicInstancing(!renderer->GetDynamicInstancing());
    }

    //  Draw debug geometry
    else if (Toybox::InputMap::Test("Debug.DebugGeometry", qualifiers, key))
    {
        draw_debug = !draw_debug;
    }

    //  Print debug information
    else if (Toybox::InputMap::Test("Debug.DebugHUD", qualifiers, key))
    {
        GetSubsystem<Urho3D::DebugHud>()->ToggleAll();
    }

    //  Switch between scenes (SPACE / GROUND)
    else if (Toybox::InputMap::Test("General.SceneSwitch", qualifiers, key))
    {
        change_scene = 1;
    }

    // Close console (if open) or exit when ESC is pressed
    if (Toybox::InputMap::Test("General.Quit", qualifiers, key))
    {
        //Urho3D::Console* console = GetSubsystem<Urho3D::Console>();
        //if (console->IsVisible())
        //    console->SetVisible(false);
        //else
            engine_->Exit();
    }

    else if (Toybox::InputMap::Test("General.ScreenShot", qualifiers, key))
    {
        Urho3D::Graphics* graphics = GetSubsystem<Urho3D::Graphics>();
        Urho3D::Image screenshot(context_);
        graphics->TakeScreenShot(screenshot);

        Urho3D::FileSystem* fs = GetSubsystem<Urho3D::FileSystem>();
        if (!fs->DirExists(fs->GetCurrentDir() + "/Screenshots"))
        {
            fs->CreateDir(fs->GetCurrentDir() + "/Screenshots");
        }
        Urho3D::Vector<Urho3D::String> files;
        fs->ScanDir(files, fs->GetCurrentDir() + "/Screenshots", "Screenshot*.png", Urho3D::SCAN_FILES | Urho3D::SCAN_HIDDEN, false);

        unsigned largest_ss = 0;
        for (unsigned i = 0; i < files.Size(); ++i)
        {
            largest_ss = Urho3D::Max(largest_ss, Urho3D::ToUInt(files[i].Substring(files[i].Find('_') + 1, 4), 10));
        }

        char ss_number[5];
        sprintf(ss_number, "%04d", largest_ss + 1);
        screenshot.SavePNG(fs->GetCurrentDir() + "/Screenshots/" + "Screenshot_" + ss_number + ".png");
    }
    else if (Toybox::InputMap::Test("General.CameraReset", qualifiers, key))
    {
        camera->Reset();
    }
    else if (Toybox::InputMap::Test("General.CameraFree", qualifiers, key))
    {
        camera->SwitchMode();
    }
    else if (Toybox::InputMap::Test("General.NextShip", qualifiers, key))
    {
        if (active_scene == Toybox::SCENE_SPACE)
        {
            if (++active_ship == num_ships)
                active_ship = 0;
            camera->SetTarget(ships[active_ship]->GetNode());
        }
    }
    else if (Toybox::InputMap::Test("General.PrevShip", qualifiers, key))
    {
        if (active_scene == Toybox::SCENE_SPACE)
        {
            if (active_ship-- == 0)
                active_ship = num_ships - 1;
            camera->SetTarget(ships[active_ship]->GetNode());
        }
    }
}


void Game01::HandleUpdate(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data)
{
    float time_step = event_data[Urho3D::Update::P_TIMESTEP].GetFloat();

    Urho3D::Input* input = GetSubsystem<Urho3D::Input>();
    if (active_scene == Toybox::SCENE_SPACE)
    {
        ships[active_ship]->UpdateControls(input, time_step);

		UpdateSpaceHUD();
    }
    else if (active_scene == Toybox::SCENE_GROUND)
    {
        if (CHAR_CONTROLLER == Toybox::CHAR_CONTROLLER_KINEMATIC)
        {
/*
            kJulia->UpdateControls(input);
            kJulia->SetControl(Toybox::KM_CHARACTER_FORWARD, Toybox::InputMap::Test("Character.Forward", input));
            kJulia->SetControl(Toybox::KM_CHARACTER_BACKWARD, Toybox::InputMap::Test("Character.Backward", input));
            kJulia->SetControl(Toybox::KM_CHARACTER_STRAFE_LEFT, Toybox::InputMap::Test("Character.StrafeLeft", input));
            kJulia->SetControl(Toybox::KM_CHARACTER_STRAFE_RIGHT, Toybox::InputMap::Test("Character.StrafeRight", input));
            kJulia->SetControl(Toybox::KM_CHARACTER_TURN_LEFT, Toybox::InputMap::Test("Character.TurnLeft", input));
            kJulia->SetControl(Toybox::KM_CHARACTER_TURN_RIGHT, Toybox::InputMap::Test("Character.TurnRight", input));
            kJulia->SetControl(Toybox::KM_CHARACTER_JUMP, Toybox::InputMap::Test("Character.Jump", input));
            kJulia->SetControl(Toybox::KM_CHARACTER_WALK, Toybox::InputMap::Test("Character.Walk", input));
*/
            apc->UpdateControls(input, time_step);
        }
        else
        {
            dJulia->UpdateControls(input, time_step);
            dJulia->SetControl(Toybox::KM_CHARACTER_FORWARD, Toybox::InputMap::Test("Character.Forward", input));
            dJulia->SetControl(Toybox::KM_CHARACTER_BACKWARD, Toybox::InputMap::Test("Character.Backward", input));
            dJulia->SetControl(Toybox::KM_CHARACTER_STRAFE_LEFT, Toybox::InputMap::Test("Character.StrafeLeft", input));
            dJulia->SetControl(Toybox::KM_CHARACTER_STRAFE_RIGHT, Toybox::InputMap::Test("Character.StrafeRight", input));
            dJulia->SetControl(Toybox::KM_CHARACTER_TURN_LEFT, Toybox::InputMap::Test("Character.TurnLeft", input));
            dJulia->SetControl(Toybox::KM_CHARACTER_TURN_RIGHT, Toybox::InputMap::Test("Character.TurnRight", input));
            dJulia->SetControl(Toybox::KM_CHARACTER_JUMP, Toybox::InputMap::Test("Character.Jump", input));
            dJulia->SetControl(Toybox::KM_CHARACTER_WALK, Toybox::InputMap::Test("Character.Walk", input));
        }

        UpdateGroundHUD();
    }
}


void Game01::HandlePostUpdate(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data)
{
    float time_step = event_data[Urho3D::Update::P_TIMESTEP].GetFloat();

    Urho3D::Input* input = GetSubsystem<Urho3D::Input>();
    camera->UpdateControls(input, time_step);
    camera->SetControl(Toybox::KM_CAMERA_MOVE, Toybox::InputMap::Test("Camera.Move", input));
    camera->SetControl(Toybox::KM_CAMERA_FORWARD, Toybox::InputMap::Test("Camera.Forward", input));
    camera->SetControl(Toybox::KM_CAMERA_BACKWARD, Toybox::InputMap::Test("Camera.Backward", input));
    camera->SetControl(Toybox::KM_CAMERA_PAN_LEFT, Toybox::InputMap::Test("Camera.PanLeft", input));
    camera->SetControl(Toybox::KM_CAMERA_PAN_RIGHT, Toybox::InputMap::Test("Camera.PanRight", input));
    camera->SetControl(Toybox::KM_CAMERA_ROLL_LEFT, Toybox::InputMap::Test("Camera.RollLeft", input));
    camera->SetControl(Toybox::KM_CAMERA_ROLL_RIGHT, Toybox::InputMap::Test("Camera.RollRight", input));

    camera->UpdateCamera(time_step);
}


void Game01::HandlePostRenderUpdate(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data)
{
    if (draw_debug)
    {
        GetSubsystem<Urho3D::Renderer>()->DrawDebugGeometry(false);

        if (active_scene == Toybox::SCENE_SPACE)
        {
            Urho3D::CollisionShape* coll_shape = ships[active_ship]->GetComponent<Urho3D::CollisionShape>();
            coll_shape->DrawDebugGeometry(scene_space->GetComponent<Urho3D::DebugRenderer>(), false);
        }
        else if (active_scene == Toybox::SCENE_GROUND)
        {
            if (CHAR_CONTROLLER == Toybox::CHAR_CONTROLLER_KINEMATIC)
            {
				Urho3D::CollisionShape* coll_shape = kJulia->GetComponent<Urho3D::CollisionShape>();
                coll_shape->DrawDebugGeometry(scene_ground->GetComponent<Urho3D::DebugRenderer>(), false);
//                Urho3D::PODVector<Urho3D::Component*> coll_shapes;
//                kJulia->GetComponents(coll_shapes, Urho3D::CollisionShape::GetTypeStatic());
//                for (unsigned i = 0; i < coll_shapes.Size(); i++)
//                    coll_shapes[i]->DrawDebugGeometry(scene_ground->GetComponent<Urho3D::DebugRenderer>(), false);
            }
            else
            {
                Urho3D::CollisionShape* coll_shape = dJulia->GetComponent<Urho3D::CollisionShape>();
                coll_shape->DrawDebugGeometry(scene_ground->GetComponent<Urho3D::DebugRenderer>(), false);
            }


            Urho3D::CollisionShape* coll_terrain = terrain_node->GetComponent<Urho3D::CollisionShape>();
            if (coll_terrain)
            {
                coll_terrain->DrawDebugGeometry(scene_ground->GetComponent<Urho3D::DebugRenderer>(), false);
            }

            cube_shape1->DrawDebugGeometry(scene_ground->GetComponent<Urho3D::DebugRenderer>(), true);
            cube_shape2->DrawDebugGeometry(scene_ground->GetComponent<Urho3D::DebugRenderer>(), true);
            cube_shape3->DrawDebugGeometry(scene_ground->GetComponent<Urho3D::DebugRenderer>(), true);
            cube_shape4->DrawDebugGeometry(scene_ground->GetComponent<Urho3D::DebugRenderer>(), true);
            apc->GetComponent<Urho3D::CollisionShape>()->DrawDebugGeometry(scene_ground->GetComponent<Urho3D::DebugRenderer>(), true);
/*
            Urho3D::Terrain* t = scene_ground->GetComponent<Urho3D::Terrain>(true);
            if (t)
            {
                Urho3D::CollisionShape* ts = t->GetComponent<Urho3D::CollisionShape>();
                if (ts)
                {
                    printf("1111111\n");
                    ts->DrawDebugGeometry(scene_ground->GetComponent<Urho3D::DebugRenderer>(), false);
                }
            }
*/
        }
    }
}


void Game01::ListAllResources()
{
    Urho3D::ResourceCache* cache = GetSubsystem<Urho3D::ResourceCache>();
    Urho3D::HashMap<Urho3D::StringHash, Urho3D::ResourceGroup> hm = cache->GetAllResources();
    //printf("%d\n", hm.Size());
    for (unsigned i=0; i<hm.Size(); ++i)
    {
        Urho3D::ResourceGroup g = hm.Values()[i];
        for (unsigned j=0; j<g.resources_.Size(); ++j)
        {
            Urho3D::SharedPtr<Urho3D::Resource> r = g.resources_.Values()[j];
            printf("%s\n", r->GetName().CString());
        }
    }
}

