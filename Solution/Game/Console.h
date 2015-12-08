#pragma once
class Console
{
public:
	static Console* GetInstance();
	static void Destroy();

	void Update();

private:
	Console();
	~Console();
	void ReadInput();

	std::string myInput;

	static Console* myInstance;
};

