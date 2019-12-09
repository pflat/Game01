#pragma once

#include "../Core/IncGuards.h"

THIRD_PARTY_GUARDS_BEGIN
//#include <Urho3D/Math/MathDefs.h>
#include <Urho3D/Math/Vector3.h>
#include <Urho3D/Math/Quaternion.h>
THIRD_PARTY_GUARDS_END


namespace Toybox
{


enum AxisCtrlMode
{
    AXIS_MODE_SET = 1 << 0,
    AXIS_MODE_ACTION = 1 << 1,
    AXIS_COORD_LOCAL = 1 << 16,
    AXIS_COORD_WORLD = 1 << 17
};


///  Very important to keep in sync with Controls.h
enum AxisAction
{
    INCREASE = 1 << 0,
    DECREASE = 1 << 1,
    SET_MAX = 1 << 2,
    SET_MIN = 1 << 3,
    STOP = 1 << 4
};


class AxisCtrlSystem
{

public:

    AxisCtrlSystem() :
            enabled_(false),
            mode_(AxisCtrlMode(AXIS_MODE_ACTION | AXIS_COORD_LOCAL)),
            step_(1),
            min_(-10),
            max_(10),
            damp_(1),
            target_(0),
            current_(0)
    {}

    AxisCtrlMode Set(const Urho3D::String& mode, const Urho3D::String& coord);

    void Set(float step, float min, float max, float damp);

    void Set(AxisCtrlMode mode, float step, float min, float max, float damp);

    void Update();

    void Update(int action);

    void SetEnable(bool enabled) { enabled_ = enabled; }

    bool Enabled() { return enabled_; }

    void Increase() { target_ += step_; }

    void Decrease() { target_ -= step_; }

    void SetMax() { target_ = max_; }

    void SetMin() { target_ = min_; }

    void Stop() { target_ = 0; }

    float Value() { return current_; }

    float Factor() { return Urho3D::Abs(current_ / max_); }

    Urho3D::Vector3 Velocity(const Urho3D::Vector3& axis, const Urho3D::Quaternion& rot);

private:
    ///  System enable state.
    bool enabled_;
    ///  Operation mode. ...
    ///  Coordinate mode. Local space (airplane thrust), or world space (submarine or hot air balloon altitude).
    AxisCtrlMode mode_;
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
