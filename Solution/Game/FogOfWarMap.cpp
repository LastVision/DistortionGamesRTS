#include "stdafx.h"
#include "FogOfWarMap.h"
#include <PollingStation.h>
#include <Entity.h>
#include <RenderPlane.h>

FogOfWarMap::FogOfWarMap()
	: AIMap(256)
{
}


FogOfWarMap::~FogOfWarMap()
{
}

void FogOfWarMap::Update()
{
	for (int i = 0; i < myGrid.Size(); ++i)
	{
		myGrid[i] = 0.1f;
	}
	const CU::GrowingArray<Entity*>& playerUnits = PollingStation::GetInstance()->GetUnits(eOwnerType::PLAYER);
	for (int j = 0; j < playerUnits.Size(); ++j)
	{
		AddValue(1.f, 20.f, playerUnits[j]->GetPosition());
	}
}

void FogOfWarMap::AddValue(float aValue, float aRadius, const CU::Vector2<float>& aPosition)
{
	CU::Vector2<int> topLeft(GetIntPosition(aPosition - aRadius));
	CU::Vector2<int> botRight(GetIntPosition(aPosition + aRadius));

	for (int y = topLeft.y; y < botRight.y; ++y)
	{
		for (int x = topLeft.x; x < botRight.x; ++x)
		{
			float distance = CU::Length(GetPosition(x, y) - aPosition);
			if (distance < aRadius && ValidIndex(x, y))
			{
				int index = x + y * mySide;
				myGrid[index] = aValue;
			}
		}
	}

	int index = GetIndex(aPosition);
	if (index >= 0 && index < myGrid.Size())
	{
		myGrid[index] = aValue;;
	}
}

void FogOfWarMap::UpdateRenderPlane()
{
	myPlane->StartModify();

	for (int i = 0; i < myGrid.Size(); ++i)
	{
		myPlane->SetVertexColor(i, GetColor(myGrid[i]));
	}

	myPlane->EndModify();
}