#include "stdafx.h"
#include <AudioInterface.h>
#include "Entity.h"
#include "SoundComponent.h"
#include "SoundNote.h"


SoundComponent::SoundComponent(Entity& aEntity)
	:Component(aEntity)
	, myAudioSFXID(-1)
{
	myAudioSFXID = Prism::Audio::AudioInterface::GetInstance()->GetUniqueID();

	if (myEntity.GetType() == eEntityType::PLAYER)
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_PlayerThrusters", myAudioSFXID);
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_PlayerThrusterBoost", myAudioSFXID);
	}
	else if (myEntity.GetType() == eEntityType::ENEMY)
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_EnemyThruster", myAudioSFXID);
	}
}


SoundComponent::~SoundComponent()
{
	if (myEntity.GetType() == eEntityType::PLAYER)
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Stop_PlayerThrusters", myAudioSFXID);
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Stop_PlayerThrusterBoost", myAudioSFXID);
	}																 
	else if (myEntity.GetType() == eEntityType::ENEMY)
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Stop_EnemyThruster", myAudioSFXID);
	}

	Prism::Audio::AudioInterface::GetInstance()->UnRegisterObject(myAudioSFXID);
}

void SoundComponent::Update(float)
{
	if (myEntity.GetType() == eEntityType::PLAYER)
	{
		Prism::Audio::AudioInterface::GetInstance()->SetListenerPosition(myEntity.myOrientation.GetPos().x
			, myEntity.myOrientation.GetPos().y, myEntity.myOrientation.GetPos().z);
	}
	else
	{
		Prism::Audio::AudioInterface::GetInstance()->SetPosition(myEntity.myOrientation.GetPos().x
			, myEntity.myOrientation.GetPos().y, myEntity.myOrientation.GetPos().z, myAudioSFXID);
	}
}

void SoundComponent::ReceiveNote(const SoundNote& aNote)
{
	if (aNote.myType == eSoundNoteType::PLAY)
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent(aNote.myEventName.c_str(), myAudioSFXID);
	}
	else if (aNote.myType == eSoundNoteType::STOP)
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent(aNote.myEventName.c_str(), myAudioSFXID);
	}
}
