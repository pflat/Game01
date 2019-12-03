#pragma once

#include "../Core/IncGuards.h"

THIRD_PARTY_GUARDS_BEGIN
#include <Urho3D/Math/MathDefs.h>
THIRD_PARTY_GUARDS_END


namespace Toybox
{

class AxisCtrlSystem
{

public:

    AxisCtrlSystem() :
            enabled_(false),
            step_(1),
            min_(-10),
            max_(10),
            damp_(1),
            target_(0),
            current_(0)
    {}

    void Set(float step, float min, float max, float damp);

    void Update();

    void SetEnable(bool enabled) { enabled_ = enabled; }

    bool Enabled() { return enabled_; }

    void Increase() { target_ += step_; }

    void Decrease() { target_ -= step_; }

    void SetMax() { target_ = max_; }

    void SetMin() { target_ = min_; }

    void Stop() { target_ = 0; }

    float Value() { return current_; }

    float Factor() { return Urho3D::Abs(current_ / max_); }

private:
    ///  System enable state.
    bool enabled_;
    ///  Amount changed with every increase/decrease of the system.
    float step_;
    ///  Minimum value allowed by the system.
    float min_;
    ///  Maximum value allowed by the system.
    float max_;
    ///  Amount changed every time the system is trying to return to zero.
    float damp_;
    ///  The value the system should have.
    float target_;
    ///  The value the system currently has.
    float current_;

};

}
