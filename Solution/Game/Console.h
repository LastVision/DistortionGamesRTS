#pragma once
#include <Subscriber.h>

class ConsoleHelp;
class ConsoleHistoryManager;
class ConsoleBackspace;

class Console : Subscriber
{
public:
	static Console* GetInstance();
	static void Destroy();

	void Update();
	const std::string& GetInput() const;
	ConsoleHistoryManager* GetConsoleHistory();
	ConsoleHelp* GetConsoleHelp();

	void ReceiveMessage(const LUARunScriptMessage& aMessage) override;

	void ClearInput();

private:
	Console();
	~Console();
	void ReadInput();

	std::string myInput;

	static Console* myInstance;
	ConsoleHelp* myHelp;
	ConsoleHistoryManager* myHistory;
	ConsoleBackspace* myBackspace;
};

inline const std::string& Console::GetInput() const
{
	return myInput;
}

inline ConsoleHistoryManager* Console::GetConsoleHistory()
{
	return myHistory;
}

inline ConsoleHelp* Console::GetConsoleHelp() 
{
	return myHelp;
}

inline void Console::ClearInput()
{
	myInput = "";
}