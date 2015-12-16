#pragma once
#include <Vector.h>

namespace GUI
{
	class Widget
	{
	public:
		Widget();
		virtual ~Widget();
		virtual void Destroy();

		virtual void Update();
		virtual void Render(const CU::Vector2<float>& aParentPosition);
		virtual void OnMouseDown(const CU::Vector2<float>& aPosition);
		virtual void OnMousePressed(const CU::Vector2<float>& aPosition);
		virtual void OnRightMousePressed(const CU::Vector2<float>& aPosition);
		virtual void OnMouseUp();
		virtual void OnMouseEnter();
		virtual void OnMouseExit();
		virtual void OnMouseMoved(const CU::Vector2<float>& aPosition);

		virtual Widget*	MouseIsOver(const CU::Vector2<float>& aPosition);

		virtual bool IsInside(const CU::Vector2<float>& aPosition) const;

		virtual void OnResize(const CU::Vector2<float>& aNewWindowSize, const CU::Vector2<float>& anOldWindowSize);

		virtual inline void SetPosition(const CU::Vector2<float>& aPosition);
		virtual inline void SetVisibility(bool aVisibility);
		inline const CU::Vector2<float>& GetSize() const;
		inline const CU::Vector2<float>& GetPosition() const;
		inline bool IsVisible() const;

		const std::string& GetHoverText() const;

	protected:
		CU::Vector2<float> myPosition;
		CU::Vector2<float> mySize;
		std::string myHoverText;
		bool myIsVisible;
	};

	inline void Widget::SetPosition(const CU::Vector2<float>& aPosition)
	{
		myPosition = aPosition;
	}

	inline void Widget::SetVisibility(bool aVisibility)
	{
		myIsVisible = aVisibility;
	}

	inline const CU::Vector2<float>& Widget::GetSize() const
	{
		return mySize;
	}

	inline const CU::Vector2<float>& Widget::GetPosition() const
	{
		return myPosition;
	}

	inline bool Widget::IsVisible() const
	{
		return myIsVisible;
	}

	inline const std::string& Widget::GetHoverText() const
	{
		return myHoverText;
	}
}