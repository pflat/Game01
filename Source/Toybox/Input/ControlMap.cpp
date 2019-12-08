#include "ControlMap.h"


namespace Toybox
{

std::map<Urho3D::String, InputControl*> ControlMap::controls_;


InputControl::InputControl(int qualifiers, int key, int button, bool allow_modifiers) :
        allow_modifiers_(allow_modifiers),
        qualifiers_(qualifiers),
        key_(key),
        button_(button)
{
	if (button == 0)
		button_ = 0x8000;
}


void InputControl::Set(int qualifiers, int key, int button)
{
    qualifiers_ = qualifiers;
    key_ = key;
    button_ = button;
}


bool InputControl::Test(int qualifiers, int key, int button)
{
    if (allow_modifiers_)
    {
        if (key_ == key || button_ == button)
            return true;
    }
    else if (qualifiers_ > 0 && qualifiers_ == qualifiers)
    {
        if (key_ == Urho3D::KEY_UNKNOWN)
            return true;
        else if (key_ == key || button_ == button)
            return true;
    }
    else if (qualifiers_ == 0 && qualifiers == 0)
    {
        if (key_ == key || button_ == button)
            return true;
    }

    return false;
}


bool InputControl::Test(Urho3D::Input* input)
{
    if (allow_modifiers_)
    {
        if (input->GetKeyDown(key_) || input->GetMouseButtonDown(button_))
            return true;
    }
    else if (qualifiers_ > 0 && qualifiers_ == input->GetQualifiers())
    {
        if (key_ == Urho3D::KEY_UNKNOWN)
            return true;
        else if (input->GetKeyDown(key_) || input->GetMouseButtonDown(button_))
            return true;
    }
    else if (qualifiers_ == 0 && input->GetQualifiers() == 0)
    {
        if (input->GetKeyDown(key_) || input->GetMouseButtonDown(button_))
            return true;
    }

    return false;
}


void ControlMap::Set(Urho3D::String ctrl, int qualifiers, int key, int button, bool allow_modifiers)
{
	controls_[ctrl] = new InputControl(qualifiers, key, button, allow_modifiers);
}


bool ControlMap::Test(Urho3D::String ctrl, int qualifiers, int key, int button)
{
	if (controls_[ctrl] > 0)
		return controls_[ctrl]->Test(qualifiers, key, button);
	else
		return false;
}


bool ControlMap::Test(Urho3D::String ctrl, Urho3D::Input* input)
{
	if (controls_[ctrl] > 0)
		return controls_[ctrl]->Test(input);
	else
		return false;
}


void ControlMap::Validate()
{
	///   ToDo
}


void ControlMap::Validate(Urho3D::String key)
{
	///   ToDo
}

}