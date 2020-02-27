#include "IO.h"

THIRD_PARTY_GUARDS_BEGIN
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/AnimationController.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/CollisionShape.h>
#include <Urho3D/Resource/XMLElement.h>
//#include <Urho3D/Resource/JSONFile.h>
THIRD_PARTY_GUARDS_END

namespace Toybox
{

void IO::LoadNode(Urho3D::Node* node, const Urho3D::XMLElement& xml_elem, Urho3D::ResourceCache* cache)
{
    if (xml_elem.HasAttribute("name"))
        node->SetName(xml_elem.GetAttribute("name"));

    if (xml_elem.HasAttribute("position"))
        node->SetPosition(xml_elem.GetVector3("position"));
    else
        node->SetPosition(Urho3D::Vector3(0.0f, 0.0f, 0.0f));

    if (xml_elem.HasAttribute("rotation"))
        node->SetRotation(xml_elem.GetQuaternion("rotation"));
    else
        node->SetRotation(Urho3D::Quaternion(0.0f, 0.0f, 0.0f));

    if (xml_elem.HasAttribute("scale"))
        node->SetScale(xml_elem.GetVector3("scale"));
    else
        node->SetScale(Urho3D::Vector3(1.0f, 1.0f, 1.0f));

    LoadBody(node, xml_elem);

    LoadModel(node, xml_elem, cache);

/*
    if (xml_elem.HasChild("effect"))
    {
        if (xml_elem.GetChild("effect").GetAttribute("type") == "particle")
            LoadParticleEmitter(xml_elem.GetChild("effect").GetAttribute("value"), node, cache);
        else if (xml_elem.GetChild("effect").GetAttribute("type") == "ribbon")
            LoadRibbonTrail(xml_elem.GetChild("effect").GetAttribute("value"), node, cache);
    }
*/
    for (Urho3D::XMLElement child_source = xml_elem.GetChild("node"); child_source; child_source = child_source.GetNext("node"))
    {
        LoadNode(node->CreateChild(), child_source, cache);
    }

}


void IO::LoadBody(Urho3D::Node* node, const Urho3D::XMLElement& xml_elem)
{
    //  Define RigidBody component
    if (xml_elem.HasChild("physics"))
    {
        Urho3D::RigidBody* body = node->CreateComponent<Urho3D::RigidBody>();

        if (xml_elem.GetChild("physics").HasAttribute("mass"))
            body->SetMass(xml_elem.GetChild("physics").GetFloat("mass") / 10.0f);
        else
            body->SetMass(0.0f);

        if (xml_elem.GetChild("physics").HasAttribute("linear_damp"))
            body->SetLinearDamping(xml_elem.GetChild("physics").GetFloat("linear_damp"));

        if (xml_elem.GetChild("physics").HasAttribute("angular_damp"))
            body->SetAngularDamping(xml_elem.GetChild("physics").GetFloat("angular_damp"));

        if (xml_elem.GetChild("physics").HasAttribute("friction"))
            body->SetFriction(xml_elem.GetChild("physics").GetFloat("friction"));

        //body->SetAngularFactor(Urho3D::Vector3::ZERO);

        //body->SetLinearDamping(0.5f);
        //body->SetAngularDamping(0.5f);
        //body->SetFriction(0.5f);
    }
}


void IO::LoadModel(Urho3D::Node* node, const Urho3D::XMLElement& xml_elem, Urho3D::ResourceCache* cache)
{
    //  Define Model component
    if (xml_elem.HasChild("model"))
    {
        Urho3D::String model_type = "static";
        if (xml_elem.GetChild("model").HasAttribute("type"))
            model_type = xml_elem.GetChild("model").GetAttribute("type");

        if (model_type.Compare("static") == 0)
        {
            //node->SetVar("model", "static");
            Urho3D::StaticModel* model = node->CreateComponent<Urho3D::StaticModel>();
            model->SetModel(cache->GetResource<Urho3D::Model>(xml_elem.GetChild("model").GetAttribute("model")));
            Urho3D::Material* m = cache->GetResource<Urho3D::Material>(xml_elem.GetChild("model").GetAttribute("material"));
            model->SetMaterial(m->Clone());
            model->SetCastShadows(true);
        }
        else if (model_type.Compare("animated") == 0)
        {
            //node->SetVar("model", "animated");
            Urho3D::AnimatedModel* model = node->CreateComponent<Urho3D::AnimatedModel>();
            model->SetModel(cache->GetResource<Urho3D::Model>(xml_elem.GetChild("model").GetAttribute("model")));
            node->CreateComponent<Urho3D::AnimationController>();
            Urho3D::Material* m = cache->GetResource<Urho3D::Material>(xml_elem.GetChild("model").GetAttribute("material"));
            model->SetMaterial(m->Clone());
            model->SetCastShadows(true);
        }

        //  Define CollisionShape component
        if (xml_elem.GetChild("model").HasAttribute("collision"))
        {
            Urho3D::CollisionShape* coll_shape = node->CreateComponent<Urho3D::CollisionShape>();
            coll_shape->SetConvexHull(cache->GetResource<Urho3D::Model>(xml_elem.GetChild("model").GetAttribute("collision")));
        }
    }
}

}
