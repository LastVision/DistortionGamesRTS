#pragma once
#include "Component.h"

class PromotionComponent :
	public Component
{
public:
	PromotionComponent(Entity& aEntity, PromotionComponentData& aData);
	~PromotionComponent();

	void Update(float aDelta) override;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;


private:
	void operator=(PromotionComponent&) = delete;

	int myKillCount;
};

inline eComponentType PromotionComponent::GetTypeStatic()
{
	return eComponentType::PROMOTION;
}

inline eComponentType PromotionComponent::GetType()
{
	return GetTypeStatic();
}