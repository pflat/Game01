#include "../Controllers/DynamicCharacterCtrl.h"
#include "../Core/Debug.h"

THIRD_PARTY_GUARDS_BEGIN
#include <Urho3D/IO/MemoryBuffer.h>
THIRD_PARTY_GUARDS_END


namespace Toybox
{

DynamicCharacter::DynamicCharacter(Urho3D::Context* context) :
    Urho3D::LogicComponent(context),
    frame_counter(0),
    body(0),
    camera_node(0)
{
    SetUpdateEventMask(Urho3D::USE_FIXEDUPDATE);
}


void DynamicCharacter::RegisterObject(Urho3D::Context* context)
{
    context->RegisterFactory<DynamicCharacter>();

    // These macros register the class attributes to the Context for automatic load / save handling.
    // We specify the Default attribute mode which means it will be used both for saving into file, and network replication
//    URHO3D_ATTRIBUTE("Controls Yaw", float, controls_.yaw_, 0.0f, AM_DEFAULT);
//    URHO3D_ATTRIBUTE("Controls Pitch", float, controls_.pitch_, 0.0f, AM_DEFAULT);
//    URHO3D_ATTRIBUTE("On Ground", bool, onGround_, false, AM_DEFAULT);
//    URHO3D_ATTRIBUTE("OK To Jump", bool, okToJump_, true, AM_DEFAULT);
//    URHO3D_ATTRIBUTE("In Air Timer", float, inAirTimer_, 0.0f, AM_DEFAULT);
}


void DynamicCharacter::Start()
{

    body = node_->GetComponent<Urho3D::RigidBody>(true);
//    animation_ctrl = node_->GetComponent<Urho3D::AnimationController>(true);

    camera_node = node_->GetChild("Camera", true);

    // Component has been inserted into its scene node. Subscribe to events now
    SubscribeToEvent(GetNode(), Urho3D::E_NODECOLLISION, URHO3D_HANDLER(DynamicCharacter, HandleNodeCollision));
}


void DynamicCharacter::FixedUpdate(float time_step)
{
    printf("FIXED\n");
    const float FORWARD_SPEED = 10.0f;
    const float BACKWARD_SPEED = 5.0f;
    const float STRAFE_SPEED = 7.0f;
    const float JUMP_SPEED = 150.0f;
    const float ROTATION_SPEED = 2.0f;
    const float WALK_FACTOR = 0.65f;


    Urho3D::PhysicsWorld* physics_world = node_->GetScene()->GetComponent<Urho3D::PhysicsWorld>();

    //Urho3D::Vector3 body_pos = body->GetPosition();
    //Urho3D::Vector3 forward_dir = (body->GetRotation() * Urho3D::Vector3::FORWARD).Normalized();
    //Urho3D::Vector3 right_dir = (body->GetRotation() * Urho3D::Vector3::RIGHT).Normalized();
    Urho3D::Quaternion body_rot = body->GetRotation();

    Urho3D::Vector3 velocity;
    if (controls_.IsDown(KM_CHARACTER_FORWARD))
        velocity += Urho3D::Vector3::FORWARD;
    if (controls_.IsDown(KM_CHARACTER_BACKWARD))
        velocity += Urho3D::Vector3::BACK;
    if (controls_.IsDown(KM_CHARACTER_STRAFE_LEFT))
        velocity += Urho3D::Vector3::LEFT;
    if (controls_.IsDown(KM_CHARACTER_STRAFE_RIGHT))
        velocity += Urho3D::Vector3::RIGHT;

    velocity.Normalize();
    //PrintVector(velocity);
    //body->SetLinearVelocity(velocity * 10);

}


void DynamicCharacter::HandleNodeCollision(Urho3D::StringHash event_type, Urho3D::VariantMap& event_data)
{
    printf("HANDLER\n");
    return;
    const Urho3D::RigidBody* body = (Urho3D::RigidBody*)event_data["Body"].GetVoidPtr();
    const Urho3D::Node* node = body->GetNode();
//    const Urho3D::String name = node->GetName();
//    const Urho3D::RigidBody* other_body = (Urho3D::RigidBody*)event_data["OtherBody"].GetVoidPtr();
//    const Urho3D::Node* other_node = other_body->GetNode();
//    const Urho3D::String other_name = other_node->GetName();
//    printf("%s %s\n", name.CString(), other_name.CString());

    // Check collision contacts and see if character is standing on ground (look for a contact that has near vertical normal)
    Urho3D::MemoryBuffer contacts(event_data[Urho3D::NodeCollision::P_CONTACTS].GetBuffer());
    while (!contacts.IsEof())
    {
        Urho3D::Vector3 contactPosition = contacts.ReadVector3();
        Urho3D::Vector3 contactNormal = contacts.ReadVector3();
        float contactDistance = contacts.ReadFloat();
        float contactImpulse = contacts.ReadFloat();

        //  If contact is in lower capsule half-circle and pointing up, assume it's a ground contact
        if (contactPosition.y_ < (node->GetPosition().y_ + 0.175f))
        {
//            on_ground = true;
            float level = contactNormal.y_;
            if (level > 0.80)
            {
//                on_slope = false;
            }
            else
            {
//                on_slope = true;
            }
        }
        else
        {
//            on_ground = false;
//            on_slope = false;
        }
    }
}

}
