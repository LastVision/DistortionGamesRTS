#pragma once
class Console
{
public:
	Console();
	~Console();

	void Update();

private:
	void ReadInput();

	std::string myInput;
};

