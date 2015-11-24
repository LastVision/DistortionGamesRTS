#pragma once
class Level
{
public:
	Level();
	~Level();

	bool LogicUpdate(float aDeltaTime);
	void Render(bool aIsActiveState);

	void OnResize(int aWidth, int aHeigth);
};

