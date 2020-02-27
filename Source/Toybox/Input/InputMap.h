#pragma once

#include "../Core/IncGuards.h"
#include <map>

THIRD_PARTY_GUARDS_BEGIN
#include <Urho3D/Input/Input.h>
THIRD_PARTY_GUARDS_END


namespace Toybox
{


class InputBinding
{
public:
    InputBinding(Urho3D::QualifierFlags qualifiers, Urho3D::Key key, Urho3D::MouseButtonFlags button = Urho3D::MOUSEB_NONE, bool ignore_extra_qualifiers = false);

    void Set(Urho3D::QualifierFlags qualifiers, Urho3D::Key key, Urho3D::MouseButtonFlags button = Urho3D::MOUSEB_NONE, bool ignore_extra_qualifiers = false);

    bool Test(Urho3D::QualifierFlags qualifiers, Urho3D::Key key, Urho3D::MouseButtonFlags button);

    bool Test(Urho3D::Input* input);

private:
    bool ignore_extra_qualifiers_;
    //int qualifiers_;
    Urho3D::QualifierFlags qualifiers_;
    Urho3D::Key key_;
    Urho3D::MouseButtonFlags button_;
    //int key_;
    //int button_;
};


class InputMap
{
public:
	static void Set(Urho3D::String ctrl, Urho3D::QualifierFlags qualifiers, Urho3D::Key key, Urho3D::MouseButtonFlags button = Urho3D::MOUSEB_NONE, bool ignore_extra_qualifiers = false);
	static bool Test(Urho3D::String ctrl, Urho3D::QualifierFlags qualifiers, Urho3D::Key key, Urho3D::MouseButtonFlags button = Urho3D::MOUSEB_NONE);
	static bool Test(Urho3D::String ctrl, Urho3D::Input* input);
	static void Validate();
	static void Validate(Urho3D::String ctrl);

private:
	static std::map<Urho3D::String, InputBinding*> controls_;
};

}
