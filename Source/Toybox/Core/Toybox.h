#pragma once

#include "../Core/IncGuards.h"
#include "../Core/ToyboxDefs.h"
#include "../Engine/Scene.h"
#include "../Controllers/CameraCtrl.h"
#include "../Controllers/ShipCtrl.h"
#include "../Controllers/KinematicCharacterCtrl.h"
#include "../Controllers/DynamicCharacterCtrl.h"
#include "../Controllers/VehicleWeaponCtrl.h"

THIRD_PARTY_GUARDS_BEGIN
#include <Urho3D/Core/Context.h>
//#include <Urho3D/Scene/Node.h>
//#include <Urho3D/Graphics/Skeleton.h>
#include <Urho3D/Graphics/ParticleEmitter.h>
#include <Urho3D/Graphics/RibbonTrail.h>
#include <Urho3D/Resource/ResourceCache.h>
THIRD_PARTY_GUARDS_END


namespace Toybox
{

void Init();

Scene* LoadScene(const Urho3D::String file, Urho3D::Scene* scene, Urho3D::ResourceCache* cache);

///  Load the ship node tree into the scene. Returns the controller of the ship.
ShipCtrl* LoadShip(const Urho3D::String file, Urho3D::Scene* scene, Urho3D::ResourceCache* cache);
//Urho3D::Node* LoadShip(const Urho3D::String file, Urho3D::Scene* scene, Urho3D::ResourceCache* cache);

VehicleWeaponCtrl* LoadWeapon(const Urho3D::String file, Urho3D::Scene* scene, Urho3D::ResourceCache* cache);

Urho3D::RibbonTrail* LoadRibbonTrail(const Urho3D::String file, Urho3D::Node* node, Urho3D::ResourceCache* cache);
Urho3D::ParticleEmitter* LoadParticleEmitter(const Urho3D::String file, Urho3D::Node* node, Urho3D::ResourceCache* cache);

///  Load character node tree into the scene. Returns the kinematic controller of the character.
KinematicCharacterCtrl* LoadKinematicCharacter(const Urho3D::String file, Urho3D::Scene* scene, Urho3D::ResourceCache* cache);
///  Load character node tree into the scene. Returns the dynamic controller of the character.
DynamicCharacter* LoadDynamicCharacter(const Urho3D::String file, Urho3D::Scene* scene, Urho3D::ResourceCache* cache);

///  Creates a camera. Returns the controller of the camera.
CameraCtrl* CreateCamera(Urho3D::Context* context);

void LoadNode(Urho3D::Node* node, const Urho3D::XMLElement& source, Urho3D::ResourceCache* cache);

///  Test, debug and learning methods
///  SHOULD EVENTUALLY BE DELETED
void SaveJSON(Urho3D::Context* context, Urho3D::ResourceCache* cache);
}
