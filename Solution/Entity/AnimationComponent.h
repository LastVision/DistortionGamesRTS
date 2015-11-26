#pragma once
#include "Component.h"
#include <Matrix.h>
#include <string>
#include <StaticArray.h>

namespace Prism
{
	class Instance;
	struct MeshData;
	class Camera;
	class Texture;
};

class AnimationComponent : public Component
{
public:
	AnimationComponent(Entity& aEntity, const char* aModelPath, const char* aEffectPath);
	~AnimationComponent();

	void AddAnimation(eEntityState aState, const std::string& aAnimationPath);

	void Update(float aDeltaTime);
	Prism::Instance* GetInstance();
	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

	void SetPosition(const CU::Vector3<float>& aPosition);

private:
	Prism::Instance* myInstance;
	float myCullingRadius;
	CU::StaticArray<std::string, int(eEntityState::_COUNT)> myAnimations;
	eEntityState myPrevEntityState;
};

inline Prism::Instance* AnimationComponent::GetInstance()
{
	return myInstance;
}

inline eComponentType AnimationComponent::GetTypeStatic()
{
	return eComponentType::ANIMATION;
}

inline eComponentType AnimationComponent::GetType()
{
	return GetTypeStatic();
}