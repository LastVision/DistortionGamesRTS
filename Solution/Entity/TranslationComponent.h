#pragma once
#include "Component.h"

class TranslationComponent : public Component 
{
public:
	TranslationComponent(Entity& aEntity);
	~TranslationComponent();


	static eComponentType GetTypeStatic();


};

inline eComponentType TranslationComponent::GetTypeStatic()
{
	return eComponentType::TRANSLATION;
}