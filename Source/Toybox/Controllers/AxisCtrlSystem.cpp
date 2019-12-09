#include "../Controllers/AxisCtrlSystem.h"

namespace Toybox
{

AxisCtrlMode AxisCtrlSystem::Set(const Urho3D::String& mode, const Urho3D::String& coord)
{
    if (mode.Compare("action") == 0)
        mode_ = AXIS_MODE_ACTION;
    else if (mode.Compare("set") == 0)
        mode_ = AXIS_MODE_SET;
    else
        mode_ = AXIS_MODE_ACTION;

    if (coord.Compare("local") == 0)
        mode_ = AxisCtrlMode(mode_ | AXIS_COORD_LOCAL);
    else if (coord.Compare("world") == 0)
        mode_ = AxisCtrlMode(mode_ | AXIS_COORD_WORLD);
    else
        mode_ = AxisCtrlMode(mode_ | AXIS_COORD_LOCAL);

    return mode_;
}


void AxisCtrlSystem::Set(float step, float min, float max, float damp)
{
    step_ = step;
    min_ = min;
    max_ = max;
    damp_ = damp;
}


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


void AxisCtrlSystem::Update(int action)
{
    if (mode_ & AXIS_MODE_SET)
    {
        if (action == INCREASE)
        {
            Increase();
        }
        else if (action == DECREASE)
        {
            Decrease();
        }
        else if (action == SET_MAX)
        {
            SetMax();
        }
        else if (action == SET_MIN)
        {
            SetMin();
        }
        else if (action == STOP)
        {
            Stop();
        }
    }
    else if (mode_ & AXIS_MODE_ACTION)
    {
        if (action == INCREASE)
        {
            SetMax();
        }
        else if (action == DECREASE)
        {
            SetMin();
        }
        else
        {
            Stop();
        }
    }

    Update();
}


Urho3D::Vector3 AxisCtrlSystem::Velocity(const Urho3D::Vector3& axis, const Urho3D::Quaternion& rot)
{
    if (mode_ & AXIS_COORD_LOCAL)
    {
        return rot * axis * current_;
    }
    else
    {
        return axis * current_;
    }
}

}
