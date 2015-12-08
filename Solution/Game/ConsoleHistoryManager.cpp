#include "stdafx.h"
#include <CommonHelper.h>
#include "ConsoleHistoryManager.h"
#include <fstream>


ConsoleHistoryManager::ConsoleHistoryManager()
	: myCurrentIndex(0)
	, myInsertIndex(0)
	, myHasWrapped(false)
{
	std::string tempPath = CU::GetMyDocumentFolderPath() + "ICE";
	CreateDirectory(tempPath.c_str(), NULL);
	std::string temp = tempPath + "\\CommandHistory.txt";
	myHistoryFile = temp.c_str();
	myHistory.Init(128);
}

ConsoleHistoryManager::~ConsoleHistoryManager()
{
}

void ConsoleHistoryManager::Save()
{
	std::fstream output;
	output.open(myHistoryFile, std::ios::out);
	for (int i = 1; i < myHistory.Size(); ++i)
	{
		output << int(myHistory[i].myType) << " " << myHistory[i].myHistory << std::endl;
	}
	output.flush();
	output.close();
}

void ConsoleHistoryManager::Load()
{
	myHistory.RemoveAll();

	History first;
	first.myHistory = "";
	first.myType = eHistoryType::HISTORY;
	myHistory.Add(first);

	std::fstream output;
	output.open(myHistoryFile, std::ios::in);

	std::string line;
	while (std::getline(output, line))
	{
		History temp;
		std::string message = CU::GetSubString(line, ' ', true);
		temp.myHistory = message;

		std::string identifier = CU::GetSubString(line, ' ', false);
		std::stringstream error;
		error << int(eHistoryType::ERROR);

		if (identifier == error.str().c_str())
		{
			temp.myType = eHistoryType::ERROR;
		}
		else
		{
			temp.myType = eHistoryType::HISTORY;
		}

		myHistory.Add(temp);
	}
	output.close();
}

const std::string& ConsoleHistoryManager::GetCurrent(eHistoryType aType)
{
	return myHistory[myCurrentIndex].myHistory;
}


const std::string& ConsoleHistoryManager::GetNext(eHistoryType aType)
{
	myCurrentIndex++;
	if (myCurrentIndex >= myHistory.Size())
	{
		myCurrentIndex = 0;
	}
	CheckType(aType, false);
	return myHistory[myCurrentIndex].myHistory;
}

const std::string& ConsoleHistoryManager::GetPrevious(eHistoryType aType)
{
	myCurrentIndex--;
	if (myCurrentIndex < 0)
	{
		myCurrentIndex = myHistory.Size() - 1;
	}
	CheckType(aType, true);
	return myHistory[myCurrentIndex].myHistory;
}

void ConsoleHistoryManager::CheckType(eHistoryType aType, bool aShouldGoBackwards)
{
	bool currentlyWrong = true;
	bool hasLooped = false;
	while (currentlyWrong == true)
	{
		if (myHistory[myCurrentIndex].myType != aType)
		{
			if (aShouldGoBackwards == false)
			{
				myCurrentIndex++;
				if (myCurrentIndex >= myHistory.Size() && hasLooped == false)
				{
					myCurrentIndex = 0;
					hasLooped = true;
				}
				else if (myCurrentIndex >= myHistory.Size() && hasLooped == true)
				{
					break;
				}
			}
			else if (aShouldGoBackwards == true)
			{
				myCurrentIndex--;
				if (myCurrentIndex < 0 && hasLooped == false)
				{
					myCurrentIndex = myHistory.Size() - 1;
					hasLooped = true;
				}
				else if (myCurrentIndex < 0 && hasLooped == true)
				{
					break;
				}
			}
		}
		else
		{
			break;
		}
	}
}


void ConsoleHistoryManager::AddHistory(const std::string& aCommand, eHistoryType anEnum)
{
	History tempHistory;
	tempHistory.myHistory = aCommand;
	tempHistory.myType = anEnum;
	if (myHistory.Size() >= myHistory.GetCapacity())
	{
		myInsertIndex = 1;
		myHasWrapped = true;
	}
	if (myHasWrapped == false)
	{
		myHistory.Add(tempHistory);
	}
	else if (myHasWrapped == true)
	{
		myHistory[myInsertIndex] = tempHistory;
	}
	myInsertIndex++;
}