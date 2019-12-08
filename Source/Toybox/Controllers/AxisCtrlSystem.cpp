#include "../Controllers/AxisCtrlSystem.h"

namespace Toybox
{

void AxisCtrlSystem::Set(AxisCtrlMode mode, float step, float min, float max, float damp)
{
    mode_ = mode;
    step_ = step;
    min_ = min;
    max_ = max;
    damp_ = damp;
}

void AxisCtrlSystem::Update()
{
    if (!enabled_)
        return;

    target_ = Urho3D::Clamp(target_, min_, max_);
    if (current_ != target_)
    {
        if (target_ == 0)
        {
            if (current_ > 0)
            {
                current_ -= damp_;
                current_ = Urho3D::Max<float>(0.0f, current_);
            }
            else if (current_ < 0)
            {
                current_ += damp_;
                current_ = Urho3D::Min<float>(0.0f, current_);
            }
        }
        else if (target_ > current_)
        {
            current_ += step_;
            current_ = Urho3D::Min<float>(target_, current_);
        }
        else if (target_ < current_)
        {
            current_ -= step_;
            current_ = Urho3D::Max<float>(target_, current_);
        }
    }
}


Urho3D::Vector3 AxisCtrlSystem::Velocity(const Urho3D::Vector3& axis, const Urho3D::Quaternion& rot)
{
    if (mode_ == AXIS_MODE_LOCAL)
    {
        return rot * axis * current_;
    }
    else
    {
        return axis * current_;
    }
}

}
