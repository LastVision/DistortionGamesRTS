#include "stdafx.h"

#include "EmitterManager.h"
#include <EmitterMessage.h>
#include <EntityId.h>
#include <ParticleDataContainer.h>
#include <ParticleEmitterInstance.h>
#include <PostMaster.h>
#include <XMLReader.h>
#include <TimerManager.h>
#include "CommonHelper.h"

#define FINISHED 0
#define UNFINISHED 1


EmitterManager::EmitterManager(const Prism::Camera& aCamera)
	: myEmitterList(64)
	, myCamera(aCamera)
	, myAvailableEmitterCount(0)
{
	PostMaster* postMaster = PostMaster::GetInstance();
	postMaster->Subscribe(eMessageType::PARTICLE, this);
	ReadListOfLists("Data/Resource/Particle/LI_emitter_lists.xml");

	for (auto it = myEmitters.begin(); it != myEmitters.end(); ++it)
	{
		myEmitterList.Add(it->second);
	}
}

EmitterManager::~EmitterManager()
{
	PostMaster* postMaster = PostMaster::GetInstance();
	postMaster->UnSubscribe(eMessageType::PARTICLE, this);
	myEmitterList.RemoveAll();
	for (auto it = myEmitters.begin(); it != myEmitters.end(); ++it)
	{
		delete it->second;
		it->second = nullptr;
	}
}

void EmitterManager::UpdateEmitters(float aDeltaTime, CU::Matrix44f aWorldMatrix)
{
	for (int i = 0; i < myEmitterList.Size(); ++i)
	{
		if (myEmitterList[i]->myGroupIsActive == false)
		{
			continue;
		}
		for (int k = 0; k < PREALLOCATED_EMITTERGROUP; k++)
		{
			if (myEmitterList[i]->myFinishedGroups[k] == FINISHED)
			{
				continue;
			}

			for (int j = 0; j < myEmitterList[i]->myEmitters[k].Size(); ++j)
			{
				Prism::ParticleEmitterInstance* instance = myEmitterList[i]->myEmitters[k][j];

				if (instance->IsActive() == false)
				{
					break;
				}
				if (instance->GetEntity() != nullptr)
				{
					instance->SetPosition({ instance->GetEntity()->GetOrientation().GetPos().x, 3.f
						, instance->GetEntity()->GetOrientation().GetPos().z });
				}

				instance->Update(aDeltaTime, aWorldMatrix);
			}
		}
	}
}

void EmitterManager::RenderEmitters()
{

	Prism::ParticleDataContainer::GetInstance()->SetGPUData(myCamera);
	for (int i = 0; i < myEmitterList.Size(); ++i)
	{
		if (myEmitterList[i]->myGroupIsActive == false)
		{
			continue;
		}
		for (int k = 0; k < PREALLOCATED_EMITTERGROUP; k++)
		{
			if (myEmitterList[i]->myFinishedGroups[k] == FINISHED)
			{
				continue;
			}

			for (int j = 0; j < myEmitterList[i]->myEmitters[k].Size(); ++j)
			{

				if (myEmitterList[i]->myEmitters[k][j]->IsActive() == false)
				{
					myEmitterList[i]->myFinishedCount++;
					myEmitterList[i]->myFinishedGroups[k] = FINISHED;
					if (myEmitterList[i]->myFinishedCount >= PREALLOCATED_EMITTERGROUP)
					{
						myEmitterList[i]->myGroupIsActive = false;
					}
					break;
				}

				myEmitterList[i]->myEmitters[k][j]->Render();
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
	

	std::string particleType = CU::ToLower(aMessage.myParticleTypeString);

	DL_ASSERT_EXP(myEmitters.find(particleType) != myEmitters.end(), "Effect did not exist!");



	if (myEmitters[particleType]->myCurrentIndex >= PREALLOCATED_EMITTERGROUP)
	{
		myEmitters[particleType]->myCurrentIndex = 0;
	}

	short index = myEmitters[particleType]->myCurrentIndex;

	for (int i = 0; i < myEmitters[particleType]->myEmitters[index].Size(); ++i)
	{
		Prism::ParticleEmitterInstance* instance = myEmitters[particleType]->myEmitters[index][i];

		if (aMessage.myEntityID != -1)
		{
			instance->SetEntity(EntityId::GetInstance()->GetEntity(aMessage.myEntityID));
		}

		instance->SetPosition(position);
		instance->Activate();
		if (aMessage.myEmitterLifeTime > 0.f)
		{
			instance->SetEmitterLifeTime(aMessage.myEmitterLifeTime);
		}
	}
	myEmitters[particleType]->myFinishedGroups[index] = UNFINISHED;
	myEmitters[particleType]->myFinishedCount--;
	myEmitters[particleType]->myGroupIsActive = true;
	myEmitters[particleType]->myCurrentIndex++;
}

void EmitterManager::ReadListOfLists(const std::string& aPath)
{
	XMLReader rootDocument;
	rootDocument.OpenDocument(aPath);
	tinyxml2::XMLElement* rootElement = rootDocument.FindFirstChild("root");
	for (tinyxml2::XMLElement* e = rootDocument.FindFirstChild(rootElement); e != nullptr; e = rootDocument.FindNextElement(e))
	{
		std::string entityPath = "";
		rootDocument.ForceReadAttribute(e, "src", entityPath);
		std::string ID;
		rootDocument.ForceReadAttribute(e, "ID", ID);
		ID = CU::ToLower(ID);
		if (entityPath != "")
		{
			EmitterData* newData = new EmitterData(entityPath);
			myEmitters[ID] = newData;

			for (short i = 0; i < PREALLOCATED_EMITTERGROUP; ++i)
			{
				ReadList(entityPath, ID, i);
			}
		}
	}
	rootDocument.CloseDocument();
}

void EmitterManager::ReadList(const std::string& aPath, const std::string& anID, short anIndex)
{
	XMLReader rootDocument;
	rootDocument.OpenDocument(aPath);
	tinyxml2::XMLElement* rootElement = rootDocument.FindFirstChild("root");
	for (tinyxml2::XMLElement* e = rootDocument.FindFirstChild(rootElement); e != nullptr;
		e = rootDocument.FindNextElement(e))
	{
		std::string entityPath = "";
		rootDocument.ForceReadAttribute(e, "src", entityPath);
		if (entityPath == "Data/Resource/Particle/P_emitter_weather_snow_medium.xml" ||
			entityPath == "Data/Resource/Particle/P_emitter_weather_snow_small.xml")
		{
			Prism::ParticleEmitterInstance* newEmitter;
			newEmitter = new Prism::ParticleEmitterInstance(Prism::ParticleDataContainer::GetInstance()->
				GetParticleData(entityPath), true);
			myEmitters[anID]->myEmitters[anIndex].Add(newEmitter);
		}
		else if (entityPath != "")
		{
			Prism::ParticleEmitterInstance* newEmitter;
			newEmitter = new Prism::ParticleEmitterInstance(Prism::ParticleDataContainer::GetInstance()->
				GetParticleData(entityPath), false);
			myEmitters[anID]->myEmitters[anIndex].Add(newEmitter);
		}
	}
	rootDocument.CloseDocument();
}

//Data
EmitterData::EmitterData(const std::string& aType)
	: myType(aType)
	, myCurrentIndex(0)
	, myFinishedCount(0)
{
	for (int i = 0; i < PREALLOCATED_EMITTERGROUP; ++i)
	{
		myEmitters[i].Init(16);
	}
	myFinishedGroups.reset();
}

EmitterData::~EmitterData()
{
	for (unsigned short i = 0; i < PREALLOCATED_EMITTERGROUP; ++i)
	{
		myEmitters[i].DeleteAll();
	}
}