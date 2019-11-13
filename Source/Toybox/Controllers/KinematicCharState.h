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
        //on_floor_(false), on_wall_(false), on_ceiling_(false), on_slope_(false),
        ground_dist_(0.0f),
        ground_level_(0.0f),
        move_pitch_(0.0f),
        ground_normal_(Urho3D::Vector3::UP)
    {
    }

    ///  This method returns KCS_STANDING, KCS_SLIDING or KCS_FALLING
    void Update(const Urho3D::PhysicsRaycastResult& raycast_result,
                const Urho3D::Vector3& velocity, float ground_dist,
                float spring_height, float spring_radius, float slope_factor)
    {
        state_ = KCS_FALLING;
        //on_floor_ = on_wall_ = on_ceiling_ = on_slope_ = false;

        //  ground_dist:
        //  negative - ground above spring end - character must move up
        //  positive - ground below spring end - character must move down
        ground_dist_ = ground_dist;

        ground_normal_ = raycast_result.normal_;
        ground_level_ = raycast_result.normal_.DotProduct(Urho3D::Vector3::UP);

        bool has_contact = false;
        if (ground_dist_ > -spring_height && ground_dist_ < spring_height)
            //state = KCS_STANDING;
            has_contact = true;

        if (has_contact == true)
        {
            if (ground_level_ < slope_factor)
                state_ = KCS_SLIDING;
            else
                state_ = KCS_STANDING;
        }
//        else
//        {
//            state_ = KCS_FALLING;
//        }

        Urho3D::Vector3 velXZ(velocity.x_, 0.0f, velocity.z_);
        move_pitch_ = velXZ.DotProduct(raycast_result.normal_);
    }


    bool OnGround() const
    {
        return (state_ == KCS_STANDING || state_ == KCS_WALKING || state_ == KCS_RUNNING);
    }


    bool OnAir() const
    {
        return (state_ == KCS_JUMPING || state_ == KCS_FALLING || state_ == KCS_SLIDING);// || state_ == KCS_LANDING);
    }


    bool OnWater() const
    {
        return (state_ == KCS_DIVING || state_ == KCS_SWIMMING);
    }


//    static const CharState EMPTY;


    void Print()
    {
        switch (state_)
        {
            case KCS_STANDING :
                printf("KCS_STANDING | ");
                break;
            case KCS_WALKING :
                printf("KCS_WALKING | ");
                break;
            case KCS_RUNNING :
                printf("KCS_RUNNING |");
                break;
            case KCS_JUMPING :
                printf("KCS_JUMPING | ");
                break;
            case KCS_FALLING :
                printf("KCS_FALLING | ");
                break;
            case KCS_SLIDING :
                printf("KCS_SLIDING | ");
                break;
            case KCS_LANDING :
                printf("KCS_LANDING | ");
                break;
            case KCS_SWIMMING :
                printf("KCS_SWIMMING | ");
                break;
            case KCS_DIVING :
                printf("KCS_DIVING | ");
                break;
            default:
                printf("UNKNOWN | ");
                break;
        }

//        printf("on_ground (%d) | on_slope (%d) | is_jumping (%d) | is_landing (%d)\n", on_ground, on_slope, is_jumping, is_landing);
        printf("%.6f | %.6f | %.6f\n", ground_dist_, ground_level_, move_pitch_);
//        printf("ground_normal (%.6f, %.6f, %.6f) | move_pitch (%d) \n", ground_normal.x_, ground_normal.y_, ground_normal.z_, move_pitch);
    }

	int state_;
    //int action_;
    //bool on_floor_, on_wall_, on_ceiling_, on_slope_;
    float ground_dist_;
    float ground_level_;
    float move_pitch_;  //  positive: up  negative: down
    Urho3D::Vector3 ground_normal_;

};

}
