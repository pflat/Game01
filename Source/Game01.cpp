#include "Game01.h"
#include "Settings.h"
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
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
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
    scene_space(0),
    scene_ground(0),
    camera(0),
    num_ships(0),
    active_ship(0),
    kJulia(0),
    dJulia(0)
{
    Toybox::CameraCtrl::RegisterObject(context);
    Toybox::ShipCtrl::RegisterObject(context);
    Toybox::VehicleWeaponCtrl::RegisterObject(context);
    Toybox::KinematicCharacterCtrl::RegisterObject(context);
    Toybox::DynamicCharacter::RegisterObject(context);
    Toybox::Scene::RegisterObject(context);
}


void Game01::Setup()
{
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

    CreateSpaceScene();
	CreateGroundScene();

	//  Set camera
    camera = Toybox::CreateCamera(context_);
    GetSubsystem<Urho3D::Audio>()->SetListener(camera->GetLookAtNode()->GetComponent<Urho3D::SoundListener>());

	SwitchScene(Toybox::SCENE_SPACE);

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


void Game01::CreateSpaceScene()
{
    Urho3D::ResourceCache* cache = GetSubsystem<Urho3D::ResourceCache>();

    scene_space = new Urho3D::Scene(context_);
    scene_space->CreateComponent<Urho3D::DebugRenderer>();
    scene_space->CreateComponent<Urho3D::Octree>();
    Urho3D::PhysicsWorld* world = scene_space->CreateComponent<Urho3D::PhysicsWorld>();
    world->SetGravity(Urho3D::Vector3(0.0f, 0.0f, 0.0f));

    sector = Toybox::LoadScene("sectors/space/corona.xml", scene_space, cache);

    //Toybox::ShipCtrl* ship1 = Toybox::LoadShip("vehicles/space/fighter_a01.xml", scene_space, cache);
    //ship1->GetNode()->SetPosition(Urho3D::Vector3(-20.0f, 0.0f, 10.0f));

    Toybox::ShipCtrl* ship2 = Toybox::LoadShip("vehicles/space/fighter_a02.xml", scene_space, cache);
    ship2->GetNode()->SetPosition(Urho3D::Vector3(-10.0f, 0.0f, 10.0f));

    //Toybox::ShipCtrl* ship3(Toybox::LoadShip("vehicles/space/fighter_a03.xml", scene_space, cache));
    //ship3->GetNode()->SetPosition(Urho3D::Vector3(0.0f, 0.0f, 10.0f));

    Toybox::ShipCtrl* ship4(Toybox::LoadShip("vehicles/space/fighter_a04.xml", scene_space, cache));
    ship4->GetNode()->SetPosition(Urho3D::Vector3(10.0f, 0.0f, 10.0f));

    //Toybox::ShipCtrl* ship5(Toybox::LoadShip("vehicles/space/fighter_a05.xml", scene_space, cache));
    //ship5->GetNode()->SetPosition(Urho3D::Vector3(20.0f, 0.0f, 10.0f));

	//ships.Push(Urho3D::WeakPtr<Toybox::ShipCtrl>(ship1));
	//ships.Push(Urho3D::WeakPtr<Toybox::ShipCtrl>(ship2));
    //ships.Push(Urho3D::WeakPtr<Toybox::ShipCtrl>(ship3));
	//ships.Push(Urho3D::WeakPtr<Toybox::ShipCtrl>(ship4));
	//ships.Push(Urho3D::WeakPtr<Toybox::ShipCtrl>(ship5));

	//ships.push_back(Urho3D::WeakPtr<Toybox::ShipCtrl>(ship1));
	ships.push_back(Urho3D::WeakPtr<Toybox::ShipCtrl>(ship2));
    //ships.push_back(Urho3D::WeakPtr<Toybox::ShipCtrl>(ship3));
	ships.push_back(Urho3D::WeakPtr<Toybox::ShipCtrl>(ship4));
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

    if (CHAR_CONTROLLER == Toybox::CHAR_CONTROLLER_KINEMATIC)
    {
        kJulia = Toybox::LoadKinematicCharacter("characters/human/julia.xml", scene_ground, cache);
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

}


void Game01::CreateGroundHUDLayout(const Urho3D::String& out_file)
{
}


void Game01::LoadGroundHUDLayout()
{
    Urho3D::ResourceCache* cache = GetSubsystem<Urho3D::ResourceCache>();
    Urho3D::UI* ui = GetSubsystem<Urho3D::UI>();

    ui->Clear();
    //Urho3D::SharedPtr<Urho3D::UIElement> loaded_root = ui->LoadLayout(cache->GetResource<Urho3D::XMLFile>("ui/hud_ground.xml"));
    //ui->GetRoot()->AddChild(loaded_root);
}


void Game01::UpdateGroundHUD()
{

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
            camera->SetTarget(kJulia->GetCameraNode(), true);
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

    int key = event_data[Urho3D::KeyUp::P_KEY].GetInt();
    bool key_ctrl = input->GetQualifierDown(Urho3D::QUAL_CTRL);
    //bool key_alt = input->GetQualifierDown(Urho3D::QUAL_ALT);
    //bool key_shift = input->GetQualifierDown(Urho3D::QUAL_SHIFT);

    //  Texture quality
    if (key == Urho3D::KEY_F1 && key_ctrl)
    {
        int quality = renderer->GetTextureQuality();
        ++quality;
        if (quality > Urho3D::QUALITY_HIGH)
            quality = Urho3D::QUALITY_LOW;
        renderer->SetTextureQuality(quality);
    }

    //  Material quality
    else if (key == Urho3D::KEY_F2 && key_ctrl)
    {
        int quality = renderer->GetMaterialQuality();
        ++quality;
        if (quality > Urho3D::QUALITY_HIGH)
            quality = Urho3D::QUALITY_LOW;
        renderer->SetMaterialQuality(quality);
    }

    //  Specular lighting
    else if (key == Urho3D::KEY_F3 && key_ctrl)
    {
        renderer->SetSpecularLighting(!renderer->GetSpecularLighting());
    }

    //  Shadow rendering
    else if (key == Urho3D::KEY_F4 && key_ctrl)
    {
        renderer->SetDrawShadows(!renderer->GetDrawShadows());
    }

    //  Shadow map resolution
    else if (key == Urho3D::KEY_F5 && key_ctrl)
    {
        int shadowMapSize = renderer->GetShadowMapSize();
        shadowMapSize *= 2;
        if (shadowMapSize > 2048)
            shadowMapSize = 512;
        renderer->SetShadowMapSize(shadowMapSize);
    }

    //  Shadow depth and filtering quality
    else if (key == Urho3D::KEY_F6 && key_ctrl)
    {
        Urho3D::ShadowQuality quality = renderer->GetShadowQuality();
        quality = (Urho3D::ShadowQuality)(quality + 1);
        if (quality > Urho3D::SHADOWQUALITY_BLUR_VSM)
            quality = Urho3D::SHADOWQUALITY_SIMPLE_16BIT;
        renderer->SetShadowQuality(quality);
    }

    //  Occlusion culling
    else if (key == Urho3D::KEY_F7 && key_ctrl)
    {
        bool occlusion = renderer->GetMaxOccluderTriangles() > 0;
        occlusion = !occlusion;
        renderer->SetMaxOccluderTriangles(occlusion ? 5000 : 0);
    }

    //  Instancing
    else if (key == Urho3D::KEY_F8 && key_ctrl)
    {
        renderer->SetDynamicInstancing(!renderer->GetDynamicInstancing());
    }

    //  Draw debug geometry
    else if (key == Urho3D::KEY_F9 && key_ctrl)
    {
        draw_debug = !draw_debug;
    }

    //  Print debug information
    else if (key == Urho3D::KEY_F10 && key_ctrl)
    {
        GetSubsystem<Urho3D::DebugHud>()->ToggleAll();
    }

    //  Switch between scenes (SPACE / GROUND)
    else if (key == Urho3D::KEY_TAB && key_ctrl)
    {
        change_scene = 1;
    }


    // Close console (if open) or exit when ESC is pressed
    if (key == Urho3D::KEY_ESCAPE)
    {
        //Urho3D::Console* console = GetSubsystem<Urho3D::Console>();
        //if (console->IsVisible())
        //    console->SetVisible(false);
        //else
            engine_->Exit();
    }
    else if (key == Urho3D::KEY_PRINTSCREEN)
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
    else if (key == Urho3D::KEY_C)
    {
        camera->Reset();
    }
    else if (key == Urho3D::KEY_F)
    {
        camera->SwitchMode();
    }
    else if (key == Urho3D::KEY_PAGEUP)
    {
        if (active_scene == Toybox::SCENE_SPACE)
        {
            if (++active_ship == num_ships)
                active_ship = 0;
            camera->SetTarget(ships[active_ship]->GetNode());
        }
    }
    else if (key == Urho3D::KEY_PAGEDOWN)
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
    Urho3D::Input* input = GetSubsystem<Urho3D::Input>();

    if (active_scene == Toybox::SCENE_SPACE)
    {
        ships[active_ship]->input.Update(input);
        ships[active_ship]->input.SetKey(Toybox::KM_VEHICLE_THRUST_X_INC, input->GetKeyDown(KEY_VEHICLE_STRAFE_INC));
        ships[active_ship]->input.SetKey(Toybox::KM_VEHICLE_THRUST_X_DEC, input->GetKeyDown(KEY_VEHICLE_STRAFE_DEC));
        ships[active_ship]->input.SetKey(Toybox::KM_VEHICLE_THRUST_X_MAX, input->GetKeyDown(KEY_VEHICLE_STRAFE_MAX));
        ships[active_ship]->input.SetKey(Toybox::KM_VEHICLE_THRUST_X_STOP, input->GetKeyDown(KEY_VEHICLE_STRAFE_STOP));
        ships[active_ship]->input.SetKey(Toybox::KM_VEHICLE_ROT_X_DEC, input->GetKeyDown(KEY_VEHICLE_PITCH_UP));
        ships[active_ship]->input.SetKey(Toybox::KM_VEHICLE_ROT_X_INC, input->GetKeyDown(KEY_VEHICLE_PITCH_DOWN));
        ships[active_ship]->input.SetKey(Toybox::KM_VEHICLE_THRUST_Y_INC, input->GetKeyDown(KEY_VEHICLE_UP_INC));
        ships[active_ship]->input.SetKey(Toybox::KM_VEHICLE_THRUST_Y_DEC, input->GetKeyDown(KEY_VEHICLE_UP_DEC));
        ships[active_ship]->input.SetKey(Toybox::KM_VEHICLE_THRUST_Y_MAX, input->GetKeyDown(KEY_VEHICLE_UP_MAX));
        ships[active_ship]->input.SetKey(Toybox::KM_VEHICLE_THRUST_Y_STOP, input->GetKeyDown(KEY_VEHICLE_UP_STOP));
        ships[active_ship]->input.SetKey(Toybox::KM_VEHICLE_ROT_Y_DEC, input->GetKeyDown(KEY_VEHICLE_YAW_LEFT));
        ships[active_ship]->input.SetKey(Toybox::KM_VEHICLE_ROT_Y_INC, input->GetKeyDown(KEY_VEHICLE_YAW_RIGHT));
        ships[active_ship]->input.SetKey(Toybox::KM_VEHICLE_THRUST_Z_INC, input->GetKeyDown(KEY_VEHICLE_FORWARD_INC));
        ships[active_ship]->input.SetKey(Toybox::KM_VEHICLE_THRUST_Z_DEC, input->GetKeyDown(KEY_VEHICLE_FORWARD_DEC));
        ships[active_ship]->input.SetKey(Toybox::KM_VEHICLE_THRUST_Z_MAX, input->GetKeyDown(KEY_VEHICLE_FORWARD_MAX));
        ships[active_ship]->input.SetKey(Toybox::KM_VEHICLE_THRUST_Z_STOP, input->GetKeyDown(KEY_VEHICLE_FORWARD_STOP));
        ships[active_ship]->input.SetKey(Toybox::KM_VEHICLE_ROT_Z_DEC, input->GetKeyDown(KEY_VEHICLE_ROLL_LEFT));
        ships[active_ship]->input.SetKey(Toybox::KM_VEHICLE_ROT_Z_INC, input->GetKeyDown(KEY_VEHICLE_ROLL_RIGHT));

		UpdateSpaceHUD();
    }
    else if (active_scene == Toybox::SCENE_GROUND)
    {
        if (CHAR_CONTROLLER == Toybox::CHAR_CONTROLLER_KINEMATIC)
        {
            kJulia->input_.Update(input);
            kJulia->input_.SetKey(Toybox::KM_CHARACTER_FORWARD, input->GetKeyDown(KEY_CHARACTER_FORWARD));
            kJulia->input_.SetKey(Toybox::KM_CHARACTER_BACKWARD, input->GetKeyDown(KEY_CHARACTER_BACKWARD));
            kJulia->input_.SetKey(Toybox::KM_CHARACTER_STRAFE_LEFT, input->GetKeyDown(KEY_CHARACTER_STRAFE_LEFT));
            kJulia->input_.SetKey(Toybox::KM_CHARACTER_STRAFE_RIGHT, input->GetKeyDown(KEY_CHARACTER_STRAFE_RIGHT));
            kJulia->input_.SetKey(Toybox::KM_CHARACTER_TURN_LEFT, input->GetKeyDown(KEY_CHARACTER_TURN_LEFT));
            kJulia->input_.SetKey(Toybox::KM_CHARACTER_TURN_RIGHT, input->GetKeyDown(KEY_CHARACTER_TURN_RIGHT));
            kJulia->input_.SetKey(Toybox::KM_CHARACTER_JUMP, input->GetKeyDown(KEY_CHARACTER_JUMP));
            kJulia->input_.SetKey(Toybox::KM_CHARACTER_WALK, input->GetKeyDown(KEY_CHARACTER_WALK));
        }
        else
        {
            dJulia->input.Update(input);
            dJulia->input.SetKey(Toybox::KM_CHARACTER_FORWARD, input->GetKeyDown(KEY_CHARACTER_FORWARD));
            dJulia->input.SetKey(Toybox::KM_CHARACTER_BACKWARD, input->GetKeyDown(KEY_CHARACTER_BACKWARD));
            dJulia->input.SetKey(Toybox::KM_CHARACTER_STRAFE_LEFT, input->GetKeyDown(KEY_CHARACTER_STRAFE_LEFT));
            dJulia->input.SetKey(Toybox::KM_CHARACTER_STRAFE_RIGHT, input->GetKeyDown(KEY_CHARACTER_STRAFE_RIGHT));
            dJulia->input.SetKey(Toybox::KM_CHARACTER_TURN_LEFT, input->GetKeyDown(KEY_CHARACTER_TURN_LEFT));
            dJulia->input.SetKey(Toybox::KM_CHARACTER_TURN_RIGHT, input->GetKeyDown(KEY_CHARACTER_TURN_RIGHT));
            dJulia->input.SetKey(Toybox::KM_CHARACTER_JUMP, input->GetKeyDown(KEY_CHARACTER_JUMP));
            dJulia->input.SetKey(Toybox::KM_CHARACTER_WALK, input->GetKeyDown(KEY_CHARACTER_WALK));
        }

        UpdateGroundHUD();
    }
}


void Game01::HandlePostUpdate(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data)
{
    float time_step = event_data[Urho3D::Update::P_TIMESTEP].GetFloat();

    Urho3D::Input* input = GetSubsystem<Urho3D::Input>();
    camera->input_.Update(input);
    camera->input_.SetKey(Toybox::KM_CAMERA_FORWARD, input->GetKeyDown(KEY_CAMERA_FORWARD));
    camera->input_.SetKey(Toybox::KM_CAMERA_BACKWARD, input->GetKeyDown(KEY_CAMERA_BACKWARD));
    camera->input_.SetKey(Toybox::KM_CAMERA_PAN_LEFT, input->GetKeyDown(KEY_CAMERA_PAN_LEFT));
    camera->input_.SetKey(Toybox::KM_CAMERA_PAN_RIGHT, input->GetKeyDown(KEY_CAMERA_PAN_RIGHT));
    camera->input_.SetKey(Toybox::KM_CAMERA_ROLL_LEFT, input->GetKeyDown(KEY_CAMERA_ROLL_LEFT));
    camera->input_.SetKey(Toybox::KM_CAMERA_ROLL_RIGHT, input->GetKeyDown(KEY_CAMERA_ROLL_RIGHT));

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

