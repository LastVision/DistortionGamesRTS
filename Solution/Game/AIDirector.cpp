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

AIDirector::AIDirector(const Prism::Terrain& aTerrain, Prism::Scene& aScene)
	: Director(eOwnerType::ENEMY, aTerrain)
	, myPlayerHasStarted(false)
	, myIdleUnits(32)
	, myUnitsOnMission(32)
	, mySurviveGatherer(nullptr)
	, myInfluenceRenderIndex(0)
	, myRedistributeUnitsTimer(10.f)
	, myCurrentRedistributeUnitsTimer(10.f)
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

	myInfluenceMap = new InfluenceMap();
	myPlayerInfluenceMap = new InfluenceMap();
	myNeutralInfluenceMap = new InfluenceMap();
	myPlayerNeutralCombinedInfluence = new InfluenceMap();
	myGoalMap = new InfluenceMap();
	myTensionMap = new TensionMap(myInfluenceMap, myPlayerInfluenceMap, myNeutralInfluenceMap
		, myGoalMap);
	myDifferenceMap = new DifferenceMap(myInfluenceMap, myPlayerNeutralCombinedInfluence);
	myVulnerabilityMap = new VulnerabilityMap(myDifferenceMap, myTensionMap);

	myDecisionMap = new DecisionMap();
	myDecisionMap->SetVulnerabilityMap(myVulnerabilityMap);
	myDecisionMap->SetDifferenceMap(myDifferenceMap);
	PostMaster::GetInstance()->Subscribe(eMessageType::TIME_MULTIPLIER, this);

	myFuzzySet = new CU::FuzzySet(static_cast<int>(eFuzzyAI::_COUNT));
}

AIDirector::~AIDirector()
{
	SAFE_DELETE(myInfluenceMap);
	SAFE_DELETE(myPlayerInfluenceMap);
	SAFE_DELETE(myNeutralInfluenceMap);
	SAFE_DELETE(myPlayerNeutralCombinedInfluence);
	SAFE_DELETE(myGoalMap);
	SAFE_DELETE(myTensionMap);
	SAFE_DELETE(myDifferenceMap);
	SAFE_DELETE(myVulnerabilityMap);
	SAFE_DELETE(myDecisionMap);
	SAFE_DELETE(myFuzzySet);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::TIME_MULTIPLIER, this);
}

void AIDirector::RenderMaps(const Prism::Camera& aCamera)
{
	switch (myInfluenceRenderIndex)
	{
	case 0:
		break;
	case 1:
		myInfluenceMap->Render(aCamera);
		break;
	case 2:
		myPlayerInfluenceMap->Render(aCamera);
		break;
	case 3:
		myNeutralInfluenceMap->Render(aCamera);
		break;
	case 4:
		myGoalMap->Render(aCamera);
		break;
	case 5:
		myTensionMap->Render(aCamera);
		break;
	case 6:
		myDifferenceMap->Render(aCamera);
		break;
	case 7:
		myPlayerNeutralCombinedInfluence->Render(aCamera);
		break;
	case 8:
		myVulnerabilityMap->Render(aCamera);
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

	if (FuzzyActionDone())
	{
		UpdateActionQueue();
		if (myActionQueue.Size() == 0)
		{
			UpdateAdvisors();
			ExecuteFuzzyAction();
		}
	}

	if (mySurviveGatherer != nullptr && mySurviveGatherer->GetAlive() == false)
	{
		mySurviveGatherer = nullptr;
	}

	myBuilding->Update(aDeltaTime);

	/*if (myBuilding->GetComponent<BuildingComponent>()->GetSpawnQueueSize() > 0)
	{
	return;
	}*/

	/*myCurrentRedistributeUnitsTimer -= aDeltaTime;
	if (myCurrentRedistributeUnitsTimer <= 0.f)
	{
		myCurrentRedistributeUnitsTimer = myRedistributeUnitsTimer;

		bool shouldBeQuiet = true;
		for (int i = 0; i < myActiveUnits.Size(); ++i)
		{
			CU::Vector2<float> targetPos = myDecisionMap->GetOffensivePosition(myActiveUnits[i]->GetPosition());
			CU::Vector3<float> pos(targetPos.x, 0.f, targetPos.y);
			myActiveUnits[i]->GetComponent<ControllerComponent>()->AttackMove(pos
				, true, shouldBeQuiet);
		}
	}*/


	//Director::SpawnUnit(eUnitType::GRUNT);
	//NotLoseLogic();
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
			//myActiveUnits.RemoveCyclicAtIndex(i);
		}
		else
		{
			myUnitsOnMission.Add(myActiveUnits[i]);
		}
	}

	/*for (int i = myIdleUnits.Size() - 1; i >= 0; --i)
	{
		if (myIdleUnits[i]->GetState() != eEntityState::IDLE)
		{
			myActiveUnits.Add(myIdleUnits[i]);
			myIdleUnits.RemoveCyclicAtIndex(i);
		}
	}*/
}

void AIDirector::UpdateActionQueue()
{
	if (myActionQueue.Size() == 0)
	{
		return;
	}

	Action action = myActionQueue.GetLast();

	myCurrentAction = action;
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
	{
		for (int i = 0; i < myIdleUnits.Size(); ++i)
		{
			if (myIdleUnits[i]->GetComponent<ControllerComponent>()->IsReady() == false)
			{
				return;
			}
		}

		CU::Vector3<float> target;
		target.x = action.myPosition.x;
		target.z = action.myPosition.y;
		bool quiet = true;
		for (int i = 0; i < myIdleUnits.Size(); ++i)
		{
			myIdleUnits[i]->GetComponent<ControllerComponent>()->AttackMove(target, true, quiet);
		}
		myCurrentAction.myIsDone = true;
	}
		//DL_ASSERT("eFuzzyAI Case Not Implemented");
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

void AIDirector::ExecuteFuzzyAction()
{
	DL_ASSERT_EXP(myActionQueue.Size() == 0, "AIDIRECTOR: Queuesize is more than zero when executing fuzzy action");
	eFuzzyAI action = static_cast<eFuzzyAI>(myFuzzySet->GetHighersMember());

	switch (action)
	{
	case eFuzzyAI::SPAWN_GRUNT:
		myActionQueue.InsertFirst(Action(eFuzzyAI::SPAWN_GRUNT));
		//Director::SpawnUnit(eUnitType::GRUNT);
		break;
	case eFuzzyAI::SPAWN_RANGER:
		myActionQueue.InsertFirst(Action(eFuzzyAI::SPAWN_RANGER));
		//Director::SpawnUnit(eUnitType::RANGER);
		break;
	case eFuzzyAI::SPAWN_TANK:
		myActionQueue.InsertFirst(Action(eFuzzyAI::SPAWN_TANK));
		//Director::SpawnUnit(eUnitType::TANK);
		break;
	case eFuzzyAI::TAKE_RESOURCE_POINT:
	{
		if (myIdleUnits.Size() >= 3)
		{
			CU::Vector2<float> target = PollingStation::GetInstance()->GetClosestNotOwnedResourcePoint(myOwner
				, myBuilding->GetPosition());

			//for (int i = 0; i < myIdleUnits.Size(); ++i)
			{
				CU::Vector3<float> target3d;
				target3d.x = target.x;
				target3d.z = target.y;
				bool quiet = true;
				//myIdleUnits[i]->GetComponent<ControllerComponent>()->AttackMove(target3d, true, quiet);
				myActionQueue.InsertFirst(Action(eFuzzyAI::TAKE_RESOURCE_POINT, target));
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
					//Director::SpawnUnit(eUnitType::GRUNT);
				}
				else if (ranger > tank)
				{
					myActionQueue.InsertFirst(Action(eFuzzyAI::SPAWN_RANGER));
					//Director::SpawnUnit(eUnitType::RANGER);
				}
				else
				{
					myActionQueue.InsertFirst(Action(eFuzzyAI::SPAWN_TANK));
					//Director::SpawnUnit(eUnitType::TANK);
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
	//DL_ASSERT("eFuzzyAI Case Not Implemented");
}

bool AIDirector::FuzzyActionDone()
{
	bool done = false;
	bool returnValue = false;

	switch (myCurrentAction.myFuzzyAction)
	{
	case eFuzzyAI::SPAWN_GRUNT:
		done = myBuilding->GetComponent<BuildingComponent>()->GetCurrentBuildTime() <= 0.f;
		returnValue = done;
		break;
	case eFuzzyAI::SPAWN_RANGER:
		done = myBuilding->GetComponent<BuildingComponent>()->GetCurrentBuildTime() <= 0.f;
		returnValue = done;
		break;
	case eFuzzyAI::SPAWN_TANK:
		done = myBuilding->GetComponent<BuildingComponent>()->GetCurrentBuildTime() <= 0.f;
		returnValue = done;
		break;
	case eFuzzyAI::TAKE_RESOURCE_POINT:
		done = myCurrentAction.myIsDone;
		returnValue = true;
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
	case eFuzzyAI::FIRST_ACTION:
		returnValue = true;
		break;
	default:
		break;
	}

	if (done == true && myActionQueue.Size() > 0)
	{
		myActionQueue.RemoveCyclicAtIndex(myActionQueue.Size() - 1);
	}

	return returnValue;
}


//void AIDirector::NotLoseLogic()
//{
//	const PollingStation* pollingStation = PollingStation::GetInstance();
//
//	eAction action = eAction::NONE;
//	CU::Vector3<float> capturePos;
//
//	if (pollingStation->GetResourcePointsCount(eOwnerType::ENEMY) < 1 && mySurviveGatherer == nullptr)
//	{
//		if (myIdleUnits.Size() == 0)
//		{
//			action = eAction::SPAWN_GRUNT;
//		}
//		else
//		{
//			const CU::GrowingArray<Entity*> resources = pollingStation->GetResourcePoints();
//			capturePos.x = resources[0]->GetPosition().x;
//			capturePos.z = resources[0]->GetPosition().y;
//			action = eAction::CAPTURE_POINT;
//			mySurviveGatherer = myIdleUnits.GetLast();
//		}
//	}
//
//
//	int playerVictoryPoints = pollingStation->GetVictoryPointsCount(eOwnerType::PLAYER);
//	if (playerVictoryPoints > 0)
//	{
//		int playerCount = pollingStation->GetUnits(eOwnerType::PLAYER).Size();
//		if (myIdleUnits.Size() == 0 && playerCount > myActiveUnits.Size()-1)
//		{
//			action = eAction::SPAWN_GRUNT;
//		}
//		else if (myIdleUnits.Size() > 0)
//		{
//			const CU::GrowingArray<Entity*> victoryPoints = pollingStation->GetVictoryPoints();
//			for (int i = 0; i < victoryPoints.Size(); ++i)
//			{
//				if (victoryPoints[i]->GetOwner() == eOwnerType::PLAYER)
//				{
//					capturePos.x = victoryPoints[i]->GetPosition().x;
//					capturePos.z = victoryPoints[i]->GetPosition().y;
//					action = eAction::CAPTURE_POINT;
//					break;
//				}
//			}
//		}
//	}
//	bool shouldBeQuiet = true;
//	switch (action)
//	{
//	case AIDirector::eAction::CAPTURE_POINT:
//	{
//		Entity* unit = myIdleUnits.GetLast();
//		myIdleUnits.RemoveCyclicAtIndex(myIdleUnits.Size() - 1);
//		unit->GetComponent<ControllerComponent>()->AttackMove(capturePos, true, shouldBeQuiet);
//		break;
//	}
//	case AIDirector::eAction::SPAWN_GRUNT:
//		Director::SpawnUnit(eUnitType::GRUNT);
//		break;
//	case AIDirector::eAction::SPAWN_RANGER:
//		Director::SpawnUnit(eUnitType::RANGER);
//		break;
//	case AIDirector::eAction::SPAWN_TANK:
//		Director::SpawnUnit(eUnitType::TANK);
//		break;
//	case AIDirector::eAction::NONE:
//		break;
//	default:
//		break;
//	}
//}

//void AIDirector::WinSlowlyLogic()
//{
//	const PollingStation* pollingStation = PollingStation::GetInstance();
//
//	int victoryPoints = pollingStation->GetVictoryPointsCount(eOwnerType::ENEMY);
//	int totalVictoryPoints = pollingStation->GetVictoryPoints().Size();
//
//	if ((totalVictoryPoints / 2) > victoryPoints)
//	{
//		//I dont have more than 50% of VictoryPoints?!?!? CAPTURE MOAR
//	}
//
//	//If the player has more than 2 resourcepoints I need to do something about it
//
//	//If I have less than 2 resourcepoints I need to do something about it
//}

void AIDirector::UpdateInfluences()
{
	CU::InputWrapper* input = CU::InputWrapper::GetInstance();

	if (input->KeyIsPressed(DIK_LSHIFT) || input->KeyIsPressed(DIK_RSHIFT))
	{
		if (input->KeyDown(DIK_F5))
		{
			myInfluenceRenderIndex = 5;
		}
		else if (input->KeyDown(DIK_F6))
		{
			myInfluenceRenderIndex = 6;
		}
		else if (input->KeyDown(DIK_F7))
		{
			myInfluenceRenderIndex = 7;
		}
		else if (input->KeyDown(DIK_F8))
		{
			myInfluenceRenderIndex = 8;
		}
		else if (input->KeyDown(DIK_F9))
		{
			//myInfluenceRenderIndex = 4;
		}
	}
	else
	{
		if (input->KeyDown(DIK_F5))
		{
			myInfluenceRenderIndex = 0;
		}
		else if (input->KeyDown(DIK_F6))
		{
			myInfluenceRenderIndex = 1;
		}
		else if (input->KeyDown(DIK_F7))
		{
			myInfluenceRenderIndex = 2;
		}
		else if (input->KeyDown(DIK_F8))
		{
			myInfluenceRenderIndex = 3;
		}
		else if (input->KeyDown(DIK_F9))
		{
			myInfluenceRenderIndex = 4;
		}
	}





	myInfluenceMap->Update();
	for (int i = 0; i < myActiveUnits.Size(); ++i)
	{
		myInfluenceMap->AddValue(1.f, 30.f, myActiveUnits[i]->GetPosition());
	}

	myPlayerInfluenceMap->Update();
	const CU::GrowingArray<Entity*>& playerUnits = PollingStation::GetInstance()->GetUnits(eOwnerType::PLAYER);
	for (int i = 0; i < playerUnits.Size(); ++i)
	{
		myPlayerInfluenceMap->AddValue(1.f, 30.f, playerUnits[i]->GetPosition());
	}

	myNeutralInfluenceMap->Update();
	const CU::GrowingArray<Entity*>& neutralUnits = PollingStation::GetInstance()->GetUnits(eOwnerType::NEUTRAL);
	for (int i = 0; i < neutralUnits.Size(); ++i)
	{
		myNeutralInfluenceMap->AddValue(1.f, 30.f, neutralUnits[i]->GetPosition());
	}

	for (int i = 0; i < myPlayerNeutralCombinedInfluence->GetGrid().Size(); ++i)
	{
		myPlayerNeutralCombinedInfluence->SetValue(i, myPlayerInfluenceMap->GetGrid()[i] + myNeutralInfluenceMap->GetGrid()[i]);
	}

	myGoalMap->Update();
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
		myGoalMap->AddValue(victoryValue, 30.f, victoryPoint[i]->GetPosition());
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
		myGoalMap->AddValue(resourceValue, 30.f, resourcePoints[i]->GetPosition());
	}

	const CU::GrowingArray<Entity*>& artifacts = PollingStation::GetInstance()->GetArtifacts();
	for (int i = 0; i < artifacts.Size(); ++i)
	{
		myGoalMap->AddValue(0.3f, 5.f, artifacts[i]->GetPosition());
	}

	myDifferenceMap->Update();
	myTensionMap->Update();
	myVulnerabilityMap->Update();
}