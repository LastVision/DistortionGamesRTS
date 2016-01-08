#include "stdafx.h"

#include "EmitterManager.h"
#include <EmitterMessage.h>
#include <EntityId.h>
#include <ParticleDataContainer.h>
#include <ParticleEmitterInstance.h>
#include <PostMaster.h>
#include <XMLReader.h>


EmitterManager::EmitterManager()
	: myEmitterList(64)
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
		for (int j = 0; j < myEmitterList[i]->myEmitters.Size(); ++j)
		{
			if (myEmitterList[i]->myEmitters[j]->IsActive() == true)
			{
				myEmitterList[i]->myEmitters[j]->Update(aDeltaTime, aWorldMatrix);
			}
		}
	}
}

void EmitterManager::RenderEmitters(Prism::Camera* aCamera)
{

	Prism::ParticleDataContainer::GetInstance()->SetGPUData(aCamera);
	for (int i = 0; i < myEmitterList.Size(); ++i)
	{
		for (int j = 0; j < myEmitterList[i]->myEmitters.Size(); ++j)
		{
			if (myEmitterList[i]->myEmitters[j]->IsActive() == true)
			{
				myEmitterList[i]->myEmitters[j]->Render();
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

	std::string particleType = aMessage.myParticleTypeString;

	for (int i = 0; i < myEmitters[particleType]->myEmitters.Size(); ++i)
	{
		myEmitters[particleType]->myEmitters[i]->SetPosition(position);
		myEmitters[particleType]->myEmitters[i]->Activate();
	}
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
		if (entityPath != "")
		{
			EmitterData* newData = new EmitterData(entityPath);
			myEmitters[ID] = newData;
			newData->myIdentifier = ID;
			ReadList(entityPath, ID);
		}
	}
	rootDocument.CloseDocument();
}

void EmitterManager::ReadList(const std::string& aPath, const std::string& anID)
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
			Prism::ParticleEmitterInstance* newEmitter;
			newEmitter = new Prism::ParticleEmitterInstance(Prism::ParticleDataContainer::GetInstance()->
				GetParticleData(entityPath), false);
			myEmitters[anID]->myEmitters.Add(newEmitter);
		}
	}
	rootDocument.CloseDocument();
}

//Data
EmitterData::EmitterData(const std::string& aType)
	: myType(aType)
	, myEmitterIndex(0)
	, myEmitters(16)
{
}

EmitterData::~EmitterData()
{
	myEmitters.DeleteAll();
}

