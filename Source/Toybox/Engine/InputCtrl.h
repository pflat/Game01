#pragma once

#include "../Core/IncGuards.h"

THIRD_PARTY_GUARDS_BEGIN
//#include <Urho3D/Math/MathDefs.h>
#include <Urho3D/Math/Vector3.h>
#include <Urho3D/Math/Quaternion.h>
THIRD_PARTY_GUARDS_END


namespace Toybox
{


class InputCtrl
{

public:

    enum Mode
    {
        MODE_SET = 1 << 0,      ///  Set a target value. Used by ship controller.
        MODE_ACTION = 1 << 1,   ///  While active, target is max/min value. Used by kinematic character controller and car controller.
                                ///  In both cases accel factor determines fast the value is reached.
        COORD_LOCAL = 1 << 8,   ///  Spaceship acceleration and rotation.
        COORD_WORLD = 1 << 9    ///  Submarine moving up/down.
    };

    InputCtrl() :
            enabled_(false),
            mode_(InputCtrl::Mode(MODE_ACTION | COORD_LOCAL)),
            min_(-1.0f),
            max_(1.0f),
            step_(0.1f),
            interval_(0.0f),
            accel_(0.5f),
            damp_(0.2f),
            target_(0.0f),
            value_(0.0f)
    {}

    InputCtrl::Mode Set(const Urho3D::String& mode, const Urho3D::String& coord);

    void Set(float min, float max, float step, float accel, float damp);

    void Set(InputCtrl::Mode mode, float min, float max, float step, float accel, float damp);

    void Update();

    void SetEnable(bool enabled) { enabled_ = enabled; }

    bool Enabled() { return enabled_; }

    void Increase() { target_ += step_; };

    void Decrease() { target_ -= step_; };

    void SetMax() { target_ = max_; }

    void SetMin() { target_ = min_; }

    void Stop() { target_ = 0.0f; }

    InputCtrl::Mode GetMode() { return mode_; }

    float Value() { return value_; }

    float Factor();

    Urho3D::Vector3 Velocity(const Urho3D::Vector3& axis, const Urho3D::Quaternion& rot);

private:
    ///  System enable state.
    bool enabled_;
    ///  Operation mode. ...
    ///  Coordinate mode. Local space (airplane thrust), or world space (submarine or hot air balloon altitude).
    InputCtrl::Mode mode_;
    ///  Minimum value allowed by the system.
    float min_;
    ///  Maximum value allowed by the system.
    float max_;
    float step_;
    float interval_;
    ///  Acceleration factor. How fast the system reaches the target value. [0,1]
    float accel_;
    ///  Damp factor. In the absence of  power, how fast the system reaches the value zero. [0,1]
    float damp_;
    ///  The value the system is aiming to reach.
    float target_;
    ///  The value the system currently has.
    float value_;

};

}
