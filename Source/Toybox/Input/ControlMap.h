#pragma once

#include "../Core/IncGuards.h"
#include <map>

THIRD_PARTY_GUARDS_BEGIN
#include <Urho3D/Input/Input.h>
THIRD_PARTY_GUARDS_END


namespace Toybox
{


class InputControl
{
public:
    InputControl(int qualifiers, int key, int button = 0, bool allow_modifiers = false);

    void Set(int qualifiers, int key, int button);

    bool Test(int qualifiers, int key, int button);

    bool Test(Urho3D::Input* input);

private:
    bool allow_modifiers_;
    int qualifiers_;
    int key_;
    int button_;
};


class ControlMap
{
public:
	static void Set(Urho3D::String ctrl, int qualifiers, int key, int button, bool allow_modifiers = false);
	static bool Test(Urho3D::String ctrl, int qualifiers, int key, int button);
	static bool Test(Urho3D::String ctrl, Urho3D::Input* input);
	static void Validate();
	static void Validate(Urho3D::String ctrl);

private:
	static std::map<Urho3D::String, InputControl*> controls_;
};

}