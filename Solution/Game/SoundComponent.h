#pragma once

#include "Component.h"

class SoundComponent : public Component
{
public:
	SoundComponent(Entity& aEntity);
	~SoundComponent();

	int GetAudioSFXID();

	void Update(float aDeltaTime) override;

	void ReceiveNote(const SoundNote& aNote) override;

	static eComponentType GetType();
private:
	int myAudioSFXID;
};


inline int SoundComponent::GetAudioSFXID()
{
	return myAudioSFXID;
}

inline eComponentType SoundComponent::GetType()
{
	return eComponentType::SOUND;
}

