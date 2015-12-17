#pragma once
#include "Widget.h"

namespace Prism
{
	class SpriteProxy;
	class Camera;
}

namespace tinyxml2
{
	class XMLElement;
}

class XMLReader;

namespace GUI
{
	class MiniMapWidget : public Widget
	{

	public:
		MiniMapWidget(XMLReader* aReader, tinyxml2::XMLElement* anXMLElement, const Prism::Camera* aCamera
			, const bool& aCantClickOn);
		~MiniMapWidget();

		void Render(const CU::Vector2<float>& aParentPosition) override;
		void OnMousePressed(const CU::Vector2<float>& aPosition) override;
		void OnRightMousePressed(const CU::Vector2<float>& aPosition) override;
		void OnResize(const CU::Vector2<float>& aNewWindowSize, const CU::Vector2<float>& anOldWindowSize) override;

	private:
		void operator=(MiniMapWidget&) = delete;

		void RenderUnits(const CU::Vector2<float>& aParentPosition);
		void RenderVictoryPoints(const CU::Vector2<float>& aParentPosition);
		void RenderResourcePoints(const CU::Vector2<float>& aParentPosition);
		void RenderBases(const CU::Vector2<float>& aParentPosition);

		Prism::SpriteProxy* myPlaceholderSprite;

		Prism::SpriteProxy* myUnitSprite;
		Prism::SpriteProxy* myBaseSprite;
		Prism::SpriteProxy* myResourcePointSprite;
		Prism::SpriteProxy* myVictoryPointSprite;
		Prism::SpriteProxy* myCameraFrustum;

		const CU::Matrix44<float>* myCameraOrientation;
		const bool& myCantClickOn;

	};
}