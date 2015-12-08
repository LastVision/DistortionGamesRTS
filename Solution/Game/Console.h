#pragma once

class ConsoleHistoryManager;

class Console
{
public:
	static Console* GetInstance();
	static void Destroy();

	void Update();
	const std::string& GetInput() const;

private:
	Console();
	~Console();
	void ReadInput();

	std::string myInput;

	static Console* myInstance;
	ConsoleHistoryManager* myHistory;
};

inline const std::string& Console::GetInput() const
{
	return myInput;
}
