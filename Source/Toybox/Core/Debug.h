#pragma once

#include "../Core/IncGuards.h"

THIRD_PARTY_GUARDS_BEGIN
#include <Urho3D/Graphics/Skeleton.h>
THIRD_PARTY_GUARDS_END


namespace Toybox
{

void PrintMessage(unsigned frame, const Urho3D::String& message);

void PrintVector(const Urho3D::Vector3& vec);

void PrintVector(const Urho3D::String& message, const Urho3D::Vector3& vec);

void PrintFormatedDate();

void PrintNodeTree(int tabs, Urho3D::Node* node);

void PrintSkeleton(int tabs, Urho3D::Skeleton* skel);

}
