#include "stdafx.h"
#include <AudioInterface.h>
#include "BulletCollisionToGUIMessage.h"
#include "BulletComponent.h"
#include "CollisionComponent.h"
#include "CollisionManager.h"
#include "CollisionNote.h"
#include "Entity.h"
#include "EmitterNote.h"
#include "HealthComponent.h"
#include "LevelScoreMessage.h"
#include "PostMaster.h"
#include "ShieldComponent.h"
#include "SoundNote.h"
#include "SpawnExplosionMessage.h"

BulletComponent::BulletComponent(Entity& aEntity)
	: Component(aEntity)
{
	myCurrentLifeTime = 0.f;
	myDamageRadius = 0.f;
	SetActive(false);
}

void BulletComponent::Update(float aDeltaTime)
{
	if (myCurrentLifeTime >= myMaxLifeTime)
	{
		SetActive(false);
		myCurrentLifeTime = 0.f;
	}
	myCurrentLifeTime += aDeltaTime;
}

void BulletComponent::Init(float aMaxTime, int aDamage, float aDamageRadius, eBulletType aType)
{
	myType = aType;
	SetActive(false);
	myMaxLifeTime = aMaxTime;
	myDamage = aDamage;
	myDamageRadius = aDamageRadius;
	DL_ASSERT_EXP(myDamage >= 0, "Can't have negative damage.");
}

void BulletComponent::SetOwner(eEntityType aOwner)
{
	myOwner = aOwner;
}

void BulletComponent::ReceiveNote(const CollisionNote& aNote)
{
	if (aNote.myEntity.GetAlive() == true)
	{
		if (aNote.myEntity.GetComponent<ShieldComponent>() != nullptr)
		{
			COMPONENT_LOG("Shield component found on entity. (BulletComponent)");
			aNote.myEntity.GetComponent<ShieldComponent>()->DamageShield(myDamage);
		}

		if (aNote.myEntity.GetComponent<ShieldComponent>() == nullptr ||
			aNote.myEntity.GetComponent<ShieldComponent>()->GetCurrentShieldStrength() <= 0)
		{
			COMPONENT_LOG("No shield component found on entity or shield were depleted.");
			if (aNote.myEntity.GetComponent<HealthComponent>() == nullptr)
			{
				DL_ASSERT(aNote.myEntity.GetName() + ": Entity without healthcomponent has been shot.");
			}
			else
			{
				aNote.myEntity.GetComponent<HealthComponent>()->RemoveHealth(myDamage);
			}
		}

		SetActive(false);
		aNote.myCollisionManager.Remove(myEntity.GetComponent<CollisionComponent>(), myEntity.GetType());

		if (myDamageRadius > 0.f)
		{
			aNote.myCollisionManager.DamageEnemiesWithinSphere(myEntity.myOrientation.GetPos(), myDamageRadius, myDamage);
		}

		if (myEntity.GetType() == eEntityType::PLAYER_BULLET && (aNote.myEntity.GetType() == eEntityType::ENEMY || aNote.myEntity.GetType() == eEntityType::STRUCTURE))
		{
			PostMaster::GetInstance()->SendMessage<LevelScoreMessage>(LevelScoreMessage(eLevelScoreMessageType::PLAYER_HIT_ENEMY));
		}

		
		PostMaster::GetInstance()->SendMessage<BulletCollisionToGUIMessage>(BulletCollisionToGUIMessage(this->GetEntity(), aNote.myEntity, myOwner));
	}


	if ((myType == eBulletType::ROCKET_MISSILE_LEVEL_1) ||
		(myType == eBulletType::ROCKET_MISSILE_LEVEL_2) ||
		(myType == eBulletType::ROCKET_MISSILE_LEVEL_3))
	{
		PostMaster::GetInstance()->SendMessage(SpawnExplosionMessage(eMessageType::SPAWN_EXPLOSION_ON_ROCKET_DEATH
			, myEntity.myOrientation.GetPos()));
	}
	else if (aNote.myEntity.GetType() != eEntityType::PLAYER)
	{
		if (aNote.myEntity.GetName().find("asteroid") != std::string::npos || aNote.myEntity.GetName().find("Asteroid") != std::string::npos)
		{
			PostMaster::GetInstance()->SendMessage(SpawnExplosionMessage(eMessageType::SPAWN_EFFECT_ON_ASTROID_HIT
				, myEntity.myOrientation.GetPos()));
		}
		else
		{
			PostMaster::GetInstance()->SendMessage(SpawnExplosionMessage(eMessageType::SPAWN_EFFECT_ON_HIT
				, myEntity.myOrientation.GetPos()));
		}
	}
}

void BulletComponent::SetActive(bool aActive)
{
	myActive = aActive;

	if (myActive == true)
	{
		if (myEntity.GetType() == eEntityType::PLAYER_BULLET)
		{
			PostMaster::GetInstance()->SendMessage<LevelScoreMessage>(LevelScoreMessage(eLevelScoreMessageType::PLAYER_SHOT));
			if (myType == eBulletType::ROCKET_MISSILE_LEVEL_1
				|| myType == eBulletType::ROCKET_MISSILE_LEVEL_2
				|| myType == eBulletType::ROCKET_MISSILE_LEVEL_3)
			{
				myEntity.SendNote(EmitterNote(EmitterNote::eType::BULLET, true, true));
				myEntity.SendNote<SoundNote>(SoundNote(eSoundNoteType::PLAY, "Play_Missiles"));
			}
		}

	}
	else
	{
		myEntity.SendNote<SoundNote>(SoundNote(eSoundNoteType::STOP, "Stop_Missiles"));
		if (myType == eBulletType::ROCKET_MISSILE_LEVEL_1
			|| myType == eBulletType::ROCKET_MISSILE_LEVEL_2
			|| myType == eBulletType::ROCKET_MISSILE_LEVEL_3)
		{
			myEntity.SendNote(EmitterNote(EmitterNote::eType::BULLET, true, false));
			myEntity.SendNote<SoundNote>(SoundNote(eSoundNoteType::PLAY, "Play_ShipExplosion"));
		}
	}
}