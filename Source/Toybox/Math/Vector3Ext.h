#pragma once

#include "../Core/IncGuards.h"

THIRD_PARTY_GUARDS_BEGIN
#include <Urho3D/Math/Vector3.h>
THIRD_PARTY_GUARDS_END


namespace Toybox
{

Urho3D::Vector3 Bounce(const Urho3D::Vector3& vec, const Urho3D::Vector3& norm);

Urho3D::Vector3 Slide(const Urho3D::Vector3& vec, const Urho3D::Vector3& norm);

Urho3D::Vector3 Align(const Urho3D::Vector3& vec1, const Urho3D::Vector3& vec2);

}
