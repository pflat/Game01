#include "../Core/Toybox.h"
#include "../Core/ToyboxDefs.h"
#include "../Core/IO.h"
#include "../Core/Debug.h"

THIRD_PARTY_GUARDS_BEGIN
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/AnimationController.h>
#include <Urho3D/Graphics/Skybox.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/ParticleEffect.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Physics/RaycastVehicle.h>
#include <Urho3D/Audio/SoundSource3D.h>
THIRD_PARTY_GUARDS_END


namespace Toybox
{

void Init(Urho3D::Context* context)
{
    CameraCtrl::RegisterObject(context);
    ShipCtrl::RegisterObject(context);
    CarCtrl::RegisterObject(context);
    VehicleWeaponCtrl::RegisterObject(context);
    KinematicCharacterCtrl::RegisterObject(context);
    DynamicCharacter::RegisterObject(context);
    Scene::RegisterObject(context);
}


//  ok
Scene* LoadScene(const Urho3D::String file, Urho3D::Scene* scene, Urho3D::ResourceCache* cache)
{
    Urho3D::XMLFile* xml_file = cache->GetResource<Urho3D::XMLFile>(file);
    const Urho3D::XMLElement& xml_elem = xml_file->GetRoot();

    //  Create component root node
    Urho3D::Node* node = scene->CreateChild();

	//  Load common node definitions
	IO::LoadNode(node, xml_elem, cache);

	//  Create Sector component and set sector specific properties
    Scene* sector = node->CreateComponent<Scene>();
	sector->Init();

    //  Set skybox
    if (xml_elem.HasChild("skybox"))
    {
        Urho3D::Skybox* skybox = node->CreateComponent<Urho3D::Skybox>();
        skybox->SetModel(cache->GetResource<Urho3D::Model>(xml_elem.GetChild("skybox").GetAttribute("model")));
        skybox->SetMaterial(cache->GetResource<Urho3D::Material>(xml_elem.GetChild("skybox").GetAttribute("material")));
    }

    //  Set ambiente music
    if (xml_elem.HasChild("music"))
    {
//        Urho3D::Sound* music = cache->GetResource<Urho3D::Sound>(xml_elem.GetChild("music").GetAttribute("name"));
//        sector->SetAmbientMusic(music);
    }

    //  Set lighting setings (zone ambient light and fog)
	Urho3D::Zone* zone = node->CreateComponent<Urho3D::Zone>();
    zone->SetBoundingBox(Urho3D::BoundingBox(-3000.0f, 3000.0f));
    if (xml_elem.HasChild("ambient"))
    {
        zone->SetAmbientColor(xml_elem.GetChild("ambient").GetColor("color"));
    }
    else
    {
        zone->SetAmbientColor(Urho3D::Color(0.9f, 0.9f, 0.9f));
    }
    if (xml_elem.HasChild("fog"))
    {
        zone->SetFogColor(xml_elem.GetChild("fog").GetColor("color"));
        zone->SetFogStart(xml_elem.GetChild("fog").GetFloat("start"));
        zone->SetFogEnd(xml_elem.GetChild("fog").GetFloat("end"));
    }

    //  Set one directional light (TO BE REMOVED)
    Urho3D::Node* light_node = scene->CreateChild();
    light_node->SetDirection(Urho3D::Vector3(0.6f, -1.0f, 0.8f));
    Urho3D::Light* light = light_node->CreateComponent<Urho3D::Light>();
    light->SetLightType(Urho3D::LIGHT_DIRECTIONAL);
    light->SetCastShadows(true);
    //light->SetShadowBias(Urho3D::BiasParameters(0.00025f, 0.5f));
    //light->SetShadowCascade(Urho3D::CascadeParameters(10.0f, 50.0f, 200.0f, 0.0f, 0.8f));
    light->SetSpecularIntensity(0.5f);

    return sector;
}


//  ok
ShipCtrl* LoadShip(const Urho3D::String file, Urho3D::ResourceCache* cache, Urho3D::Scene* scene, Urho3D::Node* node)
{
    if (scene != 0 && node == 0)
        //  Create component root node
        node = scene->CreateChild();
    else if (node == 0)
        return 0;

    Urho3D::XMLFile* xml_file = cache->GetResource<Urho3D::XMLFile>(file);
    const Urho3D::XMLElement& xml_elem = xml_file->GetRoot();

	//  Load node tree
    IO::LoadNode(node, xml_elem, cache);

    Urho3D::RigidBody* body = node->GetComponent<Urho3D::RigidBody>(true);
    if (body != 0)
    {
        body->SetCollisionLayer(Toybox::CL_TERRAIN | Toybox::CL_STATIC | Toybox::CL_VEHICLES | Toybox::CL_CHARACTERS);
        body->SetCollisionEventMode(Urho3D::COLLISION_ALWAYS);
    }

	//  Create ShipController component and set ship specific properties
    ShipCtrl* ship = node->CreateComponent<ShipCtrl>();
    if (xml_elem.HasChild("strafe"))
    {
        ship->mov_x_.Set(xml_elem.GetChild("strafe").GetAttribute("mode"), xml_elem.GetChild("strafe").GetAttribute("coord"));
        ship->mov_x_.Set(xml_elem.GetChild("strafe").GetFloat("min"),
                         xml_elem.GetChild("strafe").GetFloat("max"),
                         xml_elem.GetChild("strafe").GetFloat("step"),
                         xml_elem.GetChild("strafe").GetFloat("accel"),
                         xml_elem.GetChild("strafe").GetFloat("damp"));
        ship->mov_x_.SetEnable(true);
    }
    if (xml_elem.HasChild("lift"))
    {
        ship->mov_y_.Set(xml_elem.GetChild("lift").GetAttribute("mode"), xml_elem.GetChild("lift").GetAttribute("coord"));
        ship->mov_y_.Set(xml_elem.GetChild("lift").GetFloat("min"),
                         xml_elem.GetChild("lift").GetFloat("max"),
                         xml_elem.GetChild("lift").GetFloat("step"),
                         xml_elem.GetChild("lift").GetFloat("accel"),
                         xml_elem.GetChild("lift").GetFloat("damp"));
        ship->mov_y_.SetEnable(true);
    }
    if (xml_elem.HasChild("thrust"))
    {
        ship->mov_z_.Set(xml_elem.GetChild("thrust").GetAttribute("mode"), xml_elem.GetChild("thrust").GetAttribute("coord"));
        ship->mov_z_.Set(xml_elem.GetChild("thrust").GetFloat("min"),
                         xml_elem.GetChild("thrust").GetFloat("max"),
                         xml_elem.GetChild("thrust").GetFloat("step"),
                         xml_elem.GetChild("thrust").GetFloat("accel"),
                         xml_elem.GetChild("thrust").GetFloat("damp"));
        ship->mov_z_.SetEnable(true);
    }
    if (xml_elem.HasChild("pitch"))
    {
        ship->rot_x_.Set(xml_elem.GetChild("pitch").GetAttribute("mode"), xml_elem.GetChild("pitch").GetAttribute("coord"));
        ship->rot_x_.Set(xml_elem.GetChild("pitch").GetFloat("min"),
                         xml_elem.GetChild("pitch").GetFloat("max"),
                         xml_elem.GetChild("pitch").GetFloat("step"),
                         xml_elem.GetChild("pitch").GetFloat("accel"),
                         xml_elem.GetChild("pitch").GetFloat("damp"));
        ship->rot_x_.SetEnable(true);
    }
    if (xml_elem.HasChild("yaw"))
    {
        ship->rot_y_.Set(xml_elem.GetChild("yaw").GetAttribute("mode"), xml_elem.GetChild("yaw").GetAttribute("coord"));
        ship->rot_y_.Set(xml_elem.GetChild("yaw").GetFloat("min"),
                         xml_elem.GetChild("yaw").GetFloat("max"),
                         xml_elem.GetChild("yaw").GetFloat("step"),
                         xml_elem.GetChild("yaw").GetFloat("accel"),
                         xml_elem.GetChild("yaw").GetFloat("damp"));
        ship->rot_y_.SetEnable(true);
    }
    if (xml_elem.HasChild("roll"))
    {
        ship->rot_z_.Set(xml_elem.GetChild("roll").GetAttribute("mode"), xml_elem.GetChild("roll").GetAttribute("coord"));
        ship->rot_z_.Set(xml_elem.GetChild("roll").GetFloat("min"),
                         xml_elem.GetChild("roll").GetFloat("max"),
                         xml_elem.GetChild("roll").GetFloat("step"),
                         xml_elem.GetChild("roll").GetFloat("accel"),
                         xml_elem.GetChild("roll").GetFloat("damp"));
        ship->rot_z_.SetEnable(true);
    }

    for (Urho3D::XMLElement child_elem = xml_elem.GetChild("node"); child_elem; child_elem = child_elem.GetNext("node"))
    {
        if (child_elem.GetAttribute("name") == "exhausts_z")
        {
            ship->exhausts_z_ = node->GetChild("exhausts_z");

            if (child_elem.HasChild("sound"))
            {
                ship->sound_z_ = cache->GetResource<Urho3D::Sound>(child_elem.GetChild("sound").GetAttribute("value"));
                ship->sound_z_->SetLooped(true);

                Urho3D::SoundSource3D* engine_sound_source = node->GetChild("exhausts_z")->CreateComponent<Urho3D::SoundSource3D>();
                engine_sound_source->SetSoundType(Urho3D::SOUND_EFFECT);
                engine_sound_source->SetNearDistance(20);
                engine_sound_source->SetFarDistance(80);
                engine_sound_source->SetGain(0.15f);
            }
        }
    }

    return ship;
}


CarCtrl* LoadCar(const Urho3D::String file, Urho3D::ResourceCache* cache, Urho3D::Scene* scene, Urho3D::Node* node)
{
    if (scene != 0 && node == 0)
        //  Create entity tree root node
        node = scene->CreateChild();
    else if (node == 0)
        return 0;

    Urho3D::XMLFile* xml_file = cache->GetResource<Urho3D::XMLFile>(file);
    const Urho3D::XMLElement& xml_elem = xml_file->GetRoot();

    //  Load node tree
    IO::LoadNode(node, xml_elem, cache);

    Urho3D::RigidBody* body = node->GetComponent<Urho3D::RigidBody>(true);
    if (body != 0)
    {
        body->SetCollisionLayer(Toybox::CL_TERRAIN | Toybox::CL_STATIC | Toybox::CL_VEHICLES | Toybox::CL_CHARACTERS);
        //body->SetCollisionEventMode(Urho3D::COLLISION_ALWAYS);
    }

    //  Create raycast vehicle component
    Urho3D::RaycastVehicle* vehicle = node->CreateComponent<Urho3D::RaycastVehicle>();
    vehicle->Init();

    //  Create CarController component and set car specific properties
    CarCtrl* car = node->CreateComponent<CarCtrl>();
    car->vehicle_ = vehicle;

    car->Load(xml_elem);


    if (xml_elem.HasChild("engine"))
    {
        car->mov_z_.Set(xml_elem.GetChild("engine").GetAttribute("mode"), xml_elem.GetChild("engine").GetAttribute("coord"));
        car->mov_z_.Set(xml_elem.GetChild("engine").GetFloat("min"),
                        xml_elem.GetChild("engine").GetFloat("max"),
                        xml_elem.GetChild("engine").GetFloat("step"),
                        xml_elem.GetChild("engine").GetFloat("accel"),
                        xml_elem.GetChild("engine").GetFloat("damp"));
        car->mov_z_.SetEnable(true);
    }

    if (xml_elem.HasChild("steer"))
    {
        car->rot_y_.Set(xml_elem.GetChild("steer").GetAttribute("mode"), xml_elem.GetChild("steer").GetAttribute("coord"));
        car->rot_y_.Set(xml_elem.GetChild("steer").GetFloat("min"),
                        xml_elem.GetChild("steer").GetFloat("max"),
                        xml_elem.GetChild("steer").GetFloat("step"),
                        xml_elem.GetChild("steer").GetFloat("accel"),
                        xml_elem.GetChild("steer").GetFloat("damp"));
        car->rot_y_.SetEnable(true);
    }


    car->handbrake_ = xml_elem.GetChild("handbrake").GetFloat("value");
    car->in_air_rpm_ = xml_elem.GetChild("in_air_rpm").GetFloat("value");
    //vehicle->SetInAirRPM(200.0f);

    vehicle->ResetWheels();
    return car;
}


VehicleWeaponCtrl* LoadWeapon(const Urho3D::String file, Urho3D::Scene* scene, Urho3D::ResourceCache* cache)
{
    Urho3D::XMLFile* xml_file = cache->GetResource<Urho3D::XMLFile>(file);
    const Urho3D::XMLElement& xml_elem = xml_file->GetRoot();

    //  Create weapon node tree
    Urho3D::Node* node = scene->CreateChild();
    IO::LoadNode(node, xml_elem, cache);

    //  read weapon specific properties

    VehicleWeaponCtrl* weapon = node->CreateComponent<VehicleWeaponCtrl>();
    return weapon;
}


Urho3D::RibbonTrail* LoadRibbonTrail(const Urho3D::String file, Urho3D::Node* node, Urho3D::ResourceCache* cache)
{
    Urho3D::XMLFile* xml_file = cache->GetResource<Urho3D::XMLFile>(file);
    const Urho3D::XMLElement& xml_elem = xml_file->GetRoot();

    node->SetVar("effect", "ribbon");
    Urho3D::RibbonTrail* trail = node->CreateComponent<Urho3D::RibbonTrail>();
    trail->SetMaterial(cache->GetResource<Urho3D::Material>(xml_elem.GetChild("material").GetAttribute("name")));
    trail->SetStartColor(xml_elem.GetChild("color").GetColor("start"));
    trail->SetStartScale(xml_elem.GetChild("scale").GetFloat("start"));
    trail->SetEndColor(xml_elem.GetChild("color").GetColor("end"));
    trail->SetEndScale(xml_elem.GetChild("scale").GetFloat("end"));
    trail->SetWidth(xml_elem.GetChild("width").GetFloat("value"));
    trail->SetLifetime(xml_elem.GetChild("lifetime").GetFloat("value"));
    trail->SetTailColumn(4);
    trail->SetVertexDistance(xml_elem.GetChild("detail").GetFloat("value"));
    trail->SetCastShadows(false);
    trail->SetUpdateInvisible(true);
    trail->SetEmitting(false);

    return trail;
}


Urho3D::ParticleEmitter* LoadParticleEmitter(const Urho3D::String file, Urho3D::Node* node, Urho3D::ResourceCache* cache)
{
    node->SetVar("effect", "particle");
    Urho3D::ParticleEmitter* emitter = node->CreateComponent<Urho3D::ParticleEmitter>();
    emitter->SetEffect(cache->GetResource<Urho3D::ParticleEffect>(file));

    //Urho3D::ParticleEffect* effect = emitter->GetEffect();
    emitter->SetCastShadows(false);
    emitter->SetEmitting(false);

    return emitter;
}


KinematicCharacterCtrl* LoadKinematicCharacter(const Urho3D::String file, Urho3D::ResourceCache* cache, Urho3D::Scene* scene, Urho3D::Node* node)
{
    if (scene != 0 && node == 0)
        //  Create component root node
        node = scene->CreateChild();
    else if (node == 0)
        return 0;

    Urho3D::XMLFile* xml_file = cache->GetResource<Urho3D::XMLFile>(file);
    const Urho3D::XMLElement& xml_elem = xml_file->GetRoot();

    Urho3D::Node* spin = node->CreateChild();
    IO::LoadNode(spin, xml_elem, cache);
/*
    // Create rigidbody, and set zero mass so that the body does NOT becomes dynamic.
    Urho3D::RigidBody* body = node->CreateComponent<Urho3D::RigidBody>();
    body->SetMass(60.0f);
    body->SetKinematic(true);

    body->SetLinearDamping(0.0f);
    body->SetAngularDamping(0.0f);
    body->SetFriction(0.0f);

    // Set zero angular factor so that physics doesn't turn the character on its own.
    // Instead we will control the character yaw manually.
    // Should not be necessary, since the body isn't dynamic.
    body->SetAngularFactor(Urho3D::Vector3::ZERO);

    body->SetCollisionLayer(CL_CHARACTERS);
    body->SetCollisionMask(CL_TERRAIN | CL_STATIC);
    //body->SetCollisionEventMode(Urho3D::COLLISION_ALWAYS);
*/
    // Set a capsule shape for collision
    float capsule_height = 1.63f;
    float capsule_diameter = 0.4f;
    float spring_height = 0.2f;
    float gravity = -9.81f * 0.1f;
    float jump_strength = -gravity * 30.0f;

    Urho3D::CollisionShape* shape1 = node->CreateComponent<Urho3D::CollisionShape>();
    shape1->SetCapsule(capsule_diameter, capsule_height - spring_height, Urho3D::Vector3(0.0f, (capsule_height + spring_height) / 2.0f, 0.0f));

    // Set the head bone for manual control
    //object->GetSkeleton().GetBone("Mutant:Head")->animated_ = false;

	// Set the rigidbody to signal collision also when in rest, so that we get ground collisions properly
    //body->SetCollisionEventMode(Urho3D::COLLISION_ALWAYS);

	KinematicCharacterCtrl* kc = node->CreateComponent<KinematicCharacterCtrl>();
    kc->Init(10.0f, 5.0f, 7.0f, jump_strength, 5.0f, 0.25f, 1.0f, gravity, Urho3D::Cos(45.0f), spring_height);
    return kc;
}


DynamicCharacter* LoadDynamicCharacter(const Urho3D::String file, Urho3D::Scene* scene, Urho3D::ResourceCache* cache)
{
    Urho3D::XMLFile* xml_file = cache->GetResource<Urho3D::XMLFile>(file);
    const Urho3D::XMLElement& xml_elem = xml_file->GetRoot();

    Urho3D::Node* node = scene->CreateChild();
    IO::LoadNode(node, xml_elem, cache);


    Urho3D::RigidBody* body = node->CreateComponent<Urho3D::RigidBody>();
    body->SetMass(60.0f);

    //body->SetLinearDamping(0.0f);
    //body->SetAngularDamping(0.0f);
    //body->SetFriction(0.0f);

    body->SetAngularFactor(Urho3D::Vector3::ZERO);
    body->SetCollisionLayer(CL_CHARACTERS);
    body->SetCollisionMask(CL_TERRAIN | CL_STATIC);
    //body->SetCollisionEventMode(Urho3D::COLLISION_ALWAYS);

    Urho3D::CollisionShape* shape = node->CreateComponent<Urho3D::CollisionShape>();
    shape->SetCapsule(0.35f, 1.63f, Urho3D::Vector3(0.0f, 1.63f * 0.5f, 0.0f));  //  FULL CAPSULE
    //shape->SetCapsule(0.35f, 0.815f, Urho3D::Vector3(0.0f, 1.2225f, 0.0f));  //  TOP HALF
    //shape->SetCapsule(0.35f, 1.63f / 2.0f, Urho3D::Vector3(0.0f, 3.0f * 1.63f / 4.0f, 0.0f));  //  TOP HALF
    //shape->SetCapsule(0.1, 0.1, Urho3D::Vector3(0.0f, 1.5f, 0.0f));  //  HEAD ONLY
    //shape->SetBox(Urho3D::Vector3(0.1f, 0.1f, 0.1f), Urho3D::Vector3(0.0f, 1.5f, 0.0f));

    // Set the head bone for manual control
    //object->GetSkeleton().GetBone("Mutant:Head")->animated_ = false;


/*
    // Create rigidbody, and set non-zero mass so that the body becomes dynamic
    Urho3D::RigidBody* body = node->CreateComponent<Urho3D::RigidBody>();
    //body->SetCollisionLayer(1);
    body->SetMass(1.0f);

    //  Set zero angular factor so that physics doesn't turn the character on its own.
    //  Instead we will control the character yaw manually
    body->SetAngularFactor(Urho3D::Vector3::ZERO);

    // Set the rigidbody to signal collision also when in rest, so that we get ground collisions properly
    //body->SetCollisionEventMode(Urho3D::COLLISION_ALWAYS);

    // Set a capsule shape for collision
    //Urho3D::CollisionShape* shape = objectNode->CreateComponent<Urho3D::CollisionShape>();
    //shape->SetCapsule(0.7f, 1.8f, Urho3D::Vector3(0.0f, 0.9f, 0.0f));
*/
    DynamicCharacter* ch = node->CreateComponent<DynamicCharacter>();

    return ch;
}


//  ok
CameraCtrl* CreateCamera(Urho3D::Context* context)
{
	//  Create component root node
    Urho3D::Node* node = new Urho3D::Node(context);
    node->SetPosition(Urho3D::Vector3(0.0f, 2.0f, -20.0f));

	//  Create Camera component and set camera specific properties
    Urho3D::Camera* camera = node->CreateComponent<Urho3D::Camera>();
    camera->SetFarClip(300.0f);

	//  Create controller component
    return node->CreateComponent<CameraCtrl>();
}


//  debug
void SaveJSON(Urho3D::Context* context, Urho3D::ResourceCache* cache)
{
    Urho3D::Material* mat_test = cache->GetResource<Urho3D::Material>("materials/vehicles/space/fighter_a01.xml");

    Urho3D::JSONValue json_root;
    mat_test->Save(json_root);

    Urho3D::JSONFile json_file(context);
    json_file.GetRoot() = json_root;

    Urho3D::File file_out(context);
    file_out.Open("C:/plisa/Projects/Pessoal/Cpp/Urho3D/Game01/data/materials/vehicles/space/test.json", Urho3D::FILE_WRITE);
    json_file.Save(file_out);
    file_out.Close();

    // Save as xml
    //mat_test->SaveFile("C:/plisa/Projects/Pessoal/Cpp/Urho3D/Game01/data/materials/vehicles/space/test.xml");
}


void SaveTree(Urho3D::Context* context, Urho3D::Node* node)
{
    Urho3D::File file_out(context, "C:/plisa/Projects/Pessoal/Cpp/Urho3D/Export/test.xml", Urho3D::FILE_WRITE);
    node->SaveXML(file_out);
    //Urho3D::File file_out(context, "C:/plisa/Projects/Pessoal/Cpp/Urho3D/Export/test.json", Urho3D::FILE_WRITE);
    //node->SaveJSON(file_out);
}


}
