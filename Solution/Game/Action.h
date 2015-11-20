#pragma once
class Action
{
public:
	virtual ~Action(){};

	virtual void OnEnter() = 0;
	virtual void OnExit() = 0;

	virtual bool Update() = 0;
};

