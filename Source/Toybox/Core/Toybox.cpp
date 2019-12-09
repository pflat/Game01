#include "../Core/Toybox.h"
#include "../Core/ToyboxDefs.h"

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
#include <Urho3D/Audio/SoundSource3D.h>
THIRD_PARTY_GUARDS_END


namespace Toybox
{

void Init()
{

}


//  ok
Scene* LoadScene(const Urho3D::String file, Urho3D::Scene* scene, Urho3D::ResourceCache* cache)
{
    Urho3D::XMLFile* xml_file = cache->GetResource<Urho3D::XMLFile>(file);
    const Urho3D::XMLElement& xml_elem = xml_file->GetRoot();

    //  Create component root node
    Urho3D::Node* node = scene->CreateChild();

	//  Load common node definitions
	LoadNode(node, xml_elem, cache);

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
ShipCtrl* LoadShip(const Urho3D::String file, Urho3D::Scene* scene, Urho3D::ResourceCache* cache)
{
    Urho3D::XMLFile* xml_file = cache->GetResource<Urho3D::XMLFile>(file);
    const Urho3D::XMLElement& xml_elem = xml_file->GetRoot();

    //  Create component root node
    Urho3D::Node* node = scene->CreateChild();

	//  Load common node definitions
    LoadNode(node, xml_elem, cache);

	//  Define RigidBody component
    Urho3D::RigidBody* body = node->CreateComponent<Urho3D::RigidBody>();
    if (xml_elem.HasChild("mass"))
        body->SetMass(xml_elem.GetChild("mass").GetFloat("value"));
    else
        body->SetMass(1.0f);

    if (xml_elem.HasChild("damp"))
    {
        body->SetLinearDamping(xml_elem.GetChild("damp").GetFloat("linear"));
        body->SetAngularDamping(xml_elem.GetChild("damp").GetFloat("angular"));
        body->SetFriction(xml_elem.GetChild("damp").GetFloat("friction"));
    }
    else
    {
        body->SetLinearDamping(0.5f);
        body->SetAngularDamping(0.5f);
        body->SetFriction(0.5f);
    }

    //body->SetAngularFactor(Urho3D::Vector3::ZERO);
    body->SetCollisionLayer(1);
    body->SetCollisionEventMode(Urho3D::COLLISION_ALWAYS);

	//  Create Spaceship component and set ship specific properties
    ShipCtrl* ship = node->CreateComponent<ShipCtrl>();
    if (xml_elem.HasChild("strafe"))
    {
        ship->thrust_x_.Set(xml_elem.GetChild("strafe").GetAttribute("mode"), xml_elem.GetChild("strafe").GetAttribute("coord"));
        ship->thrust_x_.Set(xml_elem.GetChild("strafe").GetFloat("step"),
                            xml_elem.GetChild("strafe").GetFloat("min"),
                            xml_elem.GetChild("strafe").GetFloat("max"),
                            xml_elem.GetChild("strafe").GetFloat("damp"));
        ship->thrust_x_.SetEnable(true);
    }
    if (xml_elem.HasChild("lift"))
    {
        ship->thrust_y_.Set(xml_elem.GetChild("lift").GetAttribute("mode"), xml_elem.GetChild("lift").GetAttribute("coord"));
        ship->thrust_y_.Set(xml_elem.GetChild("lift").GetFloat("step"),
                            xml_elem.GetChild("lift").GetFloat("min"),
                            xml_elem.GetChild("lift").GetFloat("max"),
                            xml_elem.GetChild("lift").GetFloat("damp"));
        ship->thrust_y_.SetEnable(true);
    }
    if (xml_elem.HasChild("thrust"))
    {
        ship->thrust_z_.Set(xml_elem.GetChild("thrust").GetAttribute("mode"), xml_elem.GetChild("thrust").GetAttribute("coord"));
        ship->thrust_z_.Set(xml_elem.GetChild("thrust").GetFloat("step"),
                            xml_elem.GetChild("thrust").GetFloat("min"),
                            xml_elem.GetChild("thrust").GetFloat("max"),
                            xml_elem.GetChild("thrust").GetFloat("damp"));
        ship->thrust_z_.SetEnable(true);
    }
    if (xml_elem.HasChild("pitch"))
    {
        ship->rotate_x_.Set(xml_elem.GetChild("pitch").GetAttribute("mode"), xml_elem.GetChild("pitch").GetAttribute("coord"));
        ship->rotate_x_.Set(xml_elem.GetChild("pitch").GetFloat("step"),
                            xml_elem.GetChild("pitch").GetFloat("min"),
                            xml_elem.GetChild("pitch").GetFloat("max"),
                            xml_elem.GetChild("pitch").GetFloat("damp"));
        ship->rotate_x_.SetEnable(true);
    }
    if (xml_elem.HasChild("yaw"))
    {
        ship->rotate_y_.Set(xml_elem.GetChild("yaw").GetAttribute("mode"), xml_elem.GetChild("yaw").GetAttribute("coord"));
        ship->rotate_y_.Set(xml_elem.GetChild("yaw").GetFloat("step"),
                            xml_elem.GetChild("yaw").GetFloat("min"),
                            xml_elem.GetChild("yaw").GetFloat("max"),
                            xml_elem.GetChild("yaw").GetFloat("damp"));
        ship->rotate_y_.SetEnable(true);
    }
    if (xml_elem.HasChild("roll"))
    {
        ship->rotate_z_.Set(xml_elem.GetChild("roll").GetAttribute("mode"), xml_elem.GetChild("roll").GetAttribute("coord"));
        ship->rotate_z_.Set(xml_elem.GetChild("roll").GetFloat("step"),
                            xml_elem.GetChild("roll").GetFloat("min"),
                            xml_elem.GetChild("roll").GetFloat("max"),
                            xml_elem.GetChild("roll").GetFloat("damp"));
        ship->rotate_z_.SetEnable(true);
    }

    for (Urho3D::XMLElement child_elem = xml_elem.GetChild("node"); child_elem; child_elem = child_elem.GetNext("node"))
    {
        if (child_elem.GetAttribute("name") == "nExhausts")
        {
            ship->exhausts_node_ = node->GetChild("nExhausts");

            if (child_elem.HasChild("sound"))
            {
                ship->engine_sound = cache->GetResource<Urho3D::Sound>(child_elem.GetChild("sound").GetAttribute("value"));
                ship->engine_sound->SetLooped(true);

                Urho3D::SoundSource3D* engine_sound_source = node->GetChild("nExhausts")->CreateComponent<Urho3D::SoundSource3D>();
                engine_sound_source->SetSoundType(Urho3D::SOUND_EFFECT);
                engine_sound_source->SetNearDistance(20);
                engine_sound_source->SetFarDistance(80);
                engine_sound_source->SetGain(0.15f);
            }
        }
    }

    return ship;
}


VehicleWeaponCtrl* LoadWeapon(const Urho3D::String file, Urho3D::Scene* scene, Urho3D::ResourceCache* cache)
{
    Urho3D::XMLFile* xml_file = cache->GetResource<Urho3D::XMLFile>(file);
    const Urho3D::XMLElement& xml_elem = xml_file->GetRoot();

    //  Create weapon node tree
    Urho3D::Node* node = scene->CreateChild();
    LoadNode(node, xml_elem, cache);

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


KinematicCharacterCtrl* LoadKinematicCharacter(const Urho3D::String file, Urho3D::Scene* scene, Urho3D::ResourceCache* cache)
{
    Urho3D::XMLFile* xml_file = cache->GetResource<Urho3D::XMLFile>(file);
    const Urho3D::XMLElement& xml_elem = xml_file->GetRoot();

    Urho3D::Node* node = scene->CreateChild();
    Urho3D::Node* spin = node->CreateChild();
    LoadNode(spin, xml_elem, cache);
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
    LoadNode(node, xml_elem, cache);


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


//  ok
void LoadNode(Urho3D::Node* node, const Urho3D::XMLElement& xml_elem, Urho3D::ResourceCache* cache)
{
    if (xml_elem.HasAttribute("name"))
        node->SetName(xml_elem.GetAttribute("name"));

    if (xml_elem.HasChild("model"))
    {
        Urho3D::String model_type = xml_elem.GetChild("model").GetAttribute("type");
        if (model_type.Compare("static") == 0)
        {
            //node->SetVar("model", "static");
            Urho3D::StaticModel* model = node->CreateComponent<Urho3D::StaticModel>();
            model->SetModel(cache->GetResource<Urho3D::Model>(xml_elem.GetChild("model").GetAttribute("model")));
            Urho3D::Material* m = cache->GetResource<Urho3D::Material>(xml_elem.GetChild("model").GetAttribute("material"));
            model->SetMaterial(m->Clone());

            if (xml_elem.GetChild("model").HasAttribute("shadows"))
            {
                if (xml_elem.GetChild("model").GetInt("shadows") == 1)
                    model->SetCastShadows(true);
                else
                    model->SetCastShadows(false);
            }

        }
        else if (model_type.Compare("animated") == 0)
        {
            //node->SetVar("model", "animated");
            Urho3D::AnimatedModel* model = node->CreateComponent<Urho3D::AnimatedModel>();
            model->SetModel(cache->GetResource<Urho3D::Model>(xml_elem.GetChild("model").GetAttribute("model")));
            node->CreateComponent<Urho3D::AnimationController>();
            Urho3D::Material* m = cache->GetResource<Urho3D::Material>(xml_elem.GetChild("model").GetAttribute("material"));
            model->SetMaterial(m->Clone());

            if (xml_elem.GetChild("model").HasAttribute("shadows"))
            {
                if (xml_elem.GetChild("model").GetInt("shadows") == 1)
                    model->SetCastShadows(true);
                else
                    model->SetCastShadows(false);
            }
        }
        if (xml_elem.GetChild("model").HasAttribute("collision"))
        {
            Urho3D::CollisionShape* coll_shape = node->CreateComponent<Urho3D::CollisionShape>();
            coll_shape->SetConvexHull(cache->GetResource<Urho3D::Model>(xml_elem.GetChild("model").GetAttribute("collision")));
        }
    }


    if (xml_elem.HasChild("position"))
        node->SetPosition(xml_elem.GetChild("position").GetVector3("value"));
    else
        node->SetPosition(Urho3D::Vector3(0.0f, 0.0f, 0.0f));

    if (xml_elem.HasChild("rotation"))
        node->SetRotation(xml_elem.GetChild("rotation").GetQuaternion("value"));
    else
        node->SetRotation(Urho3D::Quaternion(0.0f, 0.0f, 0.0f));

    if (xml_elem.HasChild("scale"))
        node->SetScale(xml_elem.GetChild("scale").GetVector3("value"));
    else
        node->SetScale(Urho3D::Vector3(1.0f, 1.0f, 1.0f));

    if (xml_elem.HasChild("effect"))
    {
        if (xml_elem.GetChild("effect").GetAttribute("type") == "particle")
            LoadParticleEmitter(xml_elem.GetChild("effect").GetAttribute("value"), node, cache);
        else if (xml_elem.GetChild("effect").GetAttribute("type") == "ribbon")
            LoadRibbonTrail(xml_elem.GetChild("effect").GetAttribute("value"), node, cache);
    }

    for (Urho3D::XMLElement child_source = xml_elem.GetChild("node"); child_source; child_source = child_source.GetNext("node"))
    {
        LoadNode(node->CreateChild(), child_source, cache);
    }
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

}
