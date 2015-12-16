#include "stdafx.h"

#include "AnimationComponent.h"
#include "AnimationSystem.h"
#include <Effect.h>
#include <Engine.h>
#include <EngineEnums.h>
#include <EffectContainer.h>
#include <Instance.h>
#include <Model.h>
#include <ModelLoader.h>
#include <ModelProxy.h>
#include <Scene.h>
#include <Terrain.h>
#include <Texture.h>

//#define BOX_MODE

AnimationComponent::AnimationComponent(Entity& aEntity, AnimationComponentData& aComponentData, const Prism::Terrain& aTerrain)
	: Component(aEntity)
	, myInstance(nullptr)
	, myCullingRadius(10.f)
	, myTerrain(aTerrain)
{
#ifndef BOX_MODE
	Prism::ModelProxy* model = Prism::ModelLoader::GetInstance()->LoadModelAnimated(aComponentData.myModelPath
		, aComponentData.myEffectPath);

	myInstance = new Prism::Instance(*model, myEntity.myOrientation, myEntity.GetOctreeType(), myCullingRadius);

	for (int i = 0; i < static_cast<int>(eEntityState::_COUNT); ++i)
	{
		AnimationLoadData loadAnimation = aComponentData.myAnimations[i];
		AddAnimation(loadAnimation.myEntityState, loadAnimation.myAnimationPath, loadAnimation.myLoopFlag, loadAnimation.myResetTimeOnRestart);
	}
#endif
}

AnimationComponent::~AnimationComponent()
{
#ifndef BOX_MODE
	if (myEntity.myIsInScene == true && myEntity.GetOctreeType() != Prism::eOctreeType::NOT_IN_OCTREE
		&& myEntity.GetOwner() != eOwnerType::PLAYER)
	{
		myEntity.GetScene().RemoveInstance(myInstance);
	}
	delete myInstance;
	myInstance = nullptr;
#endif
}

void AnimationComponent::AddAnimation(eEntityState aState, const std::string& aAnimationPath
	, bool aLoopFlag, bool aResetTimeOnRestart)
{
#ifndef BOX_MODE
	Prism::AnimationSystem::GetInstance()->GetAnimation(aAnimationPath.c_str());
	AnimationData newData;
	newData.myElapsedTime = 0.f;
	newData.myFile = aAnimationPath;
	newData.myShouldLoop = aLoopFlag;
	newData.myResetTimeOnRestart = aResetTimeOnRestart;
	myAnimations[int(aState)] = newData;
#endif
}

void AnimationComponent::Update(float aDeltaTime)
{
#ifndef BOX_MODE
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
	

	if (myEntity.GetOwner() != eOwnerType::NOT_USED)
	{
		myInstance->ActivateAlbedo(myEntity.GetOwner());
	}

	data.myElapsedTime += aDeltaTime;
	myPrevEntityState = myEntity.GetState();
#else
	CU::Vector3<float> pos = myTerrain.GetHeight(myEntity.GetOrientation().GetPos(), 2.f);
	Prism::RenderBox(pos, eColorDebug::BLUE, 0.5f);
#endif
}

bool AnimationComponent::IsCurrentAnimationDone() const
{
#ifndef BOX_MODE
	return myInstance->IsAnimationDone();
#else
	return true;
#endif
}

void AnimationComponent::RestartCurrentAnimation()
{
	myInstance->ResetAnimationTime(0.f);
}