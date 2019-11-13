#include "../Controllers/VehicleCtrlSystem.h"

namespace Toybox
{

void VehicleCtrlSystem::Set(float step, float min, float max, float damp)
{
    step_ = step;
    min_ = min;
    max_ = max;
    damp_ = damp;
}

void VehicleCtrlSystem::Update()
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

}
