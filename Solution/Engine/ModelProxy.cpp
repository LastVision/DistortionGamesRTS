#include "stdafx.h"

#include "Model.h"
#include "ModelProxy.h"

namespace Prism
{
	ModelProxy::ModelProxy()
		: myModel(nullptr)
	{
	}

	void ModelProxy::Render(const CU::Matrix44<float>& aOrientation, const CU::Vector3<float>& aCameraPosition)
	{
		if (myModel != nullptr)
		{
			myModel->Render(aOrientation, aCameraPosition);
		}
	}

	void ModelProxy::SetModel(Model* aModel)
	{
		myModel = aModel;
	}

	Effect* ModelProxy::GetEffect()
	{
		if (myModel == nullptr)
		{
			return nullptr;
		}

		return myModel->GetEffect();
	}

	void ModelProxy::SetEffect(Effect* aEffect)
	{
		myModel->SetEffect(aEffect);
	}
}