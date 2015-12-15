#include "stdafx.h"

#include <CommonHelper.h>
#include "EmitterManager.h"
#include <EmitterMessage.h>
#include <ParticleDataContainer.h>
#include <Entity.h>
#include <EntityId.h>
#include <ParticleEmitterInstance.h>
#include <PostMaster.h>
#include <XMLReader.h>


EmitterManager::EmitterManager()
	: myXMLPaths(64)
{
	PostMaster* postMaster = PostMaster::GetInstance();
	postMaster->Subscribe(eMessageType::PARTICLE, this);

	int index = 0;
	ReadListOfLists("Data/Resource/Particle/LI_emitter_lists.xml");

	for (int i = 0; i < static_cast<int>(eEmitterTypeID::COUNT); ++i)
	{
		for (int j = 0; j < PREALLOCATED_EMITTER_SIZE; ++j)
		{
			Prism::ParticleEmitterInstance* newEmitter;

			newEmitter = new Prism::ParticleEmitterInstance(Prism::ParticleDataContainer::GetInstance()->
				GetParticleData(myXMLPaths[index].c_str()), false);
			myEmitters[i]->myEmitterA.Insert(j, newEmitter);

			newEmitter = new Prism::ParticleEmitterInstance(Prism::ParticleDataContainer::GetInstance()->
				GetParticleData(myXMLPaths[index + 1].c_str()), false);
			myEmitters[i]->myEmitterB.Insert(j, newEmitter);

			newEmitter = new Prism::ParticleEmitterInstance(Prism::ParticleDataContainer::GetInstance()->
				GetParticleData(myXMLPaths[index + 2].c_str()), false);
			myEmitters[i]->myEmitterC.Insert(j, newEmitter);
		}
		index += 3;
	}
}

EmitterManager::~EmitterManager()
{
	PostMaster* postMaster = PostMaster::GetInstance();
	postMaster->UnSubscribe(eMessageType::PARTICLE, this);

	myEmitters.DeleteAll();
}

//void EmitterManager::AddEmitter(ParticleEmitterComponent* anEmitter)
//{
//	myEmitters.Add(anEmitter);
//}

void EmitterManager::UpdateEmitters(float aDeltaTime, CU::Matrix44f aWorldMatrix)
{
	for (int i = 0; i < static_cast<int>(eEmitterTypeID::COUNT); ++i)
	{
		for (int j = 0; j < PREALLOCATED_EMITTER_SIZE; ++j)
		{
			myEmitters[i]->myEmitterA[j]->Update(aDeltaTime, aWorldMatrix);
			myEmitters[i]->myEmitterB[j]->Update(aDeltaTime, aWorldMatrix);
			myEmitters[i]->myEmitterC[j]->Update(aDeltaTime, aWorldMatrix);
		}
	}
}

void EmitterManager::RenderEmitters(Prism::Camera* aCamera)
{
	for (int i = 0; i < static_cast<int>(eEmitterTypeID::COUNT); ++i)
	{
		for (int j = 0; j < PREALLOCATED_EMITTER_SIZE; ++j)
		{
			myEmitters[i]->myEmitterA[j]->Render(aCamera);
			myEmitters[i]->myEmitterB[j]->Render(aCamera);
			myEmitters[i]->myEmitterC[j]->Render(aCamera);
		}
	}
}

void EmitterManager::ReceiveMessage(const EmitterMessage& aMessage)
{
	if (aMessage.myMessageType == eMessageType::PARTICLE)
	{
		CU::Vector3f position;
		if (aMessage.myEntityID != -1)
		{
			position = EntityId::GetInstance()->GetEntity(aMessage.myEntityID)->GetOrientation().GetPos();
			position.y += 5;
		}

		int particleEffectIndex = static_cast<int>(aMessage.myParticleType);
		if (myEmitters[particleEffectIndex]->myEmitterIndex >= PREALLOCATED_EMITTER_SIZE)
		{
			myEmitters[particleEffectIndex]->myEmitterIndex = 0;
		}
		int emitterIndex = myEmitters[particleEffectIndex]->myEmitterIndex;

		myEmitters[particleEffectIndex]->myEmitterA[emitterIndex]->SetPosition(position);
		myEmitters[particleEffectIndex]->myEmitterA[emitterIndex]->Activate();

		myEmitters[particleEffectIndex]->myEmitterB[emitterIndex]->SetPosition(position);
		myEmitters[particleEffectIndex]->myEmitterB[emitterIndex]->Activate();

		myEmitters[particleEffectIndex]->myEmitterC[emitterIndex]->SetPosition(position);
		myEmitters[particleEffectIndex]->myEmitterC[emitterIndex]->Activate();

		myEmitters[particleEffectIndex]->myEmitterIndex++;
	}
}

//void EmitterManager::ReceiveMessage(const DestroyEmitterMessage& aMessage)
//{
//	myEmitters.RemoveNonCyclic(aMessage.myParticleEmitterComponent);
//}
//
//void EmitterManager::ReceiveMessage(const SpawnExplosionMessage& aMessage)
//{
//	myIsCloseToPlayer = false;
//
//	CU::Vector3f distanceToEmitter = aMessage.myPosition - myPlayer->myOrientation.GetPos();
//	float dist = CU::Length2(distanceToEmitter) * 0.1f;
//	float v = (64.f * 64.f);
//	if (dist < v)
//	{
//		myIsCloseToPlayer = true;
//	}
//
//	if (aMessage.GetMessageType() == eMessageType::SPAWN_EXPLOSION_ON_UNIT_DEATH)
//	{
//		EnemyExplosion(aMessage);
//	}
//
//	if (aMessage.GetMessageType() == eMessageType::SPAWN_EFFECT_ON_HIT)
//	{
//		OnHitEffect(aMessage);
//	}
//
//	if (aMessage.GetMessageType() == eMessageType::SPAWN_EXPLOSION_ON_ASTROID_DEATH)
//	{
//		AstroidExplosion(aMessage);
//	}
//
//	if (aMessage.GetMessageType() == eMessageType::SPAWN_EXPLOSION_ON_PROP_DEATH)
//	{
//		PropExplosion(aMessage);
//	}
//
//	if (aMessage.GetMessageType() == eMessageType::SPAWN_EXPLOSION_ON_ROCKET_DEATH)
//	{
//		RocketExplosion(aMessage);
//	}
//
//	if (aMessage.GetMessageType() == eMessageType::SPAWN_EFFECT_ON_ASTROID_HIT)
//	{
//		OnAstroidHitEffect(aMessage);
//	}
//
//	if (aMessage.GetMessageType() == eMessageType::SPAWN_ON_FINAL_STRUCTURE_1)
//	{
//		OnFirstFinal(aMessage);
//	}
//
//	if (aMessage.GetMessageType() == eMessageType::SPAWN_ON_FINAL_STRUCTURE_2)
//	{
//		OnSecondFinal(aMessage);
//	}
//
//	if (aMessage.GetMessageType() == eMessageType::SPAWN_ON_FINAL_STRUCTURE_3)
//	{
//		OnThirdFinal(aMessage);
//	}
//
//}

void EmitterManager::ReadListOfLists(std::string aPath)
{
	XMLReader rootDocument;
	rootDocument.OpenDocument(aPath);
	tinyxml2::XMLElement* rootElement = rootDocument.FindFirstChild("root");
	for (tinyxml2::XMLElement* e = rootDocument.FindFirstChild(rootElement); e != nullptr;
		e = rootDocument.FindNextElement(e))
	{
		std::string entityPath = "";
		rootDocument.ForceReadAttribute(e, "src", entityPath);
		int ID;
		rootDocument.ForceReadAttribute(e, "ID", ID);
		if (entityPath != "")
		{
			if (ID == static_cast<int>(eEmitterTypeID::BLOOD))
			{
				ReadList(entityPath);
				EmitterData* newData = new EmitterData(entityPath);
				myEmitters.Insert(ID, newData);
			}
			
		}
	}
	rootDocument.CloseDocument();
}

void EmitterManager::ReadList(std::string aPath)
{
	XMLReader rootDocument;
	rootDocument.OpenDocument(aPath);
	tinyxml2::XMLElement* rootElement = rootDocument.FindFirstChild("root");
	for (tinyxml2::XMLElement* e = rootDocument.FindFirstChild(rootElement); e != nullptr;
		e = rootDocument.FindNextElement(e))
	{
		std::string entityPath = "";
		rootDocument.ForceReadAttribute(e, "src", entityPath);
		if (entityPath != "")
		{
			myXMLPaths.Add(entityPath);
		}
	}
	rootDocument.CloseDocument();
}
//
//void EmitterManager::EnemyExplosion(const SpawnExplosionMessage& aMessage)
//{
//	int index = static_cast<int>(eExplosionID::ENEMY_EXPLOSION);
//	if (myExplosions[index]->myEmitterIndex >= PREALLOCATED_EMITTER_SIZE)
//	{
//		myExplosions[index]->myEmitterIndex = 0;
//	}
//	int emitterIndex = myExplosions[index]->myEmitterIndex;
//
//	myExplosions[index]->myEmitterA[emitterIndex]->SetPosition(aMessage.myPosition);
//	myExplosions[index]->myEmitterA[emitterIndex]->ToggleActive(true);
//	myExplosions[index]->myEmitterA[emitterIndex]->ShouldLive(true);
//
//	myExplosions[index]->myEmitterB[emitterIndex]->SetPosition(aMessage.myPosition);
//	myExplosions[index]->myEmitterB[emitterIndex]->ToggleActive(true);
//	myExplosions[index]->myEmitterB[emitterIndex]->ShouldLive(true);
//
//	myExplosions[index]->myEmitterC[emitterIndex]->SetPosition(aMessage.myPosition);
//	myExplosions[index]->myEmitterC[emitterIndex]->ToggleActive(true);
//	myExplosions[index]->myEmitterC[emitterIndex]->ShouldLive(true);
//
//	myExplosions[index]->myEmitterIndex++;
//}
//
//void EmitterManager::PropExplosion(const SpawnExplosionMessage& aMessage)
//{
//	int index = static_cast<int>(eExplosionID::PROP_EXPLOSION);
//	if (myExplosions[index]->myEmitterIndex >= PREALLOCATED_EMITTER_SIZE)
//	{
//		myExplosions[index]->myEmitterIndex = 0;
//	}
//	int emitterIndex = myExplosions[index]->myEmitterIndex;
//
//	myExplosions[index]->myEmitterA[emitterIndex]->SetPosition(aMessage.myPosition);
//	myExplosions[index]->myEmitterA[emitterIndex]->ToggleActive(true);
//	myExplosions[index]->myEmitterA[emitterIndex]->ShouldLive(true);
//	myExplosions[index]->myEmitterA[emitterIndex]->SetIsCloseToPlayer(false);
//
//	if (myIsCloseToPlayer == true)
//	{
//		myExplosions[index]->myEmitterA[emitterIndex]->SetIsCloseToPlayer(true);
//	}
//
//	myExplosions[index]->myEmitterB[emitterIndex]->SetPosition(aMessage.myPosition);
//	myExplosions[index]->myEmitterB[emitterIndex]->ToggleActive(true);
//	myExplosions[index]->myEmitterB[emitterIndex]->ShouldLive(true);
//	myExplosions[index]->myEmitterB[emitterIndex]->SetIsCloseToPlayer(false);
//
//	if (myIsCloseToPlayer == true)
//	{
//		myExplosions[index]->myEmitterB[emitterIndex]->SetIsCloseToPlayer(true);
//	}
//
//	myExplosions[index]->myEmitterC[emitterIndex]->SetPosition(aMessage.myPosition);
//	myExplosions[index]->myEmitterC[emitterIndex]->ToggleActive(true);
//	myExplosions[index]->myEmitterC[emitterIndex]->ShouldLive(true);
//	myExplosions[index]->myEmitterC[emitterIndex]->SetIsCloseToPlayer(false);
//
//	if (myIsCloseToPlayer == true)
//	{
//		myExplosions[index]->myEmitterC[emitterIndex]->SetIsCloseToPlayer(true);
//	}
//
//	myExplosions[index]->myEmitterIndex++;
//}
//
//void EmitterManager::AstroidExplosion(const SpawnExplosionMessage& aMessage)
//{
//	int index = static_cast<int>(eExplosionID::ASTROID_EXPLOSION);
//	if (myExplosions[index]->myEmitterIndex >= PREALLOCATED_EMITTER_SIZE)
//	{
//		myExplosions[index]->myEmitterIndex = 0;
//	}
//	int emitterIndex = myExplosions[index]->myEmitterIndex;
//
//	myExplosions[index]->myEmitterA[emitterIndex]->SetPosition(aMessage.myPosition);
//	myExplosions[index]->myEmitterA[emitterIndex]->ToggleActive(true);
//	myExplosions[index]->myEmitterA[emitterIndex]->ShouldLive(true);
//	myExplosions[index]->myEmitterA[emitterIndex]->SetIsCloseToPlayer(false);
//
//	if (myIsCloseToPlayer == true)
//	{
//		myExplosions[index]->myEmitterA[emitterIndex]->SetIsCloseToPlayer(true);
//	}
//
//	myExplosions[index]->myEmitterB[emitterIndex]->SetPosition(aMessage.myPosition);
//	myExplosions[index]->myEmitterB[emitterIndex]->ToggleActive(true);
//	myExplosions[index]->myEmitterB[emitterIndex]->ShouldLive(true);
//	myExplosions[index]->myEmitterB[emitterIndex]->SetIsCloseToPlayer(false);
//
//	if (myIsCloseToPlayer == true)
//	{
//		myExplosions[index]->myEmitterB[emitterIndex]->SetIsCloseToPlayer(true);
//	}
//
//	myExplosions[index]->myEmitterC[emitterIndex]->SetPosition(aMessage.myPosition);
//	myExplosions[index]->myEmitterC[emitterIndex]->ToggleActive(true);
//	myExplosions[index]->myEmitterC[emitterIndex]->ShouldLive(true);
//	myExplosions[index]->myEmitterC[emitterIndex]->SetIsCloseToPlayer(false);
//
//	if (myIsCloseToPlayer == true)
//	{
//		myExplosions[index]->myEmitterC[emitterIndex]->SetIsCloseToPlayer(true);
//	}
//
//	myExplosions[index]->myEmitterIndex++;
//}
//
//void EmitterManager::RocketExplosion(const SpawnExplosionMessage& aMessage)
//{
//	int index = static_cast<int>(eExplosionID::ROCKET_EXPLOSION);
//	if (myExplosions[index]->myEmitterIndex >= PREALLOCATED_EMITTER_SIZE)
//	{
//		myExplosions[index]->myEmitterIndex = 0;
//	}
//	int emitterIndex = myExplosions[index]->myEmitterIndex;
//
//	myExplosions[index]->myEmitterA[emitterIndex]->SetPosition(aMessage.myPosition);
//	myExplosions[index]->myEmitterA[emitterIndex]->ToggleActive(true);
//	myExplosions[index]->myEmitterA[emitterIndex]->ShouldLive(true);
//	myExplosions[index]->myEmitterA[emitterIndex]->SetIsCloseToPlayer(false);
//	if (myIsCloseToPlayer == true)
//	{
//		myExplosions[index]->myEmitterA[emitterIndex]->SetIsCloseToPlayer(true);
//	}
//	myExplosions[index]->myEmitterB[emitterIndex]->SetPosition(aMessage.myPosition);
//	myExplosions[index]->myEmitterB[emitterIndex]->ToggleActive(true);
//	myExplosions[index]->myEmitterB[emitterIndex]->ShouldLive(true);
//	myExplosions[index]->myEmitterB[emitterIndex]->SetIsCloseToPlayer(false);
//	if (myIsCloseToPlayer == true)
//	{
//		myExplosions[index]->myEmitterB[emitterIndex]->SetIsCloseToPlayer(true);
//	}
//	myExplosions[index]->myEmitterC[emitterIndex]->SetPosition(aMessage.myPosition);
//	myExplosions[index]->myEmitterC[emitterIndex]->ToggleActive(true);
//	myExplosions[index]->myEmitterC[emitterIndex]->ShouldLive(true);
//	myExplosions[index]->myEmitterC[emitterIndex]->SetIsCloseToPlayer(false);
//	if (myIsCloseToPlayer == true)
//	{
//		myExplosions[index]->myEmitterC[emitterIndex]->SetIsCloseToPlayer(true);
//	}
//	myExplosions[index]->myEmitterIndex++;
//}
//
//void EmitterManager::OnHitEffect(const SpawnExplosionMessage& aMessage)
//{
//	if (myIsCloseToPlayer == false)
//	{
//		int index = static_cast<int>(eExplosionID::ONHIT_EFFECT);
//		if (myExplosions[index]->myEmitterIndex >= PREALLOCATED_EMITTER_SIZE)
//		{
//			myExplosions[index]->myEmitterIndex = 0;
//		}
//		int emitterIndex = myExplosions[index]->myEmitterIndex;
//
//		myExplosions[index]->myEmitterA[emitterIndex]->SetPosition(aMessage.myPosition);
//		myExplosions[index]->myEmitterA[emitterIndex]->ToggleActive(true);
//		myExplosions[index]->myEmitterA[emitterIndex]->ShouldLive(true);
//		myExplosions[index]->myEmitterA[emitterIndex]->SetIsCloseToPlayer(true);
//
//		myExplosions[index]->myEmitterB[emitterIndex]->SetPosition(aMessage.myPosition);
//		myExplosions[index]->myEmitterB[emitterIndex]->ToggleActive(true);
//		myExplosions[index]->myEmitterB[emitterIndex]->ShouldLive(true);
//		myExplosions[index]->myEmitterB[emitterIndex]->SetIsCloseToPlayer(true);
//
//		myExplosions[index]->myEmitterC[emitterIndex]->SetPosition(aMessage.myPosition);
//		myExplosions[index]->myEmitterC[emitterIndex]->ToggleActive(true);
//		myExplosions[index]->myEmitterC[emitterIndex]->ShouldLive(true);
//		myExplosions[index]->myEmitterC[emitterIndex]->SetIsCloseToPlayer(true);
//
//		myExplosions[index]->myEmitterIndex++;
//	}
//}
//
//void EmitterManager::OnAstroidHitEffect(const SpawnExplosionMessage& aMessage)
//{
//	if (myIsCloseToPlayer == false)
//	{
//		int index = static_cast<int>(eExplosionID::ON_ASTROID_HIT_EFFECT);
//		if (myExplosions[index]->myEmitterIndex >= PREALLOCATED_EMITTER_SIZE)
//		{
//			myExplosions[index]->myEmitterIndex = 0;
//		}
//		int emitterIndex = myExplosions[index]->myEmitterIndex;
//
//		myExplosions[index]->myEmitterA[emitterIndex]->SetPosition(aMessage.myPosition);
//		myExplosions[index]->myEmitterA[emitterIndex]->ToggleActive(true);
//		myExplosions[index]->myEmitterA[emitterIndex]->ShouldLive(true);
//		myExplosions[index]->myEmitterA[emitterIndex]->SetIsCloseToPlayer(true);
//
//		myExplosions[index]->myEmitterB[emitterIndex]->SetPosition(aMessage.myPosition);
//		myExplosions[index]->myEmitterB[emitterIndex]->ToggleActive(true);
//		myExplosions[index]->myEmitterB[emitterIndex]->ShouldLive(true);
//		myExplosions[index]->myEmitterB[emitterIndex]->SetIsCloseToPlayer(true);
//
//		myExplosions[index]->myEmitterC[emitterIndex]->SetPosition(aMessage.myPosition);
//		myExplosions[index]->myEmitterC[emitterIndex]->ToggleActive(true);
//		myExplosions[index]->myEmitterC[emitterIndex]->ShouldLive(true);
//		myExplosions[index]->myEmitterC[emitterIndex]->SetIsCloseToPlayer(true);
//
//		myExplosions[index]->myEmitterIndex++;
//	}
//}
//
//void EmitterManager::OnFirstFinal(const SpawnExplosionMessage& aMessage)
//{
//	int index = static_cast<int>(eExplosionID::FINAL_EXPLOSION_1);
//	if (myExplosions[index]->myEmitterIndex >= PREALLOCATED_EMITTER_SIZE)
//	{
//		myExplosions[index]->myEmitterIndex = 0;
//	}
//	int emitterIndex = myExplosions[index]->myEmitterIndex;
//
//	myExplosions[index]->myEmitterA[emitterIndex]->SetPosition(aMessage.myPosition);
//	myExplosions[index]->myEmitterA[emitterIndex]->ToggleActive(true);
//	myExplosions[index]->myEmitterA[emitterIndex]->ShouldLive(true);
//
//	myExplosions[index]->myEmitterB[emitterIndex]->SetPosition(aMessage.myPosition);
//	myExplosions[index]->myEmitterB[emitterIndex]->ToggleActive(true);
//	myExplosions[index]->myEmitterB[emitterIndex]->ShouldLive(true);
//
//	myExplosions[index]->myEmitterC[emitterIndex]->SetPosition(aMessage.myPosition);
//	myExplosions[index]->myEmitterC[emitterIndex]->ToggleActive(true);
//	myExplosions[index]->myEmitterC[emitterIndex]->ShouldLive(true);
//
//	myExplosions[index]->myEmitterIndex++;
//}
//
//void EmitterManager::OnSecondFinal(const SpawnExplosionMessage& aMessage)
//{
//	int index = static_cast<int>(eExplosionID::FINAL_EXPLOSION_3);
//	if (myExplosions[index]->myEmitterIndex >= PREALLOCATED_EMITTER_SIZE)
//	{
//		myExplosions[index]->myEmitterIndex = 0;
//	}
//	int emitterIndex = myExplosions[index]->myEmitterIndex;
//
//	myExplosions[index]->myEmitterA[emitterIndex]->SetPosition(aMessage.myPosition);
//	myExplosions[index]->myEmitterA[emitterIndex]->ToggleActive(true);
//	myExplosions[index]->myEmitterA[emitterIndex]->ShouldLive(true);
//
//	myExplosions[index]->myEmitterB[emitterIndex]->SetPosition(aMessage.myPosition);
//	myExplosions[index]->myEmitterB[emitterIndex]->ToggleActive(true);
//	myExplosions[index]->myEmitterB[emitterIndex]->ShouldLive(true);
//
//	myExplosions[index]->myEmitterC[emitterIndex]->SetPosition(aMessage.myPosition);
//	myExplosions[index]->myEmitterC[emitterIndex]->ToggleActive(true);
//	myExplosions[index]->myEmitterC[emitterIndex]->ShouldLive(true);
//
//	myExplosions[index]->myEmitterIndex++;
//}
//
//void EmitterManager::OnThirdFinal(const SpawnExplosionMessage& aMessage)
//{
//	int index = static_cast<int>(eExplosionID::FINAL_EXPLOSION_3);
//	if (myExplosions[index]->myEmitterIndex >= PREALLOCATED_EMITTER_SIZE)
//	{
//		myExplosions[index]->myEmitterIndex = 0;
//	}
//	int emitterIndex = myExplosions[index]->myEmitterIndex;
//
//	myExplosions[index]->myEmitterA[emitterIndex]->SetPosition(aMessage.myPosition);
//	myExplosions[index]->myEmitterA[emitterIndex]->ToggleActive(true);
//	myExplosions[index]->myEmitterA[emitterIndex]->ShouldLive(true);
//
//	myExplosions[index]->myEmitterB[emitterIndex]->SetPosition(aMessage.myPosition);
//	myExplosions[index]->myEmitterB[emitterIndex]->ToggleActive(true);
//	myExplosions[index]->myEmitterB[emitterIndex]->ShouldLive(true);
//
//	myExplosions[index]->myEmitterC[emitterIndex]->SetPosition(aMessage.myPosition);
//	myExplosions[index]->myEmitterC[emitterIndex]->ToggleActive(true);
//	myExplosions[index]->myEmitterC[emitterIndex]->ShouldLive(true);
//
//	myExplosions[index]->myEmitterIndex++;
//}

//Data
EmitterData::EmitterData(std::string aType)
	: myType(aType)
	, myEmitterIndex(0)
{
}

EmitterData::~EmitterData()
{
	myEmitterA.DeleteAll();
	myEmitterB.DeleteAll();
	myEmitterC.DeleteAll();
}

