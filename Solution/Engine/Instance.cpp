#include "stdafx.h"

#include "Camera.h"
#include "Effect.h"
#include "EffectContainer.h"
#include "Instance.h"
#include "Model.h"
#include "ModelProxy.h"


Prism::Instance::Instance(ModelProxy& aModel, const CU::Matrix44<float>& anOrientation, eOctreeType anOctreeType
		, const float& aObjectCullingRadius)
	: myProxy(aModel)
	, myOctreeType(anOctreeType)
	, myOrientation(anOrientation)
	, myScale({1,1,1})
	, myObjectCullingRadius(aObjectCullingRadius)
{
}

Prism::Instance::~Instance()
{
	delete &myProxy;
}

void Prism::Instance::Render(Camera& aCamera)
{
	if (myProxy.IsLoaded())
	{
		myProxy.GetEffect()->SetViewMatrix(CU::InverseSimple(aCamera.GetOrientation()));
		//myProxy.GetEffect()->SetProjectionMatrix(aCamera.GetProjection());
		myProxy.GetEffect()->SetViewProjectionMatrix(aCamera.GetViewProjection());
		myProxy.GetEffect()->SetScaleVector(myScale);
		myProxy.GetEffect()->SetCameraPosition(aCamera.GetOrientation().GetPos());

		myProxy.Render(myOrientation, aCamera.GetOrientation().GetPos());
	}
}

void Prism::Instance::Render(const CU::Matrix44<float>& aParentMatrix, Camera& aCamera)
{
	if (myProxy.IsLoaded())
	{
		//myProxy.GetEffect()->SetViewMatrix(CU::InverseSimple(aCamera.GetOrientation()));
		//myProxy.GetEffect()->SetProjectionMatrix(aCamera.GetProjection());
		myProxy.GetEffect()->SetViewProjectionMatrix(aCamera.GetViewProjection());
		myProxy.GetEffect()->SetScaleVector(myScale);
		myProxy.GetEffect()->SetCameraPosition(aCamera.GetOrientation().GetPos());

		myProxy.Render(myOrientation * aParentMatrix, aCamera.GetOrientation().GetPos());
	}
}

CU::Vector3<float> Prism::Instance::GetPosition() const
{
	return myOrientation.GetPos();
}

void Prism::Instance::SetEffect(const std::string& aEffectFile)
{
	if (myProxy.IsLoaded())
	{
		myProxy.SetEffect(Engine::GetInstance()->GetEffectContainer()->GetEffect(aEffectFile));
	}
}

void Prism::Instance::SetScale(const CU::Vector3<float>& aScaleVector)
{
	myScale = aScaleVector;
	
}

void Prism::Instance::UpdateDirectionalLights(
	const CU::StaticArray<DirectionalLightData, NUMBER_OF_DIRECTIONAL_LIGHTS>& someDirectionalLightData)
{
	if (myProxy.IsLoaded())
	{
		myProxy.GetEffect()->UpdateDirectionalLights(someDirectionalLightData);
	}
}

void Prism::Instance::UpdatePointLights(
	const CU::StaticArray<PointLightData, NUMBER_OF_POINT_LIGHTS>& somePointLightData)
{
	if (myProxy.IsLoaded())
	{
		myProxy.GetEffect()->UpdatePointLights(somePointLightData);
	}
}

void Prism::Instance::UpdateSpotLights(
	const CU::StaticArray<SpotLightData, NUMBER_OF_SPOT_LIGHTS>& someSpotLightData)
{
	if (myProxy.IsLoaded())
	{
		myProxy.GetEffect()->UpdateSpotLights(someSpotLightData);
	}
}

Prism::ModelProxy& Prism::Instance::GetModel()
{
	return myProxy;
}
