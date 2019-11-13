#pragma once

#include "../Core/Settings.h"
#include "../Input/InputState.h"


namespace Toybox
{

class KinematicCharControls
{

public:

    KinematicCharControls() :
        forward_vel_(0.0f),
        strafe_vel_(0.0f),
        jump_vel_(0.0f),
        rotation_(0.0f),
        direction_(Urho3D::Vector3::ZERO)
    {
    }

    void Update(InputState* input)
    {
        const float FORWARD_SPEED = 10.0f;
        const float BACKWARD_SPEED = 5.0f;
        const float STRAFE_SPEED = 7.0f;
        const float JUMP_SPEED = 150.0f;
        const float ROTATION_SPEED = 2.0f;
        const float WALK_FACTOR = 0.25f;

        // -------------------------------

        if (input->IsKeyDown(KM_CHARACTER_FORWARD))
        {
            direction_.z_ = 1.0f;
        }
        else if (input->IsKeyDown(KM_CHARACTER_BACKWARD))
        {
            direction_.z_ = -1.0f;
        }
        else
        {
            direction_.z_ = 0.0f;
        }

        if (input->IsKeyDown(KM_CHARACTER_STRAFE_LEFT))
        {
            direction_.x_ = -1.0f;
        }
        else if (input->IsKeyDown(KM_CHARACTER_STRAFE_RIGHT))
        {
            direction_.x_ = 1.0f;
        }
        else
        {
            direction_.x_ = 0.0f;
        }

        // -------------------------------

        float angle = direction_.Normalized().Angle(Urho3D::Vector3(1.0f, 0.0f, 0.0f));
        if (direction_.z_ < 0.0f)
            angle *= -1;

        if (input->IsKeyDown(KM_CHARACTER_FORWARD))
        {
            forward_vel_ = Urho3D::Sin(angle) * FORWARD_SPEED;
        }
        else if (input->IsKeyDown(KM_CHARACTER_BACKWARD))
        {
            forward_vel_ = Urho3D::Sin(angle) * BACKWARD_SPEED;
        }
        else
        {
            forward_vel_ = 0.0f;
        }

        if (input->IsKeyDown(KM_CHARACTER_STRAFE_LEFT) || input->IsKeyDown(KM_CHARACTER_STRAFE_RIGHT))
        {
            strafe_vel_ = Urho3D::Cos(angle) * STRAFE_SPEED;
        }
        else
        {
            strafe_vel_ = 0.0f;
        }

        // -------------------------------

        if (input->IsKeyDown(KM_CHARACTER_TURN_LEFT))
        {
            rotation_ -= ROTATION_SPEED;
            if (rotation_ < -180)
                rotation_ = 360.0f + rotation_;
        }
        else if (input->IsKeyDown(KM_CHARACTER_TURN_RIGHT))
        {
            rotation_ += ROTATION_SPEED;
            if (rotation_ > 180)
                rotation_ = -360.0f + rotation_;
        }

        // -------------------------------

        if (input->IsKeyDown(KM_CHARACTER_JUMP))// && !is_jumping && air_time < 0.5f)
        {
            jump_vel_ = JUMP_SPEED;
        }
        else
        {
            jump_vel_ = 0.0f;
        }

        // -------------------------------

        if (input->IsKeyDown(KM_CHARACTER_WALK))
        {
            forward_vel_ *= WALK_FACTOR;
            strafe_vel_ *= WALK_FACTOR;
        }
    }

public:

    float forward_vel_;
    float strafe_vel_;
    float jump_vel_;
    float rotation_;

private:

    Urho3D::Vector3 direction_;

};

}
