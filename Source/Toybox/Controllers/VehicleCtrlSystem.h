#pragma once

#include "../Core/IncGuards.h"

THIRD_PARTY_GUARDS_BEGIN
#include <Urho3D/Math/MathDefs.h>
THIRD_PARTY_GUARDS_END


namespace Toybox
{

class VehicleCtrlSystem
{

public:

    VehicleCtrlSystem() :
            step_(1),
            min_(-10),
            max_(10),
            damp_(1),
            target_(0),
            current_(0)
    {}

    void Set(float step, float min, float max, float damp);

    void Update();

    void Increase() { target_ += step_; }

    void Decrease() { target_ -= step_; }

    void SetMax() { target_ = max_; }

    void SetMin() { target_ = min_; }

    void Stop() { target_ = 0; }

    float Value() { return current_; }

    float Factor() { return Urho3D::Abs(current_ / max_); }

private:

    float step_;
    float min_;
    float max_;
    float damp_;
    float target_;
    float current_;

};

}
