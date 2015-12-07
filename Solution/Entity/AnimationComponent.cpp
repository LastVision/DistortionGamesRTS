#include "stdafx.h"

#include "AnimationComponent.h"
#include "AnimationComponentData.h"
#include "AnimationSystem.h"
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

AnimationComponent::AnimationComponent(Entity& aEntity, AnimationComponentData& aComponentData)
	: Component(aEntity)
	, myInstance(nullptr)
	, myCullingRadius(10.f)
{
	Prism::ModelProxy* model = Prism::ModelLoader::GetInstance()->LoadModelAnimated(aComponentData.myModelPath
		, aComponentData.myEffectPath);

	myInstance = new Prism::Instance(*model, myEntity.myOrientation, myEntity.GetOctreeType(), myCullingRadius);

	for (int i = 0; i < static_cast<int>(eEntityState::_COUNT); ++i)
	{
		AnimationLoadData loadAnimation = aComponentData.myAnimations[i];
		AddAnimation(loadAnimation.myEntityState, loadAnimation.myAnimationPath, loadAnimation.myLoopFlag, loadAnimation.myResetTimeOnRestart);
	}
}

AnimationComponent::~AnimationComponent()
{
	if (myEntity.GetOctreeType() != Prism::eOctreeType::NOT_IN_OCTREE && myEntity.GetOwner() != eOwnerType::PLAYER)
	{
		myEntity.GetScene().RemoveInstance(myInstance);
	}
	delete myInstance;
	myInstance = nullptr;
}

void AnimationComponent::AddAnimation(eEntityState aState, const std::string& aAnimationPath
	, bool aLoopFlag, bool aResetTimeOnRestart)
{
	Prism::AnimationSystem::GetInstance()->GetAnimation(aAnimationPath.c_str());
	AnimationData newData;
	newData.myElapsedTime = 0.f;
	newData.myFile = aAnimationPath;
	newData.myShouldLoop = aLoopFlag;
	newData.myResetTimeOnRestart = aResetTimeOnRestart;
	myAnimations[int(aState)] = newData;
}

void AnimationComponent::Update(float aDeltaTime)
{
	AnimationData& data = myAnimations[int(myEntity.GetState())];
	if (myPrevEntityState != myEntity.GetState())
	{
		myInstance->SetAnimation(Prism::AnimationSystem::GetInstance()->GetAnimation(data.myFile.c_str()));
	
		if (data.myResetTimeOnRestart == true)
		{
			myInstance->ResetAnimationTime(0.f);
		}
		else
		{
			myInstance->ResetAnimationTime(data.myElapsedTime);
		}
	}
	
	if (myInstance->IsAnimationDone() == false || data.myShouldLoop == true)
	{
		myInstance->Update(aDeltaTime);
	}
	
	data.myElapsedTime += aDeltaTime;
	myPrevEntityState = myEntity.GetState();
}

bool AnimationComponent::IsCurrentAnimationDone() const
{
	return myInstance->IsAnimationDone();
}

void AnimationComponent::RestartCurrentAnimation()
{
	myInstance->ResetAnimationTime(0.f);
}