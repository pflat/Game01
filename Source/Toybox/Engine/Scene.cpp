#include "../Engine/Scene.h"
/*
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Skybox.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Resource/JSONFile.h>
#include <Urho3D/Resource/JSONValue.h>
#include <Urho3D/Audio/Sound.h>
*/
THIRD_PARTY_GUARDS_BEGIN
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Audio/SoundSource.h>
THIRD_PARTY_GUARDS_END


namespace Toybox
{

Scene::Scene(Urho3D::Context* context) :
    Urho3D::Component(context),
    paused_(false),
    music_time_elapsed_(0.0f)
{
}


void Scene::RegisterObject(Urho3D::Context* context)
{
    context->RegisterFactory<Scene>();
}


void Scene::Init()
{
	Urho3D::SoundSource* music_player = node_->CreateComponent<Urho3D::SoundSource>();
    music_player->SetSoundType(Urho3D::SOUND_MUSIC);
    music_player->SetGain(0.75f);

    // Component has been inserted into its scene node. Subscribe to events now
    //SubscribeToEvent(GetNode(), E_NODECOLLISION, URHO3D_HANDLER(Character, HandleNodeCollision));
}


void Scene::SetAmbientMusic(Urho3D::Sound* music)
{
    music_ = music;
    music_->SetLooped(true);
}


void Scene::SendToBack(bool pause)
{
    paused_ = pause;
    if (paused_)
        node_->GetScene()->SetUpdateEnabled(false);

    Urho3D::SoundSource* music_player = node_->GetComponent<Urho3D::SoundSource>();
    if (music_player)
    {
        music_time_elapsed_ = music_player->GetTimePosition();
        music_player->Stop();
    }
}


void Scene::BringToFront()
{
    if (paused_)
        node_->GetScene()->SetUpdateEnabled(true);

    Urho3D::SoundSource* music_player = node_->GetComponent<Urho3D::SoundSource>();
    if (music_player)
    {
        music_player->Play(music_);
        music_player->Seek(music_time_elapsed_);
    }

    paused_ = false;
}

}
