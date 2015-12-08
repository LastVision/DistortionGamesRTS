#include "stdafx.h"
#include "MiniMapWidget.h"
#include <MoveCameraMessage.h>
#include <Sprite.h>
#include <PostMaster.h>
#include <XMLReader.h>

namespace GUI
{
	MiniMapWidget::MiniMapWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement)
	{
		CU::Vector2<float> size;
		CU::Vector2<float> position;

		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "x", size.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "size"), "y", size.y);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "x", position.x);
		aReader->ForceReadAttribute(aReader->ForceFindFirstChild(anXMLElement, "position"), "y", position.y);

		mySize = size;
		myPosition = position;

		myPlaceholderSprite = new Prism::Sprite("Data/Resource/Texture/UI/T_minimap_placeholder.dds", mySize);
	}

	MiniMapWidget::~MiniMapWidget()
	{
		SAFE_DELETE(myPlaceholderSprite);
	}

	void MiniMapWidget::Render(const CU::Vector2<float>& aParentPosition)
	{
		myPlaceholderSprite->Render(aParentPosition + myPosition);
	}

	void MiniMapWidget::OnMousePressed(const CU::Vector2<float>& aPosition)
	{
		CU::Vector2<float> position = aPosition - myPosition;
		position /= mySize;
		PostMaster::GetInstance()->SendMessage(MoveCameraMessage(position));
	}

	void MiniMapWidget::OnResize(const CU::Vector2<float>& aNewWindowSize, const CU::Vector2<float>& anOldWindowSize)
	{
		Widget::OnResize(aNewWindowSize, anOldWindowSize);
		myPlaceholderSprite->SetSize(mySize, { 0.f, 0.f });
	}
}