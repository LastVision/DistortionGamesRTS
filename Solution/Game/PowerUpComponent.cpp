#include "stdafx.h"

#include "CollisionComponent.h"
#include "CollisionNote.h"
#include "CollisionManager.h"
#include "Entity.h"
#include "Enums.h"
#include <Instance.h>
#include "GUINote.h"
#include "GraphicsComponent.h"
#include "PostMaster.h"
#include "PowerUpComponent.h"
#include "PowerUpMessage.h"
#include "PowerUpNote.h"
#include "SoundNote.h"

PowerUpComponent::PowerUpComponent(Entity& aEntity)
	: Component(aEntity)
	, myPlayer(nullptr)
	, myDuration(0)
	, myValue(0)
	, myUpgradeName("")
	, myInGameName("")
{
}

void PowerUpComponent::Init(ePowerUpType someType, const std::string& anInGameName, float someValue, float someDuration)
{
	myType = someType;
	myValue = someValue;
	myDuration = someDuration;
	myInGameName = anInGameName;
	myUpgradePickupMessage = "";
	myUpgradePickupMessageTime = 0.f;
}

void PowerUpComponent::Init(ePowerUpType someType, const std::string& anInGameName, const std::string& aUpgradeName
	, const std::string& aPickupMessage, int anUpgradeID, float aMessageTime)
{
	myType = someType;
	myUpgradeID = anUpgradeID;
	myInGameName = anInGameName;
	myUpgradeName = aUpgradeName;
	myUpgradePickupMessage = aPickupMessage;
	myUpgradePickupMessageTime = aMessageTime;
}

void PowerUpComponent::ReceiveNote(const CollisionNote& aNote)
{
	if (myType == ePowerUpType::WEAPON_UPGRADE)
	{
		PostMaster::GetInstance()->SendMessage(PowerUpMessage(myType, myUpgradeName
			, myUpgradePickupMessage, myUpgradeID, myUpgradePickupMessageTime));
	}
	else
	{
		PowerUpNote note(myType, myInGameName, myValue, myDuration);
		aNote.myEntity.SendNote(note);
	}

	switch (myType)
	{
	case ePowerUpType::FIRERATEBOOST:
		aNote.myEntity.SendNote<SoundNote>(SoundNote(eSoundNoteType::PLAY, "Play_FirerateUp"));
		break;
	case ePowerUpType::SHIELDBOOST:
		aNote.myEntity.SendNote<SoundNote>(SoundNote(eSoundNoteType::PLAY, "Play_ShieldBoost"));
		break;
	case ePowerUpType::HEALTHKIT:
		aNote.myEntity.SendNote<SoundNote>(SoundNote(eSoundNoteType::PLAY, "Play_HealthKit"));
		break;
	case ePowerUpType::WEAPON_UPGRADE:
		aNote.myEntity.SendNote<SoundNote>(SoundNote(eSoundNoteType::PLAY, "Play_WeaponUpgrade"));
		break;
	case ePowerUpType::EMP:
		aNote.myEntity.SendNote<SoundNote>(SoundNote(eSoundNoteType::PLAY, "Play_EMPAcquired"));
		break;
	case ePowerUpType::HOMING:
		aNote.myEntity.SendNote<SoundNote>(SoundNote(eSoundNoteType::PLAY, "Play_WeaponHoming"));
		break;
	case ePowerUpType::INVULNERABLITY:
		aNote.myEntity.SendNote<SoundNote>(SoundNote(eSoundNoteType::PLAY, "Play_Invulnerability"));
		break;
	default:
		DL_ASSERT("UNKNOWN PowerUpType");
		//aNote.myEntity.SendNote<SoundNote>(SoundNote(eSoundNoteType::PLAY, "Play_PowerUpTaken"));
		break;
	}
	aNote.myCollisionManager.Remove(myEntity.GetComponent<CollisionComponent>(), myEntity.GetType());
	myEntity.Kill();
}

void PowerUpComponent::Update(float aDeltaTime)
{
	myPlayer->SendNote<GUINote>(GUINote(&myEntity, eGUINoteType::POWERUP));

	CU::Vector3f pos = myEntity.myOrientation.GetPos();
	myEntity.myOrientation *= CU::Matrix44f::CreateRotateAroundY(1.f* aDeltaTime);
	myEntity.myOrientation.SetPos(pos);
}

void PowerUpComponent::SetPlayer(Entity* aPlayer)
{
	myPlayer = aPlayer;
}
