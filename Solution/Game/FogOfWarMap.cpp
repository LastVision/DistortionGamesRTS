#include "stdafx.h"
#include "FogOfWarMap.h"
#include <PollingStation.h>
#include <Entity.h>
#include <RenderPlane.h>
#include <TriggerComponent.h>

#define OUTER_RING 10.f
#define MIN_DARKNESS 0.3f

FogOfWarMap::FogOfWarMap()
	: AIMap(256)
{
	myMaxValue = 1.f;
}


FogOfWarMap::~FogOfWarMap()
{
}

void FogOfWarMap::Update()
{
	for (int i = 0; i < myGrid.Size(); ++i)
	{
		myGrid[i] = MIN_DARKNESS;
	}

	AddValue(1.f, 20.f, PollingStation::GetInstance()->GetBase(eOwnerType::PLAYER)->GetPosition());

	const CU::GrowingArray<Entity*>& playerUnits = PollingStation::GetInstance()->GetUnits(eOwnerType::PLAYER);
	for (int j = 0; j < playerUnits.Size(); ++j)
	{
		AddValue(1.f, 20.f, playerUnits[j]->GetPosition());
	}

	const CU::GrowingArray<Entity*>& playerPoints = PollingStation::GetInstance()->GetVictoryAndResourcePoints();
	for (int i = 0; i < playerPoints.Size(); ++i)
	{
		if (playerPoints[i]->GetComponent<TriggerComponent>()->GetOwnerGainingPoint() == eOwnerType::PLAYER)
		{
			AddValue(1.f, 20.f, playerPoints[i]->GetPosition());
		}
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
			if (distance < aRadius - OUTER_RING && ValidIndex(x, y))
			{
				int index = x + y * mySide;
				myGrid[index] = aValue;
			}
			else if (distance >= aRadius - OUTER_RING && distance <= aRadius)
			{
				int index = x + y * mySide;
				//myGrid[index] = aValue;
				/*float rest = aRadius - OUTER_RING;
				float parts = 1.f / OUTER_RING;
				float toPoint = (distance - rest) * parts;
				float divider = (rest / myTileSize);
				float value = aValue - toPoint;*/

				float inner = aRadius - (OUTER_RING);
				float newDist = distance - inner;
				float newMax = aRadius - inner;
				float value = 1.f - (newDist / newMax);

				myGrid[index] += value;
				myGrid[index] = fminf(myGrid[index], myMaxValue);
				myGrid[index] = fmaxf(myGrid[index], MIN_DARKNESS);


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