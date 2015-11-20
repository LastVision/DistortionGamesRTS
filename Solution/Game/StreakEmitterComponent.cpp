#include "stdafx.h"

#include "AIComponent.h"
#include "Camera.h"
#include "Constants.h"

#include "StreakEmitterComponent.h"
#include <StreakEmitterData.h>
#include <StreakEmitterInstance.h>
#include "EmitterNote.h"
#include <EmitterDataContainer.h>
#include "DestroyEmitterMessage.h"
#include "Entity.h"
#include "PostMaster.h"
#include "Scene.h"
#include <XMLReader.h>


StreakEmitterComponent::StreakEmitterComponent(Entity& aEntity)
	: Component(aEntity)
	, myEmitterData(nullptr)
	, myEmitters(8)
{
}

StreakEmitterComponent::~StreakEmitterComponent()
{
	myEmitters.DeleteAll();
}

void StreakEmitterComponent::Init(std::string aPath)
{
	myXMLPath = aPath;

	DL_ASSERT_EXP(myEmitterData== nullptr, "StreakEmitter were inited twice. Contact Linus Ljungman");

	XMLReader reader;
	reader.OpenDocument(myXMLPath);

	tinyxml2::XMLElement* element = reader.ForceFindFirstChild(reader.ForceFindFirstChild("root"), "Emitter");

	std::string streakEmitterDataPath;

	reader.ForceReadAttribute(reader.ForceFindFirstChild(element, "Path"), "src", streakEmitterDataPath);

	myEmitterData = Prism::Engine::GetInstance()->GetEmitterDataContainer()->GetStreakData(streakEmitterDataPath);
	//myEmitterData = new Prism::StreakEmitterData();
	//myEmitterData->LoadDataFile(streakEmitterDataPath.c_str());

	for (tinyxml2::XMLElement* positionElement = reader.ForceFindFirstChild(element, "Position"); positionElement != nullptr
			; positionElement = reader.FindNextElement(positionElement, "Position"))
	{
		CU::Vector3<float> position;

		reader.ForceReadAttribute(positionElement, "X", position.x);
		reader.ForceReadAttribute(positionElement, "Y", position.y);
		reader.ForceReadAttribute(positionElement, "Z", position.z);

		CU::Matrix44<float> orientation;
		orientation.SetPos(position);
		AddStreak(orientation);
	}

	reader.CloseDocument();
}

void StreakEmitterComponent::Update(float aDeltaTime)
{
	if (myEntity.GetAlive() == true)
	{
		AIComponent* comp = myEntity.GetComponent<AIComponent>();
		for (int i = 0; i < myEmitters.Size(); ++i)
		{
			if (comp != nullptr && comp->GetCouldMove() == false)
			{
				myEmitters[i]->myEmitter->Reset();
			}
			myEmitters[i]->myEmitter->SetOrientation(myEmitters[i]->myOrientation * myEntity.myOrientation);
			myEmitters[i]->myEmitter->Update(aDeltaTime);

		}
	}
}

void StreakEmitterComponent::Render()
{
	if (myEntity.GetAlive() == true)
	{
		for (int i = 0; i < myEmitters.Size(); ++i)
		{
			myEmitters[i]->myEmitter->Render(&myEntity.GetScene().GetCamera());
		}
	}
}

void StreakEmitterComponent::AddStreak(const CU::Matrix44f aOrientation)
{
	Emitter* emitter = new Emitter();
	emitter->myEmitter = new Prism::StreakEmitterInstance(*myEmitterData);
	emitter->myOrientation = aOrientation;
	myEmitters.Add(emitter);
}

void StreakEmitterComponent::Reset()
{
	for (int i = 0; i < myEmitters.Size(); ++i)
	{
		myEmitters[i]->myEmitter->Reset();
	}
}


StreakEmitterComponent::Emitter::~Emitter()
{
	delete myEmitter;
}