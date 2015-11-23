#pragma once

#include "ParticleEmitterData.h"
#include "ParticleData.h"
#include <bitset>
namespace Prism
{
	class Camera;
	struct VertexBufferWrapper;

	class ParticleEmitterInstance
	{
	public:

		ParticleEmitterInstance();
		~ParticleEmitterInstance();
		void ReleaseData();
		void Initiate(ParticleEmitterData* someData, bool anAllowManyParticles = false);
		void Render(Camera* aCamera);
		void Update(float aDeltaTime, const CU::Matrix44f& aWorldMatrix);
		bool GetIsActive();
		void ToggleActive();
		void ToggleActive(bool aIsActive);
		void ShouldLive(bool aToStop);
		void SetPosition(CU::Vector3f aPosition);
		void SetIsCloseToPlayer(bool aIsCloseToPlayer);
	private:

		void CreateVertexBuffer();
		void UpdateVertexBuffer();

		void UpdateEmitter(float aDeltaTime, const CU::Matrix44f& aWorldMatrix);
		void UpdateParticle(float aDeltaTime);

		void EmitParticle(const CU::Matrix44f& aWorldMatrix);

		CU::GrowingArray<LogicalParticle> myLogicalParticles;
		CU::GrowingArray<GraphicalParticle> myGraphicalParticles;

		CU::Vector3f myDiffColor;
		CU::Matrix44f myOrientation;

		ParticleEmitterData* myParticleEmitterData;
		VertexBufferWrapper *myVertexWrapper;

		float myEmissionTime;
		float myEmitterLife;
		float myParticleScaling;

		int myParticleIndex;
		int myDeadParticleCount;

		bool myIsActive;
		bool myShouldLive;
		bool myIsCloseToPlayer;


		std::string myEmitterPath;
	};

	inline void ParticleEmitterInstance::ShouldLive(bool aToStop)
	{
		myShouldLive = aToStop;
	}

	inline void ParticleEmitterInstance::SetIsCloseToPlayer(bool aIsCloseToPlayer)
	{
		myIsCloseToPlayer = aIsCloseToPlayer;
	}


}
