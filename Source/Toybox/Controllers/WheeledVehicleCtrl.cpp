#include "../Controllers/WheeledVehicleCtrl.h"

THIRD_PARTY_GUARDS_BEGIN
//#include <Urho3D/IO/MemoryBuffer.h>
//#include <Urho3D/Math/Ray.h>
//#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/PhysicsEvents.h>

//#include <Bullet/BulletCollision/BroadphaseCollision/btDbvtBroadphase.h>
//#include <Bullet/BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h>
//#include <Bullet/BulletCollision/CollisionDispatch/btInternalEdgeUtility.h>
//#include <Bullet/BulletCollision/CollisionShapes/btBoxShape.h>
//#include <Bullet/BulletCollision/CollisionShapes/btSphereShape.h>
#include <Bullet/BulletCollision/CollisionShapes/btCapsuleShape.h>
//#include <Bullet/BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h>
//#include <Bullet/BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
THIRD_PARTY_GUARDS_END


namespace Toybox
{

WheeledVehicleCtrl::WheeledVehicleCtrl(Urho3D::Context* context) :
    Urho3D::LogicComponent(context)
{
    SetUpdateEventMask(Urho3D::USE_FIXEDUPDATE);
}


void WheeledVehicleCtrl::RegisterObject(Urho3D::Context* context)
{
    context->RegisterFactory<WheeledVehicleCtrl>();

    // These macros register the class attributes to the Context for automatic load / save handling.
    // We specify the Default attribute mode which means it will be used both for saving into file, and network replication
//    URHO3D_ATTRIBUTE("Controls Yaw", float, controls_.yaw_, 0.0f, AM_DEFAULT);
//    URHO3D_ATTRIBUTE("Controls Pitch", float, controls_.pitch_, 0.0f, AM_DEFAULT);
//    URHO3D_ATTRIBUTE("On Ground", bool, onGround_, false, AM_DEFAULT);
//    URHO3D_ATTRIBUTE("OK To Jump", bool, okToJump_, true, AM_DEFAULT);
//    URHO3D_ATTRIBUTE("In Air Timer", float, inAirTimer_, 0.0f, AM_DEFAULT);
}


void WheeledVehicleCtrl::Start()
{
    // Component has been inserted into its scene node. Subscribe to events now
}


void WheeledVehicleCtrl::FixedUpdate(float time_step)
{

}

}
