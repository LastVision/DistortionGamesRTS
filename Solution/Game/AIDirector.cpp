#include "stdafx.h"
#include "AIDirector.h"
#include <BuildingComponent.h>
#include <ControllerComponent.h>
#include <Entity.h>
#include <EntityFactory.h>
#include <GameConstants.h>
#include <PollingStation.h>
#include <PostMaster.h>
#include <SpawnUnitMessage.h>
#include <TimeMultiplierMessage.h>
#include <InputWrapper.h>

#include "InfluenceMap.h"
#include "TensionMap.h"
#include "DifferenceMap.h"
#include "VulnerabilityMap.h"
#include "DecisionMap.h"

#include <FuzzySet.h>
#include <TriggerComponent.h>
#include <ActorComponent.h>
#include <BlockMapMessage.h>

AIDirector::AIMaps::AIMaps()
{
	myInfluenceMap = new InfluenceMap();
	myPlayerInfluenceMap = new InfluenceMap();
	myNeutralInfluenceMap = new InfluenceMap();
	myPlayerNeutralCombinedInfluence = new InfluenceMap();
	myGoalMap = new InfluenceMap();
	myBlockMap = new InfluenceMap();
	myTensionMap = new TensionMap(myInfluenceMap, myPlayerInfluenceMap, myNeutralInfluenceMap
		, myGoalMap);
	myDifferenceMap = new DifferenceMap(myInfluenceMap, myPlayerNeutralCombinedInfluence);
	myVulnerabilityMap = new VulnerabilityMap(myDifferenceMap, myTensionMap);

	myDecisionMap = new DecisionMap();
	myDecisionMap->SetVulnerabilityMap(myVulnerabilityMap);
	myDecisionMap->SetDifferenceMap(myDifferenceMap);

	myInfluenceRenderIndex = 0;
}

AIDirector::AIMaps::~AIMaps()
{
	SAFE_DELETE(myInfluenceMap);
	SAFE_DELETE(myPlayerInfluenceMap);
	SAFE_DELETE(myNeutralInfluenceMap);
	SAFE_DELETE(myPlayerNeutralCombinedInfluence);
	SAFE_DELETE(myGoalMap);
	SAFE_DELETE(myBlockMap);
	SAFE_DELETE(myTensionMap);
	SAFE_DELETE(myDifferenceMap);
	SAFE_DELETE(myVulnerabilityMap);
	SAFE_DELETE(myDecisionMap);
}

AIDirector::AIDirector(const Prism::Terrain& aTerrain, Prism::Scene& aScene)
	: Director(eOwnerType::ENEMY, aTerrain)
	, myPlayerHasStarted(false)
	, myIdleUnits(32)
	, myUnitsOnMission(32)
	, myActionQueue(8)
{
	for (int i = 0; i < GC::enemyGruntCount; ++i)
	{
		myUnits.Add(EntityFactory::CreateEntity(eOwnerType::ENEMY, eEntityType::UNIT, "grunt", Prism::eOctreeType::DYNAMIC,
			aScene, { 20.f + i, 0.f, 40.f }, aTerrain));
	}

	for (int i = 0; i < GC::enemyRangerCount; ++i)
	{
		myUnits.Add(EntityFactory::CreateEntity(eOwnerType::ENEMY, eEntityType::UNIT, "ranger", Prism::eOctreeType::DYNAMIC,
			aScene, { 20.f + i, 0.f, 40.f }, aTerrain));
	}

	for (int i = 0; i < GC::enemyTankCount; ++i)
	{
		myUnits.Add(EntityFactory::CreateEntity(eOwnerType::ENEMY, eEntityType::UNIT, "tank", Prism::eOctreeType::DYNAMIC,
			aScene, { 20.f + i, 0.f, 40.f }, aTerrain));
	}

	
	PostMaster::GetInstance()->Subscribe(eMessageType::TIME_MULTIPLIER, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::BLOCK_MAP, this);

	myFuzzySet = new CU::FuzzySet(static_cast<int>(eFuzzyAI::_COUNT));

	myCurrentAction.myFuzzyAction = eFuzzyAI::FIRST_ACTION;
	myCurrentAction.myIsDone = true;
}

AIDirector::~AIDirector()
{
	SAFE_DELETE(myFuzzySet);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::TIME_MULTIPLIER, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::BLOCK_MAP, this);
}

void AIDirector::RenderMaps(const Prism::Camera& aCamera)
{
	switch (myMaps.myInfluenceRenderIndex)
	{
	case 0:
		break;
	case 1:
		myMaps.myInfluenceMap->Render(aCamera);
		break;
	case 2:
		myMaps.myPlayerInfluenceMap->Render(aCamera);
		break;
	case 3:
		myMaps.myNeutralInfluenceMap->Render(aCamera);
		break;
	case 4:
		myMaps.myGoalMap->Render(aCamera);
		break;
	case 5:
		myMaps.myTensionMap->Render(aCamera);
		break;
	case 6:
		myMaps.myDifferenceMap->Render(aCamera);
		break;
	case 7:
		myMaps.myPlayerNeutralCombinedInfluence->Render(aCamera);
		break;
	case 8:
		myMaps.myVulnerabilityMap->Render(aCamera);
		break;
	case 9:
		myMaps.myBlockMap->Render(aCamera);
		break;
	default:
		DL_ASSERT("Invalid myInfluenceRenderIndex");
		break;
	}
}

void AIDirector::Update(float aDeltaTime)
{
	aDeltaTime *= myTimeMultiplier;

	Director::Update(aDeltaTime);

	if (myPlayerHasStarted == false)
	{
		myPlayerHasStarted = PollingStation::GetInstance()->GetUnits(eOwnerType::PLAYER).Size() > 0;
		return;
	}

	UpdateInfluences();
	UpdateUnitLists();


	if (UpdateCurrentAction() == true)
	{
		if (FuzzyDecisionDone() == true)
		{
			UpdateAdvisors();
			InterpretFuzzySet();
		}

		StartNextAction();
	}

	myBuilding->Update(aDeltaTime);
}

void AIDirector::ReceiveMessage(const SpawnUnitMessage& aMessage)
{
	if (static_cast<eUnitType>(aMessage.myUnitType) == eUnitType::RANGER && myHasUnlockedRanger == false) return;
	if (static_cast<eUnitType>(aMessage.myUnitType) == eUnitType::TANK && myHasUnlockedTank == false) return;
	Director::ReceiveMessage(aMessage);
	//if (aMessage.myOwnerType == static_cast<int>(eOwnerType::ENEMY))
	//{
	//	Entity* newEntity = myActiveUnits.GetLast();

	//	//myIdleUnits.Add(newEntity);

	//	bool shouldBeQuiet = true;
	//	CU::Vector2<float> targetPos = myDecisionMap->GetOffensivePosition(newEntity->GetPosition());
	//	CU::Vector3<float> pos(targetPos.x, 0.f, targetPos.y);
	//	newEntity->GetComponent<ControllerComponent>()->AttackMove(pos
	//		, true, shouldBeQuiet);
	//}
	}

void AIDirector::ReceiveMessage(const TimeMultiplierMessage& aMessage)
{
	if (aMessage.myOwner == eOwnerType::ENEMY)
	{
		myTimeMultiplier = aMessage.myMultiplier;
	}
}

void AIDirector::ReceiveMessage(const BlockMapMessage& aMessage)
{
	if (aMessage.myShouldAddValue == true)
	{
		myMaps.myBlockMap->AddValue(1.f, 5.f, aMessage.myPosition);
	}
	else
	{
		myMaps.myBlockMap->RemoveValue(1.f, 5.f, aMessage.myPosition);
	}
}

void AIDirector::UpdateInfluences()
{
	CU::InputWrapper* input = CU::InputWrapper::GetInstance();

	if (input->KeyIsPressed(DIK_LSHIFT) || input->KeyIsPressed(DIK_RSHIFT))
	{
		if (input->KeyDown(DIK_F5))
		{
			myMaps.myInfluenceRenderIndex = 5;
		}
		else if (input->KeyDown(DIK_F6))
		{
			myMaps.myInfluenceRenderIndex = 6;
		}
		else if (input->KeyDown(DIK_F7))
		{
			myMaps.myInfluenceRenderIndex = 7;
		}
		else if (input->KeyDown(DIK_F8))
		{
			myMaps.myInfluenceRenderIndex = 8;
		}
		else if (input->KeyDown(DIK_F9))
		{
			myMaps.myInfluenceRenderIndex = 9;
		}
	}
	else
	{
		if (input->KeyDown(DIK_F5))
		{
			myMaps.myInfluenceRenderIndex = 0;
		}
		else if (input->KeyDown(DIK_F6))
		{
			myMaps.myInfluenceRenderIndex = 1;
		}
		else if (input->KeyDown(DIK_F7))
		{
			myMaps.myInfluenceRenderIndex = 2;
		}
		else if (input->KeyDown(DIK_F8))
		{
			myMaps.myInfluenceRenderIndex = 3;
		}
		else if (input->KeyDown(DIK_F9))
		{
			myMaps.myInfluenceRenderIndex = 4;
		}
	}





	myMaps.myInfluenceMap->Update();
	for (int i = 0; i < myActiveUnits.Size(); ++i)
	{
		myMaps.myInfluenceMap->AddValue(1.f, 30.f, myActiveUnits[i]->GetPosition());
	}

	myMaps.myPlayerInfluenceMap->Update();
	const CU::GrowingArray<Entity*>& playerUnits = PollingStation::GetInstance()->GetUnits(eOwnerType::PLAYER);
	for (int i = 0; i < playerUnits.Size(); ++i)
	{
		myMaps.myPlayerInfluenceMap->AddValue(1.f, 30.f, playerUnits[i]->GetPosition());
	}

	myMaps.myNeutralInfluenceMap->Update();
	const CU::GrowingArray<Entity*>& neutralUnits = PollingStation::GetInstance()->GetUnits(eOwnerType::NEUTRAL);
	for (int i = 0; i < neutralUnits.Size(); ++i)
	{
		myMaps.myNeutralInfluenceMap->AddValue(1.f, 30.f, neutralUnits[i]->GetPosition());
	}

	for (int i = 0; i < myMaps.myPlayerNeutralCombinedInfluence->GetGrid().Size(); ++i)
	{
		myMaps.myPlayerNeutralCombinedInfluence->SetValue(i, myMaps.myPlayerInfluenceMap->GetGrid()[i] + myMaps.myNeutralInfluenceMap->GetGrid()[i]);
	}

	myMaps.myGoalMap->Update();
	const CU::GrowingArray<Entity*>& victoryPoint = PollingStation::GetInstance()->GetVictoryPoints();
	for (int i = 0; i < victoryPoint.Size(); ++i)
	{
		eOwnerType pointOwner = victoryPoint[i]->GetComponent<TriggerComponent>()->GetOwnerGainingPoint();
		float victoryValue = 1.f;
		if (pointOwner == myOwner)
		{
			victoryValue = 0.2f;
		}
		else if (pointOwner == eOwnerType::NEUTRAL)
		{
			victoryValue = 1.f;
		}
		else if (pointOwner == eOwnerType::PLAYER)
		{
			victoryValue = 0.7f;
		}
		myMaps.myGoalMap->AddValue(victoryValue, 30.f, victoryPoint[i]->GetPosition());
	}

	const CU::GrowingArray<Entity*>& resourcePoints = PollingStation::GetInstance()->GetResourcePoints();
	for (int i = 0; i < resourcePoints.Size(); ++i)
	{
		eOwnerType pointOwner = resourcePoints[i]->GetComponent<TriggerComponent>()->GetOwnerGainingPoint();
		float resourceValue = 0.8f;
		if (pointOwner == myOwner)
		{
			resourceValue = 0.1f;
		}
		else if (pointOwner == eOwnerType::NEUTRAL)
		{
			resourceValue = 0.8f;
		}
		else if (pointOwner == eOwnerType::PLAYER)
		{
			resourceValue = 0.35f;
		}
		myMaps.myGoalMap->AddValue(resourceValue, 30.f, resourcePoints[i]->GetPosition());
	}

	const CU::GrowingArray<Entity*>& artifacts = PollingStation::GetInstance()->GetArtifacts();
	for (int i = 0; i < artifacts.Size(); ++i)
	{
		myMaps.myGoalMap->AddValue(0.3f, 5.f, artifacts[i]->GetPosition());
	}

	myMaps.myDifferenceMap->Update();
	myMaps.myTensionMap->Update();
	myMaps.myVulnerabilityMap->Update();
}

void AIDirector::UpdateUnitLists()
{
	myUnitsOnMission.RemoveAll();
	myIdleUnits.RemoveAll();

	for (int i = myActiveUnits.Size()-1; i >= 0; --i)
	{
		bool ready = myActiveUnits[i]->GetComponent<ControllerComponent>()->IsReady();
		if (myActiveUnits[i]->GetState() == eEntityState::IDLE && ready == true)
		{
			myIdleUnits.Add(myActiveUnits[i]);
		}
		else
		{
			myUnitsOnMission.Add(myActiveUnits[i]);
		}
	}
}

void AIDirector::UpdateAdvisors()
{
	myFuzzySet->Reset();

	*myFuzzySet += UpdateAttackAdvisor();
	*myFuzzySet += UpdateDefendAdvisor();
	*myFuzzySet += UpdateResourceAdvisor();

	myFuzzySet->Normalize();
}

CU::FuzzySet AIDirector::UpdateAttackAdvisor()
{
	CU::FuzzySet set(static_cast<int>(eFuzzyAI::_COUNT));

	float gruntCount = 0;
	float rangerCount = 0;
	float tankCount = 0;
	for (int i = 0; i < myActiveUnits.Size(); ++i)
	{
		if (myActiveUnits[i]->GetUnitType() == eUnitType::GRUNT)
		{
			++gruntCount;
		}
		else if (myActiveUnits[i]->GetUnitType() == eUnitType::RANGER)
		{
			++rangerCount;
		}
		else if (myActiveUnits[i]->GetUnitType() == eUnitType::TANK)
		{
			++tankCount;
		}
	}


	float gruntValue = 1.f;
	float rangerValue = 0.75f;
	float tankValue = 0.5f;
	if (myActiveUnits.Size() > 0)
	{
		gruntValue = (1 - (gruntCount / myActiveUnits.Size())) * gruntValue;
		rangerValue = (1 - (rangerCount / myActiveUnits.Size())) * rangerValue;
		tankValue = (1 - (tankCount / myActiveUnits.Size())) * tankValue;
	}


	set.AddValue(static_cast<int>(eFuzzyAI::SPAWN_GRUNT), gruntValue);
	set.AddValue(static_cast<int>(eFuzzyAI::SPAWN_RANGER), rangerValue);
	set.AddValue(static_cast<int>(eFuzzyAI::SPAWN_TANK), tankValue);

	return set;
}

CU::FuzzySet AIDirector::UpdateDefendAdvisor()
{
	CU::FuzzySet set(static_cast<int>(eFuzzyAI::_COUNT));

	float gruntCount = 0;
	float rangerCount = 0;
	float tankCount = 0;
	for (int i = 0; i < myActiveUnits.Size(); ++i)
	{
		if (myActiveUnits[i]->GetUnitType() == eUnitType::GRUNT)
		{
			++gruntCount;
		}
		else if (myActiveUnits[i]->GetUnitType() == eUnitType::RANGER)
		{
			++rangerCount;
		}
		else if (myActiveUnits[i]->GetUnitType() == eUnitType::TANK)
		{
			++tankCount;
		}
	}

	float gruntValue = 0.6f;
	float rangerValue = 1.f;
	float tankValue = 1.f;
	if (myActiveUnits.Size() > 0)
	{
		gruntValue = (1 - (gruntCount / myActiveUnits.Size())) * gruntValue;
		rangerValue = (1 - (rangerCount / myActiveUnits.Size())) * rangerValue;
		tankValue = (1 - (tankCount / myActiveUnits.Size())) * tankValue;
	}


	set.AddValue(static_cast<int>(eFuzzyAI::SPAWN_GRUNT), gruntValue);
	set.AddValue(static_cast<int>(eFuzzyAI::SPAWN_RANGER), rangerValue);
	set.AddValue(static_cast<int>(eFuzzyAI::SPAWN_TANK), tankValue);
	return set;
}

CU::FuzzySet AIDirector::UpdateResourceAdvisor()
{
	int optimalGunpowderCount = 200;
	float gunpowderValue = 5.f;

	float threshHold = 1.f - (static_cast<float>(myGunpowder) / static_cast<float>(optimalGunpowderCount));

	int ownedPointsCount = PollingStation::GetInstance()->GetResourcePointsCount(myOwner);
	int totalPointsCount = PollingStation::GetInstance()->GetResourcePoints().Size();
	float pointModifier = 1.f - (static_cast<float>(ownedPointsCount) / static_cast<float>(totalPointsCount));

	float fuzzyValue = threshHold * gunpowderValue;
	fuzzyValue *= pointModifier;

	CU::FuzzySet set(static_cast<int>(eFuzzyAI::_COUNT));
	set.AddValue(static_cast<int>(eFuzzyAI::TAKE_RESOURCE_POINT), fuzzyValue);

	return set;
}

bool AIDirector::UpdateCurrentAction()
{
	switch (myCurrentAction.myFuzzyAction)
	{
	case eFuzzyAI::SPAWN_GRUNT:
		myCurrentAction.myIsDone = myBuilding->GetComponent<BuildingComponent>()->GetCurrentBuildTime() <= 0.f;
		break;
	case eFuzzyAI::SPAWN_RANGER:
		myCurrentAction.myIsDone = myBuilding->GetComponent<BuildingComponent>()->GetCurrentBuildTime() <= 0.f;
		break;
	case eFuzzyAI::SPAWN_TANK:
		myCurrentAction.myIsDone = myBuilding->GetComponent<BuildingComponent>()->GetCurrentBuildTime() <= 0.f;
		break;
	case eFuzzyAI::TAKE_RESOURCE_POINT:
	{
		bool shouldMove = true;
		float blockWeight = 23.f;
		float playerInfluenceWeight = 6.f;
		float blockMapValue = myMaps.myBlockMap->GetValue(myCurrentAction.myPosition) * blockWeight;
		float playerInfluenceMapValue = myMaps.myPlayerInfluenceMap->GetValue(myCurrentAction.myPosition) * playerInfluenceWeight;

		if (blockMapValue - playerInfluenceMapValue > 0.f)
		{
			myCurrentAction.myIsDone = true;
			shouldMove = false;
		}

		if (myIdleUnits.Size() < 3)
		{
			myCurrentAction.myIsDone = false;
			shouldMove = false;
		}

		for (int i = 0; i < myIdleUnits.Size(); ++i)
		{
			if (myIdleUnits[i]->GetComponent<ControllerComponent>()->IsReady() == false)
			{
				myCurrentAction.myIsDone = false;
				shouldMove = false;
			}
		}

		if (shouldMove == true)
		{
			CU::Vector3<float> target;
			target.x = myCurrentAction.myPosition.x;
			target.z = myCurrentAction.myPosition.y;
			bool quiet = true;
			for (int i = 0; i < myIdleUnits.Size(); ++i)
			{
				CU::Vector2<float> oldTarget = myIdleUnits[i]->GetComponent<ControllerComponent>()->GetTargetPosition();
				ReceiveMessage(BlockMapMessage(oldTarget, false));
				myIdleUnits[i]->GetComponent<ControllerComponent>()->AttackMove(target, true, quiet);
			}
			myCurrentAction.myIsDone = true;
		}
		break;
	}
	case eFuzzyAI::TAKE_VICTORY_POINT:
		DL_ASSERT("eFuzzyAI Case Not Implemented");
		break;
	case eFuzzyAI::TAKE_ARTIFACT:
		DL_ASSERT("eFuzzyAI Case Not Implemented");
		break;
	case eFuzzyAI::UPGRADE_GRUNT:
		DL_ASSERT("eFuzzyAI Case Not Implemented");
		break;
	case eFuzzyAI::UPGRADE_RANGER:
		DL_ASSERT("eFuzzyAI Case Not Implemented");
		break;
	case eFuzzyAI::UPGRADE_TANK:
		DL_ASSERT("eFuzzyAI Case Not Implemented");
		break;
	case eFuzzyAI::FIRST_ACTION:
		myCurrentAction.myIsDone = true;
		break;
	default:
		break;
	}

	return myCurrentAction.myIsDone;
}

bool AIDirector::FuzzyDecisionDone()
{
	return myActionQueue.Size() == 0 && myCurrentAction.myIsDone == true;
}

void AIDirector::InterpretFuzzySet()
{
	DL_ASSERT_EXP(myActionQueue.Size() == 0, "AIDIRECTOR: Queuesize is more than zero when executing fuzzy action");
	eFuzzyAI action = static_cast<eFuzzyAI>(myFuzzySet->GetHighersMember());

	switch (action)
	{
	case eFuzzyAI::SPAWN_GRUNT:
		myActionQueue.InsertFirst(Action(eFuzzyAI::SPAWN_GRUNT));
		break;
	case eFuzzyAI::SPAWN_RANGER:
		myActionQueue.InsertFirst(Action(eFuzzyAI::SPAWN_RANGER));
		break;
	case eFuzzyAI::SPAWN_TANK:
		myActionQueue.InsertFirst(Action(eFuzzyAI::SPAWN_TANK));
		break;
	case eFuzzyAI::TAKE_RESOURCE_POINT:
	{
		if (myIdleUnits.Size() >= 3)
		{
			float maxDist = sqrt(256.f * 256.f + 256.f * 256.f);
			float distWeight = 10.f;
			float blockWeight = 23.f;
			float playerInfluenceWeight = 6.f;

			float bestResult = FLT_MAX;
			Entity* bestPoint = nullptr;

			const CU::GrowingArray<Entity*>& points = PollingStation::GetInstance()->GetResourcePoints();
			for (int i = 0; i < points.Size(); ++i)
			{
				if (points[i]->GetComponent<TriggerComponent>()->GetOwnerGainingPoint() ==
					myOwner)
				{
					continue;
				}

				CU::Vector2<float> pos = points[i]->GetPosition();

				float dist = (CU::Length(pos - myBuilding->GetPosition()) + 0.000001f) / maxDist;
				float blockMapValue = myMaps.myBlockMap->GetValue(pos) * blockWeight;
				float playerInfluenceMapValue = myMaps.myPlayerInfluenceMap->GetValue(pos) * playerInfluenceWeight;

				float result = (blockMapValue) - (playerInfluenceMapValue);
				if (result > 0.f)
				{
					int apa = 5;
				}
				if (result <= 0.f)
				{
					result = abs(result);
					result += dist * distWeight;

					if (result < bestResult)
					{
						bestResult = result;
						bestPoint = points[i];
					}
				}
			}

			if (bestPoint != nullptr)
			{
				myActionQueue.InsertFirst(Action(eFuzzyAI::TAKE_RESOURCE_POINT, bestPoint->GetPosition()));
			}
			else
			{
				myActionQueue.InsertFirst(Action(eFuzzyAI::FIRST_ACTION));
			}
		}
		else
		{
			float grunt = myFuzzySet->GetValue(static_cast<int>(eFuzzyAI::SPAWN_GRUNT));
			float ranger = myFuzzySet->GetValue(static_cast<int>(eFuzzyAI::SPAWN_RANGER));
			float tank = myFuzzySet->GetValue(static_cast<int>(eFuzzyAI::SPAWN_TANK));
			for (int i = 0; i < 3 - myIdleUnits.Size(); ++i)
			{
				if (grunt > ranger && grunt > tank)
				{
					myActionQueue.InsertFirst(Action(eFuzzyAI::SPAWN_GRUNT));
				}
				else if (ranger > tank)
				{
					myActionQueue.InsertFirst(Action(eFuzzyAI::SPAWN_RANGER));
				}
				else
				{
					myActionQueue.InsertFirst(Action(eFuzzyAI::SPAWN_TANK));
				}
			}
			CU::Vector2<float> target = PollingStation::GetInstance()->GetClosestNotOwnedResourcePoint(myOwner
				, myBuilding->GetPosition());
			myActionQueue.InsertFirst(Action(eFuzzyAI::TAKE_RESOURCE_POINT, target));
		}
		break;
	}
	case eFuzzyAI::TAKE_VICTORY_POINT:
		DL_ASSERT("eFuzzyAI Case Not Implemented");
		break;
	case eFuzzyAI::TAKE_ARTIFACT:
		DL_ASSERT("eFuzzyAI Case Not Implemented");
		break;
	case eFuzzyAI::UPGRADE_GRUNT:
		DL_ASSERT("eFuzzyAI Case Not Implemented");
		break;
	case eFuzzyAI::UPGRADE_RANGER:
		DL_ASSERT("eFuzzyAI Case Not Implemented");
		break;
	case eFuzzyAI::UPGRADE_TANK:
		DL_ASSERT("eFuzzyAI Case Not Implemented");
		break;
	default:
		break;
	}

	myCurrentAction.myFuzzyAction = eFuzzyAI::FIRST_ACTION;
	myCurrentAction.myIsDone = true;
}

void AIDirector::StartNextAction()
{
	myCurrentAction = myActionQueue.GetLast();
	myActionQueue.RemoveCyclicAtIndex(myActionQueue.Size() - 1);

	switch (myCurrentAction.myFuzzyAction)
	{
	case eFuzzyAI::SPAWN_GRUNT:
		Director::SpawnUnit(eUnitType::GRUNT);
		break;
	case eFuzzyAI::SPAWN_RANGER:
		Director::SpawnUnit(eUnitType::RANGER);
		break;
	case eFuzzyAI::SPAWN_TANK:
		Director::SpawnUnit(eUnitType::TANK);
		break;
	case eFuzzyAI::TAKE_RESOURCE_POINT:
		//Does not need to do anything here, everything is handled in UpdateCurrentAction
		break;
	case eFuzzyAI::TAKE_VICTORY_POINT:
		DL_ASSERT("eFuzzyAI Case Not Implemented");
		break;
	case eFuzzyAI::TAKE_ARTIFACT:
		DL_ASSERT("eFuzzyAI Case Not Implemented");
		break;
	case eFuzzyAI::UPGRADE_GRUNT:
		DL_ASSERT("eFuzzyAI Case Not Implemented");
		break;
	case eFuzzyAI::UPGRADE_RANGER:
		DL_ASSERT("eFuzzyAI Case Not Implemented");
		break;
	case eFuzzyAI::UPGRADE_TANK:
		DL_ASSERT("eFuzzyAI Case Not Implemented");
		break;
	default:
		break;
	}
}