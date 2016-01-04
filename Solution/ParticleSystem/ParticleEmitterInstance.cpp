#include "stdafx.h"
#include "Camera.h"
#include <CommonHelper.h>
#include <d3dx11effect.h>
#include <MathHelper.h>
#include "ParticleEmitterInstance.h"
#include "VertexBufferWrapper.h"
#include <Effect.h>
#include <TextureContainer.h>
#include <Engine.h>
namespace Prism
{
	ParticleEmitterInstance::ParticleEmitterInstance(ParticleEmitterData* someData, bool anAllowManyParticles)
		: myVertexWrapper(nullptr)
		, myIsActive(false)
		, myEmissionTime(0)
		, myEmitterLife(0)
		, myParticleIndex(0)
		, myDeadParticleCount(0)
	{
		myParticleEmitterData = someData;
		myEmitterPath = myParticleEmitterData->myFileName;

		int particleCount = static_cast<int>(myParticleEmitterData->myParticlesPerEmitt * myParticleEmitterData->myParticlesLifeTime / myParticleEmitterData->myEmissionRate) + 1;


		DL_DEBUG(("Loading :" + myEmitterPath).c_str());
		DL_ASSERT_EXP(anAllowManyParticles == true || particleCount <= 201, "Can't have more than 201 particles in an emitter!");

		myGraphicalParticles.Init(particleCount);
		myLogicalParticles.Init(particleCount);

		myEmissionTime = myParticleEmitterData->myEmissionRate;

		myDiffColor = (myParticleEmitterData->myData.myEndColor - myParticleEmitterData->myData.myStartColor) / myParticleEmitterData->myParticlesLifeTime;

		for (int i = 0; i < particleCount; ++i)
		{
			GraphicalParticle tempGraphics;
			myGraphicalParticles.Add(tempGraphics);
			LogicalParticle tempLogic;
			myLogicalParticles.Add(tempLogic);
		}

		myIsActive = myParticleEmitterData->myIsActiveAtStart;

		myEmitterLife = myParticleEmitterData->myEmitterLifeTime;

		myTechniqueDesc = new _D3DX11_TECHNIQUE_DESC();

		CreateVertexBuffer();

	}

	ParticleEmitterInstance::~ParticleEmitterInstance()
	{
		if (myVertexWrapper != nullptr && myVertexWrapper->myVertexBuffer != nullptr)
		{
			myVertexWrapper->myVertexBuffer->Release();
		}


		SAFE_DELETE(myVertexWrapper);
		SAFE_DELETE(myTechniqueDesc);
	}

	void ParticleEmitterInstance::ReleaseData()
	{
		myParticleEmitterData = nullptr;
	}

	void ParticleEmitterInstance::Render()
	{
		UpdateVertexBuffer();
		myParticleEmitterData->myEffect->SetTexture(TextureContainer::GetInstance()->GetTexture(myParticleEmitterData->myTextureName));

		Engine::GetInstance()->GetContex()->IASetVertexBuffers(
			myVertexWrapper->myStartSlot
			, myVertexWrapper->myNumberOfBuffers
			, &myVertexWrapper->myVertexBuffer
			, &myVertexWrapper->myStride
			, &myVertexWrapper->myByteOffset);


		for (UINT i = 0; i < myTechniqueDesc->Passes; ++i)
		{
			myParticleEmitterData->myEffect->GetTechnique()->GetPassByIndex(i)->Apply(0, Engine::GetInstance()->GetContex());
			Engine::GetInstance()->GetContex()->Draw(myGraphicalParticles.Size(), 0);
		}
	}

	void ParticleEmitterInstance::Update(float aDeltaTime, const CU::Matrix44f& aWorldMatrix)
	{
		UpdateEmitter(aDeltaTime, aWorldMatrix);
	}

	void ParticleEmitterInstance::CreateVertexBuffer()
	{

		myVertexWrapper = new VertexBufferWrapper();
		myVertexWrapper->myStride = sizeof(GraphicalParticle);
		myVertexWrapper->myByteOffset = 0;
		myVertexWrapper->myStartSlot = 0;
		myVertexWrapper->myNumberOfBuffers = 1;

		HRESULT hr;
		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

		vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		if (myVertexWrapper->myVertexBuffer != nullptr)
		{
			myVertexWrapper->myVertexBuffer->Release();
		}

		vertexBufferDesc.ByteWidth = sizeof(GraphicalParticle) * myGraphicalParticles.Size();

		hr = Engine::GetInstance()->GetDevice()->CreateBuffer(&vertexBufferDesc, nullptr, &myVertexWrapper->myVertexBuffer);
		DL_ASSERT_EXP(hr == S_OK, "[ParticleEmitterInstance](CreateVertexBuffer) : Failed to create VertexBuffer");

		Engine::GetInstance()->SetDebugName(myVertexWrapper->myVertexBuffer
			, "ParticleEmitterInstance::myVertexWrapper->myVertexBuffer");

	}

	void ParticleEmitterInstance::UpdateVertexBuffer()
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		Engine::GetInstance()->GetContex()->Map(myVertexWrapper->myVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

		if (mappedResource.pData != nullptr)
		{
			GraphicalParticle *data = (GraphicalParticle*)mappedResource.pData;

			bool isSafe = sizeof(GraphicalParticle) == sizeof(myGraphicalParticles[0]);
			DL_ASSERT_EXP(isSafe, "[ParticleEmitter](UpdateVertexBuffer) : Not safe to copy.");
			memcpy(data, &myGraphicalParticles[0], sizeof(GraphicalParticle)* myGraphicalParticles.Size());
		}
		Engine::GetInstance()->GetContex()->Unmap(myVertexWrapper->myVertexBuffer, 0);
	}

	void ParticleEmitterInstance::UpdateEmitter(float aDeltaTime, const CU::Matrix44f& aWorldMatrix)
	{
		myEmissionTime -= aDeltaTime;
		myEmitterLife -= aDeltaTime;

		UpdateParticle(aDeltaTime);

		if (myEmissionTime <= 0.f && (myEmitterLife > 0.f || myParticleEmitterData->myUseEmitterLifeTime == false))
		{
			EmitParticle(aWorldMatrix);
			myEmissionTime = myParticleEmitterData->myEmissionRate;
		}

		if (myEmitterLife <= 0.f && myDeadParticleCount == myLogicalParticles.Size())
		{
			myIsActive = false;
		}
	}

	void ParticleEmitterInstance::UpdateParticle(float aDeltaTime)
	{
		for (int i = 0; i < myLogicalParticles.Size(); ++i)
		{
			if (myGraphicalParticles[i].myAlpha <= 0.0f)
			{
				myDeadParticleCount++;
				myLogicalParticles[i].myIsAlive = false;
				continue;
			}

			myGraphicalParticles[i].myPosition = myGraphicalParticles[i].myPosition -
				(myLogicalParticles[i].myVelocity * myLogicalParticles[i].mySpeed) * aDeltaTime;

			myGraphicalParticles[i].myAlpha += myParticleEmitterData->myData.myAlphaDelta * aDeltaTime;
			myGraphicalParticles[i].mySize += myParticleEmitterData->myData.mySizeDelta * aDeltaTime;

			myGraphicalParticles[i].myColor += myDiffColor  * aDeltaTime;

			myGraphicalParticles[i].myRotation += myLogicalParticles[i].myRotation * aDeltaTime;

			myGraphicalParticles[i].myLifeTime -= aDeltaTime;

		}
	}

	void ParticleEmitterInstance::EmitParticle(const CU::Matrix44f& aWorldMatrix)
	{
		for (int i = 0; i < myParticleEmitterData->myParticlesPerEmitt; ++i)
		{
			if (myParticleIndex == myLogicalParticles.Size() - 1)
			{
				myParticleIndex = 0;
			}
			myDeadParticleCount--;
			myLogicalParticles[myParticleIndex].myIsAlive = true;


			myGraphicalParticles[myParticleIndex].myColor = myParticleEmitterData->myData.myStartColor;

			myGraphicalParticles[myParticleIndex].myPosition =
				CU::Math::RandomVector(aWorldMatrix.GetPos() - myParticleEmitterData->myEmitterSize
				, aWorldMatrix.GetPos() + myParticleEmitterData->myEmitterSize);

			myGraphicalParticles[myParticleIndex].myLifeTime = myParticleEmitterData->myParticlesLifeTime;

			myParticleScaling = CU::Math::RandomRange(myParticleEmitterData->myData.myMinStartSize
				, myParticleEmitterData->myData.myMaxStartSize);

			myGraphicalParticles[myParticleIndex].mySize = 1 * myParticleScaling;

			myLogicalParticles[myParticleIndex].myVelocity.x = CU::Math::RandomRange(myParticleEmitterData->myData.myMinVelocity.x,
				myParticleEmitterData->myData.myMaxVelocity.x);

			myLogicalParticles[myParticleIndex].myVelocity.y = CU::Math::RandomRange(myParticleEmitterData->myData.myMinVelocity.y,
				myParticleEmitterData->myData.myMaxVelocity.y);

			myLogicalParticles[myParticleIndex].myVelocity.z = CU::Math::RandomRange(myParticleEmitterData->myData.myMinVelocity.z,
				myParticleEmitterData->myData.myMaxVelocity.z);

			myLogicalParticles[myParticleIndex].myRotation = CU::Math::RandomRange(myParticleEmitterData->myMinRotation, myParticleEmitterData->myMaxRotation);

			myLogicalParticles[myParticleIndex].mySpeed = myParticleEmitterData->mySpeedMultiplier;

			myGraphicalParticles[myParticleIndex].myAlpha = myParticleEmitterData->myData.myStartAlpha;

			myParticleIndex += 1;
		}
	}

	void ParticleEmitterInstance::SetPosition(CU::Vector3f aPosition)
	{
		myOrientation.SetPos(aPosition);
	}

	void ParticleEmitterInstance::Activate()
	{
		myIsActive = true;
		myEmitterLife = myParticleEmitterData->myEmitterLifeTime;
	}


	void ParticleEmitterInstance::SetGPUData(Camera* aCamera)
	{
		myParticleEmitterData->myEffect->SetWorldMatrix(myOrientation);
		myParticleEmitterData->myEffect->SetViewMatrix(CU::InverseSimple(aCamera->GetOrientation()));
		myParticleEmitterData->myEffect->SetProjectionMatrix(aCamera->GetProjection());
	
		Engine::GetInstance()->GetContex()->IASetInputLayout(myParticleEmitterData->myInputLayout);
		Engine::GetInstance()->GetContex()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		myParticleEmitterData->myEffect->GetTechnique()->GetDesc(myTechniqueDesc);
	}

}

