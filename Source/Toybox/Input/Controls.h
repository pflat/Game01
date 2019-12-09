#pragma once

#include "../Core/IncGuards.h"

THIRD_PARTY_GUARDS_BEGIN
#include <Urho3D/Input/Input.h>
THIRD_PARTY_GUARDS_END


namespace Toybox
{


class Controls
{
public:
    Controls();

    void Update();
    void Update(Urho3D::Input* input);

	Urho3D::IntVector2 MouseMove();
	int MouseWheel();

	void Set(int ctrl, bool down = true);
    bool IsDown(int ctrl);
    bool IsJustDown(int ctrl);
    bool IsJustUp(int ctrl);

    int State() { return controls_; }

    void Reset();

private:
	void SetMouseState(Urho3D::Input* input);

	Urho3D::IntVector2 mouse_move_;
    int mouse_wheel_;
    int controls_;
    int prev_controls_;

};

}
