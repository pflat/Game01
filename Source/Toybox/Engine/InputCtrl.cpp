#include "../Engine/InputCtrl.h"

namespace Toybox
{

InputCtrl::Mode InputCtrl::Set(const Urho3D::String& mode, const Urho3D::String& coord)
{
    if (mode.Compare("set") == 0)
        mode_ = InputCtrl::MODE_SET;
    else
        mode_ = InputCtrl::MODE_ACTION;

    if (coord.Compare("world") == 0)
        mode_ = InputCtrl::Mode(mode_ | InputCtrl::COORD_WORLD);
    else
        mode_ = InputCtrl::Mode(mode_ | InputCtrl::COORD_LOCAL);

    return mode_;
}


void InputCtrl::Set(float min, float max, float step, float accel, float damp)
{
    min_ = min;
    max_ = max;
    step_ = step;
    accel_ = accel;
    damp_ = damp;
}


void InputCtrl::Set(InputCtrl::Mode mode, float min, float max, float step, float accel, float damp)
{
    mode_ = mode;
    Set(min, max, step, accel, damp);
}


float InputCtrl::Factor()
{
    if (value_ > 0.0f)
        return (value_ / max_);
    else if (value_ < 0.0f)
        return (value_ / min_);
    else return 0.0f;
}


void InputCtrl::Update()
{
    target_ = Urho3D::Clamp(target_, min_, max_);
    if (value_ != target_)
    {
/*
        if (target_ == 0)
        {
            {
                value_ = value_ * (1 - damp_);
            }
        }
        else
*/
        {
            value_ = value_ * (1 - accel_) + (target_ * accel_);
        }

    }

    //  To be replaced with start-stop engine mechanic
//    if (Urho3D::Abs<float>(value_) < (max_ * 0.02f))
//        value_ = 0.0f;
}


Urho3D::Vector3 InputCtrl::Velocity(const Urho3D::Vector3& axis, const Urho3D::Quaternion& rot)
{
    if (mode_ & InputCtrl::COORD_LOCAL)
    {
        return rot * axis * value_;
    }
    else
    {
        return axis * value_;
    }
}

}
