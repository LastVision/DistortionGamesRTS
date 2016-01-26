#pragma once
#include <Vector2.h>
#include <GameEnum.h>

namespace Prism
{
	class SpriteProxy;
}

namespace CU
{
	class InputWrapper;
}

namespace GUI
{
	class Cursor
	{
	public:
		Cursor(const CU::Vector2<int>& aWindowSize);
		~Cursor();

		void Update();
		void Render();

		const CU::Vector2<float>& GetMousePosition() const;
		const CU::Vector2<float>& GetMousePositionZeroToOne() const;
		void OnResize(int aWidth, int aHeight);

		void ClipCursor(); // for software cursor

		void SetCurrentCursor(eCursorType aType);

	private:
		CU::Vector2<float> myPosition;
		CU::Vector2<float> myPositionZeroToOne;
		CU::Vector2<float> myWindowSize;
		CU::GrowingArray<Prism::SpriteProxy*> mySprites;

		eCursorType myCurrentType;
	};

	inline void Cursor::SetCurrentCursor(eCursorType aType)
	{
		myCurrentType = aType;
	}
}