#pragma once
#include "../Game/GameDefines.h"
#include "Widget.h"
#include <StaticArray.h>

namespace tinyxml2
{
	class XMLElement;
}

class XMLReader;

namespace GUI
{
	class ButtonWidget;

	class UpgradeButtonWidget : public Widget
	{
	public:
		UpgradeButtonWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const int& aUpgradeLevel);
		~UpgradeButtonWidget();

		void Update() override;
		void Render(const CU::Vector2<float>& aParentPosition) override;

		Widget* MouseIsOver(const CU::Vector2<float>& aPosition) override;

		void OnResize(const CU::Vector2<float>& aNewSize, const CU::Vector2<float>& anOldSize) override;

	private:
		void operator=(UpgradeButtonWidget&) = delete;

		CU::StaticArray<ButtonWidget*, AMOUNT_OF_UPGRADES> myUpgrades;

		const int& myUpgradeLevel;

	};
}