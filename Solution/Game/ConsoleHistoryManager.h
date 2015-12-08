#pragma once
#include <GrowingArray.h>
#undef ERROR

namespace Prism
{
	class Text;
}

enum class eHistoryType
{
	ERROR,
	HISTORY,
};

struct History
{
	eHistoryType myType;
	std::string myMessage;
	Prism::Text* myRenderText;
};

class ConsoleHistoryManager
{
public:
	ConsoleHistoryManager();
	~ConsoleHistoryManager();

	const std::string& GetNext(eHistoryType aType = eHistoryType::HISTORY);
	const std::string& GetPrevious(eHistoryType aType = eHistoryType::HISTORY);
	const std::string& GetCurrent(eHistoryType aType = eHistoryType::HISTORY);

	void AddHistory(const std::string& aCommand, eHistoryType anEnum = eHistoryType::HISTORY);
	void Save();
	void Load();
	const CU::GrowingArray<History>& GetHistoryArray();
private:
	void CheckType(eHistoryType aType, bool aShouldGoBackwards);


	CU::GrowingArray<History> myHistory;
	const char* myHistoryFile;
	int myCurrentIndex;
	int myInsertIndex;
	bool myHasWrapped;

};

inline const CU::GrowingArray<History>& ConsoleHistoryManager::GetHistoryArray()
{
	return myHistory;
}