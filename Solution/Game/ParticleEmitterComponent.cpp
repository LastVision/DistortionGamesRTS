#include "stdafx.h"

#include "Camera.h"
#include "Constants.h"
#include "DestroyEmitterMessage.h"
#include <EmitterDataContainer.h>
#include "EmitterNote.h"
#include "Entity.h"
#include "ParticleEmitterComponent.h"
#include <ParticleEmitterData.h>
#include <ParticleEmitterInstance.h>
#include "PostMaster.h"
#include "Scene.h"
#include <XMLReader.h>

ParticleEmitterComponent::ParticleEmitterComponent(Entity& aEntity)
	: Component(aEntity)
	, myEmitters(12)
{
}

ParticleEmitterComponent::~ParticleEmitterComponent()
{
#ifndef DLL_EXPORT
	if (myEntity.GetType() != eEntityType::PLAYER)
	{
		PostMaster::GetInstance()->SendMessage(DestroyEmitterMessage(this));
	}
#endif

	myEmitters.DeleteAll();
}

void ParticleEmitterComponent::Init(const std::string& aPath)
{
	myXMLPath = aPath;

	if (aPath.rfind("P_speed_feeling_emitter.xml") != std::string::npos)
	{
		AddEmitter(aPath);
	}
	else
	{
		ReadList(aPath);
	}
}

void ParticleEmitterComponent::Update(float aDeltaTime)
{
	if (myEntity.GetAlive() == true)
	{
		for (int i = 0; i < myEmitters.Size(); ++i)
		{
			myEmitters[i]->myEmitter->Update(aDeltaTime, myEntity.myOrientation);
		}
	}
}

void ParticleEmitterComponent::Render()
{
	if (myEntity.GetAlive() == true)
	{
		for (int i = 0; i < myEmitters.Size(); ++i)
		{
			myEmitters[i]->myEmitter->Render(&myEntity.GetScene().GetCamera());
		}
	}
}

Prism::ParticleEmitterInstance* ParticleEmitterComponent::GetEmitter(int anIndex)
{
	if (this != nullptr)
		return myEmitters[anIndex]->myEmitter;

	return nullptr;

}

void ParticleEmitterComponent::ReceiveNote(const EmitterNote& aNote)
{
	if (aNote.myType == EmitterNote::eType::BULLET)
	{
		for (int i = 0; i < myEmitters.Size(); ++i)
		{
			myEmitters[i]->myEmitter->ToggleActive(aNote.myIsActive);
			myEmitters[i]->myEmitter->ShouldLive(aNote.myShouldLive);
		}
	}
}

void ParticleEmitterComponent::ReadList(const std::string& aPath)
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
			AddEmitter(entityPath);
		}
	}
	rootDocument.CloseDocument();
}

void ParticleEmitterComponent::AddEmitter(const std::string& aPath)
{
	Emitter* emitter = new Emitter();
	emitter->myEmitter = new Prism::ParticleEmitterInstance();
	myEmitters.Add(emitter);

	myEmitters[myEmitters.Size()-1]->myEmitter->Initiate(Prism::Engine::GetInstance()->GetEmitterDataContainer()->
		GetParticleData(aPath), myEntity.GetType() == eEntityType::PLAYER);
}

ParticleEmitterComponent::Emitter::~Emitter()
{
	delete myEmitter;
	myEmitter = nullptr;
}