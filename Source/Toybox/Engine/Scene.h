#pragma once

#include "../Core/IncGuards.h"

THIRD_PARTY_GUARDS_BEGIN
#include <Urho3D/Core/Context.h>
#include <Urho3D/Scene/Component.h>
#include <Urho3D/Audio/Sound.h>
//#include <Urho3D/Scene/Scene.h>
//#include <Urho3D/Scene/Node.h>
//#include <Urho3D/Resource/ResourceCache.h>
THIRD_PARTY_GUARDS_END


namespace Toybox
{

class Scene : public Urho3D::Component
{
    URHO3D_OBJECT(Scene, Component);

public:

	Scene(Urho3D::Context* context);

    static void RegisterObject(Urho3D::Context* context);

    void Init();
    void SetAmbientMusic(Urho3D::Sound* music);
    void SendToBack(bool pause = true);
    void BringToFront();

private:

	bool paused_;
    float music_time_elapsed_;
	Urho3D::SharedPtr<Urho3D::Sound> music_;

};

}
