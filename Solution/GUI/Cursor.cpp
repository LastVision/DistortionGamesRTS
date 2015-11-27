#include "stdafx.h"
#include "Cursor.h"
#include "../InputWrapper/InputWrapper.h"
#include <Sprite.h>

namespace GUI
{
	Cursor::Cursor(const CU::Vector2<int>& aWindowSize)
		: mySprite(nullptr)
	{
		myWindowSize.x = float(aWindowSize.x);
		myWindowSize.y = float(aWindowSize.y);

		mySprite = new Prism::Sprite("Data/Resource/Texture/UI/T_crosshair_default.dds", { 70.f, 70.f }, { 35.f, 35.f });
		myPosition = myWindowSize / 2.f;
	}

	Cursor::~Cursor()
	{
		delete mySprite;
		mySprite = nullptr;
	}

	void Cursor::Update()
	{
		// uncomment for software cursor:
		//myPosition.x += myInputWrapper->GetMouseDX();
		//myPosition.y -= myInputWrapper->GetMouseDY();

		myPosition.x = CU::InputWrapper::GetInstance()->GetMousePosition().x;
		myPosition.y = myWindowSize.y - CU::InputWrapper::GetInstance()->GetMousePosition().y;
	}

	void Cursor::Render()
	{
		mySprite->Render(myPosition);
	}

	const CU::Vector2<float>& Cursor::GetMousePosition() const
	{
		return myPosition;
	}

	void Cursor::ClipCursor()
	{
		if (myPosition.x < 0.f)
		{
			myPosition.x = 0.f;
		}
		else if (myPosition.x > myWindowSize.x)
		{
			myPosition.x = myWindowSize.x;
		}

		if (myPosition.y < 0.f)
		{
			myPosition.y = 0.f;
		}
		else if (myPosition.y > myWindowSize.y)
		{
			myPosition.y = myWindowSize.y;
		}
	}
}