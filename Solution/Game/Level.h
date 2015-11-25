#pragma once

namespace Prism
{
	class Camera;
	class Terrain;
}

class Level
{
public:
	Level();
	~Level();

	bool LogicUpdate(float aDeltaTime);
	void Render(bool aIsActiveState, const Prism::Camera& aCamera);

	void OnResize(int aWidth, int aHeigth);

private:
	Prism::Terrain* myTerrain;
};

