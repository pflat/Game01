#pragma once

#include "../Core/IncGuards.h"

THIRD_PARTY_GUARDS_BEGIN
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Resource/ResourceCache.h>
THIRD_PARTY_GUARDS_END

namespace Toybox
{

class IO
{

public:

    static void LoadNode(Urho3D::Node* node, const Urho3D::XMLElement& xml_elem, Urho3D::ResourceCache* cache);

    static void LoadBody(Urho3D::Node* node, const Urho3D::XMLElement& xml_elem);

    static void LoadModel(Urho3D::Node* node, const Urho3D::XMLElement& xml_elem, Urho3D::ResourceCache* cache);

};

}
