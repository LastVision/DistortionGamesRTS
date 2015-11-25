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

class GraphicsComponent : public Component
{
public:
	GraphicsComponent(Entity& aEntity, const char* aModelPath, const char* aEffectPath);
	~GraphicsComponent();

	void InitDLL(const char* aModelPath, const char* aEffectPath);
	void InitCube(float aWidth, float aHeight, float aDepth);
	void Update(float aDeltaTime);
	Prism::Instance* GetInstance();
	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

	void SetPosition(const CU::Vector3<float>& aPosition);
	void SetScale(const CU::Vector3<float>& aScale);

	float GetCullingRadius() const;

private:
	Prism::Instance* myInstance;
	float myCullingRadius;
};

inline Prism::Instance* GraphicsComponent::GetInstance()
{
	return myInstance;
}

inline eComponentType GraphicsComponent::GetTypeStatic()
{
	return eComponentType::GRAPHICS;
}

inline eComponentType GraphicsComponent::GetType()
{
	return GetTypeStatic();
}

inline float GraphicsComponent::GetCullingRadius() const
{
	return myCullingRadius;
}