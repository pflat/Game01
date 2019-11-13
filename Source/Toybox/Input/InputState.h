#pragma once

#include "../Core/IncGuards.h"

THIRD_PARTY_GUARDS_BEGIN
#include <Urho3D/Input/Input.h>
THIRD_PARTY_GUARDS_END


namespace Toybox
{


class InputState
{
public:

    int mouse_x;
    int mouse_y;
    int mouse_wheel;
    unsigned mouse_buttons;
    unsigned prev_mouse_buttons;
    unsigned keyboard_keys;
    unsigned prev_keyboard_keys;
    int keyboard_qualifiers;
    int prev_keyboard_qualifiers;

    InputState();
    ~InputState();

    void Update();
    void Update(Urho3D::Input* input);

    void SetQualifiers(int qualifiers);
    void SetKey(unsigned key, bool down = true);
    bool IsKeyDown(unsigned key) const;
    bool IsKeyJustDown(unsigned key) const;

    void SetMouseState(Urho3D::Input* input);
    void SetButton(unsigned button, bool down = true);
    bool IsButtonDown(unsigned button) const;
    bool IsButtonJustDown(unsigned button) const;

    void Reset();
};

}
