#include "../Input/InputState.h"
#include "../Core/Settings.h"


namespace Toybox
{

InputState::InputState() :
    mouse_x(0),
    mouse_y(0),
    mouse_wheel(0),
    mouse_buttons(0),
    prev_mouse_buttons(0),
    keyboard_keys(0),
    prev_keyboard_keys(0),
    keyboard_qualifiers(0),
    prev_keyboard_qualifiers(0)
{
}


InputState::~InputState()
{
}


void InputState::Update()
{
    prev_mouse_buttons = mouse_buttons;
    prev_keyboard_keys = keyboard_keys;
    prev_keyboard_qualifiers = keyboard_qualifiers;
}


void InputState::Update(Urho3D::Input* input)
{
    Update();
    SetMouseState(input);
    SetQualifiers(input->GetQualifiers());
}


void InputState::SetQualifiers(int qualifiers)
{
    keyboard_qualifiers = qualifiers;
}


void InputState::SetKey(unsigned key, bool down)
{
    if (down)
        keyboard_keys |= key;
    else
        keyboard_keys &= ~key;
}


bool InputState::IsKeyDown(unsigned key) const
{
    return (keyboard_keys & key) != 0;
}


bool InputState::IsKeyJustDown(unsigned key) const
{
    return (keyboard_keys & key) != 0 && (prev_keyboard_keys & key) == 0;
}


void InputState::SetMouseState(Urho3D::Input* input)
{
    mouse_x = input->GetMouseMove().x_;
    mouse_y = input->GetMouseMove().y_;
    mouse_wheel = input->GetMouseMoveWheel();
    SetButton(MM_BUTTON_LEFT, input->GetMouseButtonDown(MM_BUTTON_LEFT));
    SetButton(MM_BUTTON_MIDDLE, input->GetMouseButtonDown(MM_BUTTON_MIDDLE));
    SetButton(MM_BUTTON_RIGHT, input->GetMouseButtonDown(MM_BUTTON_RIGHT));
}


void InputState::SetButton(unsigned button, bool down)
{
    if (down)
        mouse_buttons |= button;
    else
        mouse_buttons &= ~button;
}


bool InputState::IsButtonDown(unsigned button) const
{
    return (mouse_buttons & button) != 0;
}


bool InputState::IsButtonJustDown(unsigned button) const
{
    return (mouse_buttons & button) != 0 && (prev_mouse_buttons & button) == 0;
}


void InputState::Reset()
{
    mouse_buttons = 0;
    mouse_x = 0;
    mouse_y = 0;
    mouse_wheel = 0;
    keyboard_keys = 0;
    prev_keyboard_keys = 0;
}

}
