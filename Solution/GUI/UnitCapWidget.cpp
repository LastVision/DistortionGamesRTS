#include "stdafx.h"
#include <Engine.h>
#include <sstream>
#include "UnitCapWidget.h"

namespace GUI
{

	UnitCapWidget::UnitCapWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement
		, const CU::Vector2<float>& aSize, const int& aUnitCap, const int& aUnitCount)
		: myUnitCap(aUnitCap)
		, myUnitCount(aUnitCount)
	{
		mySize = aSize;

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "x", myPosition.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "y", myPosition.y);
	}

	UnitCapWidget::~UnitCapWidget()
	{
	}

	void UnitCapWidget::Render(const CU::Vector2<float>& aParentPosition)
	{
		std::stringstream ss;

		ss << myUnitCount << "/" << myUnitCap;

		Prism::Engine::GetInstance()->PrintText(ss.str(), myPosition + aParentPosition, Prism::eTextType::RELEASE_TEXT);
	}
}