#pragma once
#include "Component.h"
#include <Matrix.h>

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

	void Update(float aDeltaTime);
	Prism::Instance* GetInstance();
	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

	void SetPosition(const CU::Vector3<float>& aPosition);

private:
	Prism::Instance* myInstance;
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