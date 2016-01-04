#include "stdafx.h"

#include "Instance.h"
#include "SelectionComponent.h"


SelectionComponent::SelectionComponent(Entity& aEntity, SelectionComponentData& aComponentData)
	: Component(aEntity)
	, myHover(nullptr)
{
	Prism::ModelProxy* model = Prism::ModelLoader::GetInstance()->LoadModel(aComponentData.myModelPath
		, aComponentData.myEffectPath);

	myInstance = new Prism::Instance(*model, myEntity.myOrientation, myEntity.GetOctreeType(), myCullingRadius);
}


SelectionComponent::~SelectionComponent()
{
}

void SelectionComponent::Update(float aDeltaTime)
{
	if (myEntity.IsHovered() == true)
	{

	}
	if (myEntity.IsSelected() == true)
	{

	}
}
