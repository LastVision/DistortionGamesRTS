#include "stdafx.h"
#include "MiniMapWidget.h"
#include <PostMaster.h>
#include <MoveCameraMessage.h>

namespace GUI
{
	MiniMapWidget::MiniMapWidget()
	{
	}

	MiniMapWidget::~MiniMapWidget()
	{
	}

	void MiniMapWidget::OnMouseDown(const CU::Vector2<float>& aPosition)
	{
		CU::Vector2<float> position = aPosition - myPosition;
		position /= mySize;
		PostMaster::GetInstance()->SendMessage(MoveCameraMessage(position));
	}
}