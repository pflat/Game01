#pragma once

#include "../Core/IncGuards.h"
#include "../Core/ToyboxDefs.h"
#include "../Engine/Scene.h"
#include "../Controllers/CameraCtrl.h"
#include "../Controllers/ShipCtrl.h"
#include "../Controllers/CarCtrl.h"
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

void Init(Urho3D::Context* context);

Scene* LoadScene(const Urho3D::String file, Urho3D::Scene* scene, Urho3D::ResourceCache* cache);

///  Load the ship node tree into the scene. Returns the controller of the ship.
ShipCtrl* LoadShip(const Urho3D::String file, Urho3D::ResourceCache* cache, Urho3D::Scene* scene = 0, Urho3D::Node* node = 0);

CarCtrl* LoadCar(const Urho3D::String file, Urho3D::ResourceCache* cache, Urho3D::Scene* scene = 0, Urho3D::Node* node = 0);

VehicleWeaponCtrl* LoadWeapon(const Urho3D::String file, Urho3D::Scene* scene, Urho3D::ResourceCache* cache);

Urho3D::RibbonTrail* LoadRibbonTrail(const Urho3D::String file, Urho3D::Node* node, Urho3D::ResourceCache* cache);
Urho3D::ParticleEmitter* LoadParticleEmitter(const Urho3D::String file, Urho3D::Node* node, Urho3D::ResourceCache* cache);

///  Load character node tree into the scene. Returns the kinematic controller of the character.
KinematicCharacterCtrl* LoadKinematicCharacter(const Urho3D::String file, Urho3D::ResourceCache* cache, Urho3D::Scene* scene = 0, Urho3D::Node* node = 0);
///  Load character node tree into the scene. Returns the dynamic controller of the character.
DynamicCharacter* LoadDynamicCharacter(const Urho3D::String file, Urho3D::Scene* scene, Urho3D::ResourceCache* cache);

///  Creates a camera. Returns the controller of the camera.
CameraCtrl* CreateCamera(Urho3D::Context* context);


///  Test, debug and learning methods
///  SHOULD EVENTUALLY BE DELETED
void SaveJSON(Urho3D::Context* context, Urho3D::ResourceCache* cache);

void SaveTree(Urho3D::Context* context, Urho3D::Node* node);
}
