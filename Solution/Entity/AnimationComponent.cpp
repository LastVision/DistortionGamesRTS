#include "stdafx.h"

#include "AnimationComponent.h"
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


AnimationComponent::AnimationComponent(Entity& aEntity, const char* aModelPath, const char* aEffectPath)
	: Component(aEntity)
	, myInstance(nullptr)
	, myCullingRadius(10.f)
{
	Prism::ModelProxy* model = Prism::ModelLoader::GetInstance()->LoadModelAnimated(aModelPath
		, aEffectPath);

	myInstance = new Prism::Instance(*model, myEntity.myOrientation, myEntity.GetOctreeType(), myCullingRadius);
}

AnimationComponent::~AnimationComponent()
{
	if (myEntity.GetOctreeType() != Prism::eOctreeType::NOT_IN_OCTREE && myEntity.GetType() != eEntityType::PLAYER)
	{
		myEntity.GetScene().RemoveInstance(myInstance);
	}
	delete myInstance;
	myInstance = nullptr;
}

void AnimationComponent::AddAnimation(eEntityState aState, const std::string& aAnimationPath)
{
	myAnimations[int(aState)] = aAnimationPath;
}

void AnimationComponent::Update(float aDeltaTime)
{
	if (myPrevEntityState != myEntity.GetState())
	{
		/*Animation* newAnimation = AnimationSystem::GetInstance()->GetAnimation(myAnimations[int(myEntity.GetState())]);
		myInstance->SetAnimation(newAnimation);
		myInstance->ResetAnimationTime();*/
	}

	myInstance->Update(aDeltaTime);

	myPrevEntityState = myEntity.GetState();
}

void AnimationComponent::SetPosition(const CU::Vector3<float>& aPosition)
{
	myEntity.myOrientation.SetPos(aPosition);
}