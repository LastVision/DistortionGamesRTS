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

		void SetValue(const float& aValue) override;

	private:
		const float* myCooldown;
	};
}