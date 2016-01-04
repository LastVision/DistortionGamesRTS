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

	for (int i = 0; i < static_cast<int>(eParticleType::_COUNT); ++i)
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
	for (int i = 0; i < static_cast<int>(eParticleType::_COUNT); ++i)
	{
		for (int j = 0; j < PREALLOCATED_EMITTER_SIZE; ++j)
		{
			if (myEmitters[i]->myEmitterA[j]->IsActive() == true)
			{
				myEmitters[i]->myEmitterA[j]->Update(aDeltaTime, aWorldMatrix);
				myEmitters[i]->myEmitterB[j]->Update(aDeltaTime, aWorldMatrix);
				myEmitters[i]->myEmitterC[j]->Update(aDeltaTime, aWorldMatrix);
			}
		}
	}
}

void EmitterManager::RenderEmitters(Prism::Camera* aCamera)
{

	myEmitters[0]->myEmitterA[0]->SetGPUData(aCamera);

	for (int i = 0; i < static_cast<int>(eParticleType::_COUNT); ++i)
	{
		for (int j = 0; j < PREALLOCATED_EMITTER_SIZE; ++j)
		{
			if (myEmitters[i]->myEmitterA[j]->IsActive() == true)
			{
				myEmitters[i]->myEmitterA[j]->Render();
				myEmitters[i]->myEmitterB[j]->Render();
				myEmitters[i]->myEmitterC[j]->Render();
			}
		}
	}
}

void EmitterManager::ReceiveMessage(const EmitterMessage& aMessage)
{
	CU::Vector3f position = aMessage.myPosition;
	if (aMessage.myEntityID != -1)
	{
		position = EntityId::GetInstance()->GetEntity(aMessage.myEntityID)->GetOrientation().GetPos();
		position.y += 2;
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
			ReadList(entityPath);
			EmitterData* newData = new EmitterData(entityPath);
			myEmitters.Insert(ID, newData);
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

