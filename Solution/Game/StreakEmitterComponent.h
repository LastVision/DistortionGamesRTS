#pragma once
#include "Component.h"

namespace Prism
{
	class StreakEmitterInstance;
	class StreakEmitterData;
}

class StreakEmitterComponent : public Component
{
public:
	StreakEmitterComponent(Entity& aEntity);
	~StreakEmitterComponent();
	void Init(std::string aPath);
	void Update(float aDeltaTime) override;
	void Render();
	static eComponentType GetType();

	void AddStreak(const CU::Matrix44f aOrientation);

	void Reset() override;

private:
	struct Emitter
	{
		~Emitter();

		CU::Matrix44f myOrientation;
		Prism::StreakEmitterInstance* myEmitter;
	};

	CU::GrowingArray<Emitter*> myEmitters;

	Prism::StreakEmitterData* myEmitterData;

	std::string myXMLPath;
};



inline eComponentType StreakEmitterComponent::GetType()
{
	return eComponentType::STREAK_EMITTER;
}

