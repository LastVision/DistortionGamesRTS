#include "stdafx.h"

#include "GraphicsComponent.h"
#include "GraphicsComponentData.h"
#include <Effect.h>
#include "Entity.h"
#include <Engine.h>
#include <EngineEnums.h>
#include <EffectContainer.h>
#include <Instance.h>
#include <Model.h>
#include <ModelLoader.h>
#include <ModelProxy.h>
#include <Scene.h>
#include <Texture.h>
#include <XMLReader.h>

GraphicsComponent::GraphicsComponent(Entity& aEntity, GraphicsComponentData& aComponentData)
	: Component(aEntity)
	, myInstance(nullptr)
	, myCullingRadius(10.f)
{
	Prism::ModelProxy* model = Prism::ModelLoader::GetInstance()->LoadModel(aComponentData.myModelPath
		, aComponentData.myEffectPath);

	myInstance = new Prism::Instance(*model, myEntity.myOrientation, myEntity.GetOctreeType(), myCullingRadius);
}

GraphicsComponent::~GraphicsComponent()
{
	if (myEntity.GetOctreeType() != Prism::eOctreeType::NOT_IN_OCTREE && myEntity.GetOwner() != eOwnerType::PLAYER)
	{
		myEntity.GetScene().RemoveInstance(myInstance);
	}
	delete myInstance;
	myInstance = nullptr;
}

void GraphicsComponent::InitDLL(const char* aModelPath, const char* aEffectPath)
{
	Prism::ModelProxy* model = Prism::ModelLoader::GetInstance()->LoadModel(aModelPath
		, aEffectPath);

	Prism::EffectContainer::GetInstance()->GetEffect(aEffectPath);
	model->SetEffect(Prism::EffectContainer::GetInstance()->GetEffect(aEffectPath));

	myInstance = new Prism::Instance(*model, myEntity.myOrientation, myEntity.GetOctreeType(), myCullingRadius);
}

void GraphicsComponent::InitCube(float aWidth, float aHeight, float aDepth)
{
	Prism::ModelProxy* model = Prism::ModelLoader::GetInstance()->LoadCube(aWidth, aHeight, aDepth);

	myInstance = new Prism::Instance(*model, myEntity.myOrientation, myEntity.GetOctreeType(), myCullingRadius);
}

void GraphicsComponent::Update(float aDeltaTime)
{
	aDeltaTime;
}

void GraphicsComponent::SetPosition(const CU::Vector3<float>& aPosition)
{
	myEntity.myOrientation.SetPos(aPosition);
}

void GraphicsComponent::SetRotation(const CU::Vector3<float>& aRotation)
{
	CU::Vector3f position(myEntity.myOrientation.GetPos());
}

void GraphicsComponent::SetScale(const CU::Vector3<float>& aScale)
{
	myInstance->SetScale(aScale);
}