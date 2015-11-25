#pragma once
#include <Vector2.h>

namespace Prism
{
	class Sprite;
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
		Cursor(CU::InputWrapper* anInputWrapper, const CU::Vector2<int>& aWindowSize);
		~Cursor();

		void Update();
		void Render();

		const CU::Vector2<float>& GetMousePosition() const;

		void ClipCursor(); // for software cursor

	private:
		CU::Vector2<float> myPosition;
		CU::Vector2<float> myWindowSize;
		Prism::Sprite* mySprite;
		CU::InputWrapper* myInputWrapper;
	};
}