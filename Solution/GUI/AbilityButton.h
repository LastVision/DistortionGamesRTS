#pragma once
#include "ButtonWidget.h"

namespace Prism
{
	class SpriteProxy;
}

class PlayerDirector;

namespace GUI
{
	class AbilityButton : public ButtonWidget
	{
	public:
		AbilityButton(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const PlayerDirector* aPlayer);
		~AbilityButton();

		void Render(const CU::Vector2<float>& aParentPosition) override;

		void SetValue(const float& aFirstValue, const float& aSecondValue) override;

	private:
		const float* myCooldown;
		const float* myMaxCooldown;
		Prism::SpriteProxy* myCooldownIndicator;
		Prism::SpriteProxy* myHasCooldownSprite;
	};
}