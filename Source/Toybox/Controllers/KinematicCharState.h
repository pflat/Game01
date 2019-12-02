#pragma once

#include "../Core/IncGuards.h"

THIRD_PARTY_GUARDS_BEGIN
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Physics/PhysicsWorld.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Physics/CollisionShape.h>
THIRD_PARTY_GUARDS_END


namespace Toybox
{

enum KinematicState
{
    KCS_STANDING = 1,
    KCS_WALKING,
    KCS_RUNNING,
    KCS_JUMPING,
    KCS_FALLING,
    KCS_SLIDING,
    KCS_LANDING,
    KCS_SWIMMING,
    KCS_DIVING  //  Underwater swimming
};

/*
enum KinematicAction
{
    KCA_IDLE = 1,
    KCA_WALK,
    KCA_RUN,
    KCA_FALL,
    KCA_LAND
};
*/


class CollisionState
{
public:

    CollisionState() :
        state_(KCS_FALLING),
        ground_dist_(0.0f),
        ground_level_(0.0f),
        ground_normal_(Urho3D::Vector3::UP)
    {
    }

    //  This method returns KCS_STANDING, KCS_SLIDING or KCS_FALLING
    void Update(const Urho3D::PhysicsRaycastResult& raycast_result,
                const Urho3D::Vector3& velocity,
                float spring_height, float slope_factor)
    {
        //  ground_dist:
        //  negative - ground above spring end - character must move up
        //  positive - ground below spring end - character must move down
        if (raycast_result.body_)
            ground_dist_ = raycast_result.distance_ - spring_height;
        else
            ground_dist_ = Urho3D::M_INFINITY;

        ground_normal_ = raycast_result.normal_;
        ground_level_ = raycast_result.normal_.DotProduct(Urho3D::Vector3::UP);

        float hysteresis = 0.0001f;
        if (ground_dist_ > -(spring_height + hysteresis) && ground_dist_ < (spring_height + hysteresis))
        {
            //  Small hack to allow character liftoff. When starting a jump, if the amount moved isn't enough
            //  to get some distance from the floor the character would stay on the ground.
            if (state_ == KCS_JUMPING && velocity.y_ >= 0.0f)
                state_ = KCS_JUMPING;
            else if (ground_level_ < slope_factor)
                state_ = KCS_SLIDING;
            else
                state_ = KCS_STANDING;
        }
        else
        {
            if (state_ != KCS_JUMPING)
                state_ = KCS_FALLING;
        }
    }


    bool OnGround() const
    {
        return (state_ == KCS_STANDING || state_ == KCS_WALKING || state_ == KCS_RUNNING);
    }


    bool OnAir() const
    {
        return (state_ == KCS_JUMPING || state_ == KCS_FALLING || state_ == KCS_SLIDING);
    }


    bool OnWater() const
    {
        return (state_ == KCS_DIVING || state_ == KCS_SWIMMING);
    }


    bool IsJumping() const
    {
        return (state_ == KCS_JUMPING);
    }




	int state_;

    float ground_dist_;

    float ground_level_;

    Urho3D::Vector3 ground_normal_;
};

}
