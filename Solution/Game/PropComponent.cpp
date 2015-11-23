#include "stdafx.h"
#include "CollisionNote.h"
#include "DefendMessage.h"
#include "Entity.h"
#include "HealthComponent.h"
#include "KillStructureMessage.h"
#include "PropComponent.h"
#include "ShieldComponent.h"
#include "PhysicsComponent.h"
#include "PostMaster.h"

PropComponent::PropComponent(Entity& aEntity)
	: Component(aEntity)
{
}


PropComponent::~PropComponent()
{
	if (myDefendName != "")
	{
		PostMaster::GetInstance()->UnSubscribe(eMessageType::DEFEND, this);
	}
	if (myStructureName != "")
	{
		PostMaster::GetInstance()->UnSubscribe(eMessageType::KILL_STRUCTURE, this);
	}
}

void PropComponent::Init(const std::string& aDefendName, const std::string& aStructureName)
{
	myDefendName = aDefendName;
	if (myDefendName != "")
	{
		PostMaster::GetInstance()->Subscribe(eMessageType::DEFEND, this);
	}
	myStructureName = aStructureName;
	if (myStructureName != "")
	{
		PostMaster::GetInstance()->Subscribe(eMessageType::KILL_STRUCTURE, this);
	}
}

void PropComponent::ReceiveNote(const CollisionNote& aNote)
{
	if (aNote.myEntity.GetAlive() == true)
	{
		PhysicsComponent* physicsComponent = aNote.myEntity.GetComponent<PhysicsComponent>();
		if (physicsComponent != nullptr && physicsComponent->GetSpeed() > 0)
		{
			if (aNote.myEntity.GetComponent<ShieldComponent>() != nullptr)
			{
				COMPONENT_LOG("Shield component found on entity. (Prop component)");
				aNote.myEntity.GetComponent<ShieldComponent>()->DamageShield(10);
			}

			if (aNote.myEntity.GetComponent<ShieldComponent>() == nullptr ||
				aNote.myEntity.GetComponent<ShieldComponent>()->GetCurrentShieldStrength() <= 0)
			{
				COMPONENT_LOG("No shield component found on entity or shield were depleted.");
				aNote.myEntity.GetComponent<HealthComponent>()->RemoveHealth(10);
			}
		}
	}

	if (aNote.myEntity.GetComponent<PhysicsComponent>() == nullptr)
	{
		DL_ASSERT(aNote.myEntity.GetName() + "Collision with entity without PhysicsComponent.");
	}
	else
	{
		aNote.myEntity.GetComponent<PhysicsComponent>()->BounceOff(myEntity);
	}
}

void PropComponent::ReceiveMessage(const DefendMessage& aMessage)
{
	if (aMessage.myType == DefendMessage::eType::NAME && aMessage.myDefendName == myDefendName)
	{
		PostMaster::GetInstance()->SendMessage<DefendMessage>(DefendMessage(DefendMessage::eType::ENTITY, myDefendName, &GetEntity()));
	}
}

void PropComponent::ReceiveMessage(const KillStructureMessage& aMessage)
{
	if (aMessage.myType == KillStructureMessage::eType::NAME && aMessage.myStructureName == myStructureName)
	{
		PostMaster::GetInstance()->SendMessage<KillStructureMessage>(KillStructureMessage(KillStructureMessage::eType::ENTITY, myStructureName, &GetEntity()));
	}
}
