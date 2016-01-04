#include "stdafx.h"

#include <Engine.h> //WHy neeeded include??
#include "Effect.h"
#include "Instance.h"
#include <ModelLoader.h>
#include <ModelProxy.h>
#include "SelectionComponent.h"
#include "SelectionComponentData.h"
#include <Scene.h>


SelectionComponent::SelectionComponent(Entity& aEntity, SelectionComponentData& aComponentData)
	: Component(aEntity)
	, myInstance(nullptr)
	, myCullingRadius(2.5f)
{
	Prism::ModelProxy* model = Prism::ModelLoader::GetInstance()->LoadModel(aComponentData.myModelPath
		, aComponentData.myEffectPath);
	myInstance = new Prism::Instance(*model, myEntity.GetOrientation(), myEntity.GetOctreeType(), myCullingRadius);
}


SelectionComponent::~SelectionComponent()
{
	SAFE_DELETE(myInstance);
}

void SelectionComponent::Update(float aDeltaTime)
{
	if (myEntity.IsSelected() == true)
	{
		myInstance->SetSelected(true);
	}
	else if (myEntity.IsHovered() == true)
	{
		myInstance->SetHovered(true);
	}
	else
	{
		myInstance->SetHovered(false);
		myInstance->SetSelected(false);
	}
	
}