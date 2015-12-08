#pragma once
#include <GrowingArray.h>

class ConsoleHistoryManager
{
public:
	ConsoleHistoryManager();
	~ConsoleHistoryManager();

	void GetNext();
	void GetPrevious();
	const std::string& GetCurrent();
	void AddHistory(const std::string& aCommand);
	void Save();
	void Load();
	const CU::GrowingArray<std::string>& GetHistoryArray();
private:

	CU::GrowingArray<std::string> myHistory;
	const char* myHistoryFile;
	int myCurrentIndex;
	int myInsertIndex;
	bool myHasWrapped;

};

inline const CU::GrowingArray<std::string>& ConsoleHistoryManager::GetHistoryArray()
{
	return myHistory;
}