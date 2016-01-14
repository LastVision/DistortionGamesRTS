#pragma once
#include "Component.h"

namespace Prism
{
	class SpriteProxy;
}

struct PromotionComponentData;
class PromotionComponent :
	public Component
{
public:
	PromotionComponent(Entity& aEntity, PromotionComponentData& aData);
	~PromotionComponent();
	void Reset() override;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

	void RenderPromotion(const Prism::Camera& aCamera);

	void EnemyKilled();
	bool GetPromoted() const;


private:
	void operator=(PromotionComponent&) = delete;

	int myKillCount;
	Prism::SpriteProxy* mySprite;


	int myCurrentKillCount;
};

inline eComponentType PromotionComponent::GetTypeStatic()
{
	return eComponentType::PROMOTION;
}

inline eComponentType PromotionComponent::GetType()
{
	return GetTypeStatic();
}

inline void PromotionComponent::EnemyKilled()
{
	++myCurrentKillCount;
}

inline bool PromotionComponent::GetPromoted() const
{
	return myCurrentKillCount >= myKillCount;
}