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
	GraphicsComponent(Entity& aEntity);
	~GraphicsComponent();

	void Init(const char* aModelPath, const char* aEffectPath, const CU::Matrix44<float>* aCockpitOrientation = nullptr);
	void InitDLL(const char* aModelPath, const char* aEffectPath);
	void InitGeometry(const Prism::MeshData& aMeshData);
	void InitCube(float aWidth, float aHeight, float aDepth);
	void Update(float aDeltaTime);
	Prism::Instance* GetInstance();
	static eComponentType GetType();

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

inline eComponentType GraphicsComponent::GetType()
{
	return eComponentType::GRAPHICS;
}

inline float GraphicsComponent::GetCullingRadius() const
{
	return myCullingRadius;
}