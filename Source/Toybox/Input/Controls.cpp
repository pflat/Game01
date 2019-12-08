#include "../Input/Controls.h"


namespace Toybox
{

Controls::Controls() :
	mouse_move_(Urho3D::IntVector2::ZERO),
    mouse_wheel_(0),
    controls_(0),
    prev_controls_(0)
{
}


void Controls::Update()
{
    prev_controls_ = controls_;
}


void Controls::Update(Urho3D::Input* input)
{
    Update();
    SetMouseState(input);
}


void Controls::SetMouseState(Urho3D::Input* input)
{
    mouse_move_ = input->GetMouseMove();
    mouse_wheel_ = input->GetMouseMoveWheel();
}


Urho3D::IntVector2 Controls::MouseMove()
{
	return mouse_move_;
}


int Controls::MouseWheel()
{
	return mouse_wheel_;
}


void Controls::Set(int ctrl, bool down)
{
    if (down)
        controls_ |= ctrl;
    else
        controls_ &= ~ctrl;
}


bool Controls::IsDown(int ctrl)
{
    return (controls_ & ctrl) != 0;
}


bool Controls::IsJustDown(int ctrl)
{
    return (controls_ & ctrl) != 0 && (prev_controls_ & ctrl) == 0;
}


bool Controls::IsJustUp(int ctrl)
{
    return false;
}


void Controls::Reset()
{
    mouse_move_ = Urho3D::IntVector2::ZERO;
    controls_ = 0;
    prev_controls_ = 0;
}

}
