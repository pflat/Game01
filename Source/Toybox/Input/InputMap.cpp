#include "InputMap.h"


namespace Toybox
{

std::map<Urho3D::String, InputBinding*> InputMap::controls_;


InputBinding::InputBinding(Urho3D::QualifierFlags qualifiers, Urho3D::Key key, Urho3D::MouseButtonFlags button, bool ignore_extra_qualifiers) :
    ignore_extra_qualifiers_(ignore_extra_qualifiers),
        qualifiers_(qualifiers),
        key_(key),
        button_(button)
{
//	if (button == Urho3D::MOUSEB_NONE)
//		button_ = 0x8000;
//    if (qualifiers > 0)
//        allow_modifiers = false;
//    if (qualifiers != Urho3D::QUAL_NONE)
//        allow_modifiers = false;
}


void InputBinding::Set(Urho3D::QualifierFlags qualifiers, Urho3D::Key key, Urho3D::MouseButtonFlags button, bool ignore_extra_qualifiers)
{
    qualifiers_ = qualifiers;
    key_ = key;
    button_ = button;
//    if (qualifiers != Urho3D::QUAL_NONE)
//        allow_modifiers_ = false;
//    else
//        allow_modifiers_ = allow_modifiers;
}


bool InputBinding::Test(Urho3D::QualifierFlags qualifiers, Urho3D::Key key, Urho3D::MouseButtonFlags button)
{
    if (ignore_extra_qualifiers_)
    {
        //  Test for qualifiers only binding
        if (qualifiers_ != Urho3D::QUAL_NONE && ((qualifiers_ & qualifiers) == qualifiers_) &&
            (key_ == Urho3D::KEY_UNKNOWN && button_ == Urho3D::MOUSEB_NONE))
        {
            return true;
        }
        //  Test for (qualifier + key) and (qualifier + mouse button)
        else if (((qualifiers_ & qualifiers) == qualifiers_ && key_ == key && key_ != Urho3D::KEY_UNKNOWN) ||
            ((qualifiers_ & qualifiers) == qualifiers_ && button_ == button && button_ != Urho3D::MOUSEB_NONE))
        {
            return true;
        }
    }
    else
    {
        //  Test for qualifiers only binding
        if (qualifiers_ != Urho3D::QUAL_NONE && qualifiers_ == qualifiers &&
            (key_ == Urho3D::KEY_UNKNOWN && button_ == Urho3D::MOUSEB_NONE))
        {
            return true;
        }
        //  Test for (qualifier + key) and (qualifier + mouse button)
        else if ((qualifiers_ == qualifiers && key_ == key && key_ != Urho3D::KEY_UNKNOWN) ||
            (qualifiers_ == qualifiers && button_ == button && button_ != Urho3D::MOUSEB_NONE))
        {
            return true;
        }
    }
    return false;
}


bool InputBinding::Test(Urho3D::Input* input)
{
    Urho3D::QualifierFlags qualifiers = input->GetQualifiers();
    bool key_down = input->GetKeyDown(key_);
    bool button_down = input->GetMouseButtonDown(button_);

    if (ignore_extra_qualifiers_)
    {
        //  Test for qualifiers only binding
        if (qualifiers_ != Urho3D::QUAL_NONE && (qualifiers_ & qualifiers) == qualifiers_ &&
            (key_ == Urho3D::KEY_UNKNOWN && button_ == Urho3D::MOUSEB_NONE))
        {
            return true;
        }
        //  Test for (qualifier + key) and (qualifier + mouse button)
        else if (((qualifiers_ & qualifiers) == qualifiers_ && key_down && key_ != Urho3D::KEY_UNKNOWN) ||
            ((qualifiers_ & qualifiers) == qualifiers_ && button_down && button_ != Urho3D::MOUSEB_NONE))
        {
            return true;
        }
    }
    else
    {
        //  Test for qualifiers only binding
        if (qualifiers_ != Urho3D::QUAL_NONE && qualifiers_ == qualifiers &&
            (key_ == Urho3D::KEY_UNKNOWN && button_ == Urho3D::MOUSEB_NONE))
        {
            return true;
        }
        //  Test for (qualifier + key) and (qualifier + mouse button)
        else if ((qualifiers_ == qualifiers && key_down && key_ != Urho3D::KEY_UNKNOWN) || 
            (qualifiers_ == qualifiers && button_down && button_ != Urho3D::MOUSEB_NONE))
        {
            return true;
        }
    }
    return false;
}


void InputMap::Set(Urho3D::String ctrl, Urho3D::QualifierFlags qualifiers, Urho3D::Key key, Urho3D::MouseButtonFlags button, bool ignore_extra_qualifiers)
{
    //  Insert
	controls_[ctrl] = new InputBinding(qualifiers, key, button, ignore_extra_qualifiers);
	//  ToDo Replace
}


bool InputMap::Test(Urho3D::String ctrl, Urho3D::QualifierFlags qualifiers, Urho3D::Key key, Urho3D::MouseButtonFlags button)
{
	if (controls_[ctrl] > 0)
		return controls_[ctrl]->Test(qualifiers, key, button);
	else
		return false;
}


bool InputMap::Test(Urho3D::String ctrl, Urho3D::Input* input)
{
	if (controls_[ctrl] > 0)
		return controls_[ctrl]->Test(input);
	else
		return false;
}


void InputMap::Validate()
{
	///   ToDo
}


void InputMap::Validate(Urho3D::String key)
{
	///   ToDo
}

}
