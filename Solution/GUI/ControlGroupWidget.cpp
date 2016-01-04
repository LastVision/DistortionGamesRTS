#include "stdafx.h"
#include "ControlGroupWidget.h"
#include <Engine.h>
#include "../Game/PlayerDirector.h"

namespace GUI
{
	ControlGroupWidget::ControlGroupWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const PlayerDirector* aPlayer)
		: myControlGroups(aPlayer->GetControlGroups())
	{
	}

	ControlGroupWidget::~ControlGroupWidget()
	{
	}

	void ControlGroupWidget::Render(const CU::Vector2<float>& aParentPosition)
	{
		CU::Vector2<float> position = myPosition + aParentPosition;

		for (int i = 0; i < AMOUNT_OF_CONTROL_GROUPS; i++)
		{
			if (myControlGroups[i].Size() > 0)
			{
				Prism::Engine::GetInstance()->PrintText(i + 1, position, Prism::eTextType::RELEASE_TEXT);
				position.x += 50.f;
			}
		}
	}
}