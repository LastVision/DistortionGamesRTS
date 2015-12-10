#include "stdafx.h"
//#include <Entity.h>
//#include <EntityEnum.h>
//
//#include <TriggerComponent.h>
//#include "TriggerManager.h"
//
//TriggerManager::TriggerManager()
//	: myResourceTriggers(8)
//{
//}
//
//void TriggerManager::Add(TriggerComponent* aComponent)
//{
//	switch (aComponent->GetEntity.GetType())
//	{
//	case eEntityType::RESOURCE_POINT:
//#ifdef _DEBUG
//		DL_ASSERT_EXP(myResourceTriggers.Find(aComponent) < 0, "component already in TriggerManager.");
//#endif
//		myResourceTriggers.Add(aComponent);
//		break;
//	default:
//		DL_ASSERT("Tried to Add invalid EntityType to TriggerManager.");
//		break;
//	}
//}
//
//void TriggerManager::Remove(TriggerComponent* aComponent)
//{
//	switch (aComponent->GetEntity.GetType())
//	{
//	case eEntityType::RESOURCE_POINT:
//		myResourceTriggers.RemoveCyclic(aComponent);
//		break;
//	default:
//		DL_ASSERT("Tried to Remove invalid EntityType to CollisionManager.");
//		break;
//	}
//}
//
//void TriggerManager::Update()
//{
//	//use polling station to get all player units, enemy units etc
//	for (int i = myResourceTriggers.Size() - 1; i >= 0; --i)
//	{
//		myResourceTriggers->Update();
//	}
//}
//
//void TriggerManager::CleanUp()
//{
//	for (int i = myResourceTriggers.Size() - 1; i >= 0; --i)
//	{
//		if (myResourceTriggers[i]->GetEntity().GetAlive() == false)
//		{
//			myResourceTriggers.RemoveCyclicAtIndex(i);
//		}
//	}
//}