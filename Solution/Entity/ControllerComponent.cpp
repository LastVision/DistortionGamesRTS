#include "stdafx.h"

#include <AudioInterface.h>
#include "ActorComponent.h"
#include "AnimationComponent.h"
#include "BehaviorNote.h"
#include "ControllerComponent.h"
#include "GrenadeComponent.h"
#include "HealthComponent.h"
#include <KillUnitMessage.h>
#include <PathFinderAStar.h>
#include <PathFinderFunnel.h>
#include "PollingStation.h"
#include "PromotionComponent.h"
#include <PostMaster.h>
#include "SoundComponent.h"
#include <Terrain.h>



ControllerComponent::ControllerComponent(Entity& aEntity, ControllerComponentData& aData, const Prism::Terrain& aTerrain)
	: Component(aEntity)
	, myTerrain(aTerrain)
	, myCommands(16)
{
}


ControllerComponent::~ControllerComponent()
{
}

void ControllerComponent::Reset()
{
	myTerrain.CalcEntityHeight(myEntity.myOrientation);

	myStartNewAction = true;


	myCommands.RemoveAll();
	myCommands.Add(EntityCommandData(eEntityCommand::STOP, nullptr, myEntity.GetPosition()));
}

void ControllerComponent::ReceiveNote(const BehaviorNote& aNote)
{
	if (aNote.myFinished == true)
	{
		myStartNewAction = true;
	}
}

void ControllerComponent::Update(float aDelta)
{
	if (myEntity.GetState() == eEntityState::DIE)
	{
		return;
	}

	if (myStartNewAction == true)
	{
		myStartNewAction = false;
		StartNextAction();
	}

	RenderDebugLines();
}

void ControllerComponent::MoveTo(const CU::Vector3<float>& aPosition, bool aClearCommandQueue, bool& aHasPlayedSound)
{
	if (myEntity.GetOwner() == eOwnerType::PLAYER)
	{
		if (aHasPlayedSound == false)
		{
			if (myEntity.GetUnitType() == eUnitType::GRUNT)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Grunt_Move"
					, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
			}
			else if (myEntity.GetUnitType() == eUnitType::RANGER)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Ranger_Move"
					, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
			}
			else if (myEntity.GetUnitType() == eUnitType::TANK)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Tank_Move"
					, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
			}
			aHasPlayedSound = true;
		}
	}
	FillCommandList(eEntityCommand::MOVE, aClearCommandQueue, nullptr, { aPosition.x, aPosition.z });
}

void ControllerComponent::AttackTarget(Entity* aEntity, bool aClearCommandQueue, bool& aHasPlayedSound)
{
	if (myEntity.GetOwner() == eOwnerType::PLAYER)
	{
		if (aHasPlayedSound == false)
		{
			if (myEntity.GetUnitType() == eUnitType::GRUNT)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Grunt_Attack"
					, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
			}
			else if (myEntity.GetUnitType() == eUnitType::RANGER)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Ranger_Attack"
					, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
			}
			else if (myEntity.GetUnitType() == eUnitType::TANK)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Tank_Attack"
					, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
			}
			aHasPlayedSound = true;
		}
	}
	FillCommandList(eEntityCommand::ATTACK_TARGET, aClearCommandQueue, aEntity);
}

void ControllerComponent::AttackMove(const CU::Vector3<float>& aPosition, bool aClearCommandQueue, bool& aHasPlayedSound)
{
	if (myEntity.GetOwner() == eOwnerType::PLAYER)
	{
		if (aHasPlayedSound == false)
		{
			if (myEntity.GetUnitType() == eUnitType::GRUNT)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Grunt_AttackMove"
					, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
			}
			else if (myEntity.GetUnitType() == eUnitType::RANGER)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Ranger_AttackMove"
					, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
			}
			else if (myEntity.GetUnitType() == eUnitType::TANK)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Tank_AttackMove"
					, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
			}
			aHasPlayedSound = true;
		}
	}
	FillCommandList(eEntityCommand::ATTACK_MOVE, aClearCommandQueue, nullptr, { aPosition.x, aPosition.z });
}

void ControllerComponent::Stop(bool& aHasPlayedSound)
{
	if (myEntity.GetOwner() == eOwnerType::PLAYER)
	{
		if (aHasPlayedSound == false)
		{
			if (myEntity.GetUnitType() == eUnitType::GRUNT)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Grunt_Stop"
					, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
			}
			else if (myEntity.GetUnitType() == eUnitType::RANGER)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Ranger_Stop"
					, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
			}
			else if (myEntity.GetUnitType() == eUnitType::TANK)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Tank_Stop"
					, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
			}
			aHasPlayedSound = true;
		}
	}

	myCommands.RemoveAll();
	myStartNewAction = true;
	myCommands.Add(EntityCommandData(eEntityCommand::STOP, nullptr, myEntity.myPosition));
}

void ControllerComponent::HoldPosition(bool& aHasPlayedSound)
{
	if (myEntity.GetOwner() == eOwnerType::PLAYER)
	{
		if (aHasPlayedSound == false)
		{
			if (myEntity.GetUnitType() == eUnitType::GRUNT)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Grunt_Hold"
					, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
			}
			else if (myEntity.GetUnitType() == eUnitType::RANGER)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Ranger_Hold"
					, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
			}
			else if (myEntity.GetUnitType() == eUnitType::TANK)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Tank_Hold"
					, myEntity.GetComponent<SoundComponent>()->GetAudioSFXID());
			}
			aHasPlayedSound = true;
		}
	}

	myCommands.RemoveAll();
	myStartNewAction = true;
	myCommands.Add(EntityCommandData(eEntityCommand::HOLD_POSITION, nullptr, myEntity.myPosition));
}

void ControllerComponent::FillCommandList(eEntityCommand aAction, bool aClearCommandQueue, Entity* aEntity
	, const CU::Vector2<float>& aTargetPosition)
{
	if (aClearCommandQueue == true)
	{
		myCommands.RemoveAll();
		myStartNewAction = true;
	}

	if (myCommands.Size() < 1)
	{
		CU::Vector2<float> position(aTargetPosition);
		if (aEntity != nullptr)
		{
			position = aEntity->GetPosition();
		}
		myCommands.Add(EntityCommandData(eEntityCommand::STOP, nullptr, position));
	}

	EntityCommandData action;
	action.myCommand = aAction;
	action.myEntity = nullptr;

	CU::Vector3<float> targetPosition(aTargetPosition.x, 0, aTargetPosition.y);

	if (aEntity != nullptr)
	{
		targetPosition = aEntity->GetOrientation().GetPos();
	}

	CU::GrowingArray<CU::Vector3<float>> path(16);
	if (myTerrain.GetPathFinder()->FindPath(myEntity.GetOrientation().GetPos(), targetPosition, path) == true)
	{
		if (path.Size() > 0)
		{
			for (int i = 0; i < path.Size(); ++i)
			{
				CU::Vector2<float> target = { path[i].x, path[i].z };

				action.myPosition = target;

				if (myCommands.Size() > 1)
				{
					if (i == path.Size() - 1)
					{
						action.myEntity = aEntity;
					}
					myCommands.Insert(1, action);
				}
				else
				{
					if (i == path.Size() - 1)
					{
						action.myEntity = aEntity;
					}
					myCommands.Add(action);
				}
			}
		}
		else
		{
			action.myEntity = aEntity;
			action.myPosition = aTargetPosition;

			if (myCommands.Size() > 1)
			{
				myCommands.Insert(1, action);
			}
			else
			{
				myCommands.Add(action);
			}
		}
	}

	if (aClearCommandQueue == false)
	{
		if (myCommands.Size() > 0)
		{
			CU::Vector2<float> position(aTargetPosition);
			if (aEntity != nullptr)
			{
				position = aEntity->GetPosition();
			}
			myCommands[0] = EntityCommandData(eEntityCommand::STOP, nullptr, position);
		}
	}
}

void ControllerComponent::StartNextAction()
{
	if (myCommands.Size() > 0)
	{
		myReturnPosition = myCommands.GetLast().GetPosition();
		myCurrentCommand = myCommands.GetLast();
		myCommands.RemoveCyclicAtIndex(myCommands.Size() - 1);
	}
	else
	{
		myCurrentCommand = EntityCommandData(eEntityCommand::STOP, nullptr, myReturnPosition);
	}
}

// TODO: Var ska detta ligga?
//void ControllerComponent::RefreshPathToAttackTarget()
//{
//	bool shouldBeQuiet = true;
//	AttackTarget(myCurrentCommand.myEntity, true, shouldBeQuiet);
//	myCurrentAttackTargetPathRefreshTime = myAttackTargetPathRefreshTime;
//}

void ControllerComponent::RenderDebugLines() const
{
	if (myCurrentCommand.myCommand != eEntityCommand::STOP)
	{
		CU::Vector3<float> pointA = myTerrain.GetHeight(myEntity.myOrientation.GetPos(), 1.f);
		CU::Vector3<float> pointB = myTerrain.GetHeight(myCurrentCommand.GetPosition(), 1.f);

		Prism::RenderLine3D(pointA, pointB, GetActionColor(myCurrentCommand.myCommand));
	}

	if (myCommands.Size() > 0)
	{
		CU::Vector3<float> pointA = myTerrain.GetHeight(myEntity.myOrientation.GetPos(), 1.f);
		if (myCurrentCommand.myEntity != nullptr || myCurrentCommand.myPosition != CU::Vector2<float>(-1, -1))
		{
			CU::Vector3<float> pointA = myTerrain.GetHeight(myCurrentCommand.GetPosition(), 1.f);
		}

		CU::Vector3<float> pointB = myTerrain.GetHeight(myCommands.GetLast().GetPosition(), 1.f);
		eColorDebug color = GetActionColor(myCommands.GetLast().myCommand);

		Prism::RenderLine3D(pointA, pointB, color);

		for (int i = myCommands.Size() - 1; i >= 1; --i)
		{
			color = GetActionColor(myCommands[i - 1].myCommand);


			pointA = myTerrain.GetHeight(myCommands[i].GetPosition(), 1.f);
			pointB = myTerrain.GetHeight(myCommands[i - 1].GetPosition(), 1.f);
			Prism::RenderLine3D(pointA, pointB, color);
		}
	}
}

eColorDebug ControllerComponent::GetActionColor(eEntityCommand aAction) const
{
	switch (aAction)
	{
	case eEntityCommand::STOP:
		return eColorDebug::WHITE;
		break;
	case eEntityCommand::MOVE:
		return eColorDebug::GREEN;
		break;
	case eEntityCommand::ATTACK_TARGET:
		return eColorDebug::RED;
		break;
	case eEntityCommand::ATTACK_MOVE:
		return eColorDebug::RED;
		break;
	case eEntityCommand::HOLD_POSITION:
		return eColorDebug::YELLOW;
		break;
	default:
		DL_ASSERT("Tried to GetActionColor of a invalid action");
		break;
	}

	return eColorDebug::NOT_USED;
}