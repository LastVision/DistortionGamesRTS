#include "stdafx.h"
#include <CommonHelper.h>
#include "ConsoleHistoryManager.h"
#include <fstream>


ConsoleHistoryManager::ConsoleHistoryManager()
	: myCurrentIndex(0)
	, myInsertIndex(0)
{
	std::string tempPath = CU::GetMyDocumentFolderPath() + "ICE";
	CreateDirectory(tempPath.c_str(), NULL);
	std::string temp = tempPath + "\\CommandHistory.txt";
	myHistoryFile = temp.c_str();
	myHistory.Reserve(1024);
}

ConsoleHistoryManager::~ConsoleHistoryManager()
{
}

void ConsoleHistoryManager::Save()
{
	std::fstream output;
	output.open(myHistoryFile, std::ios::out);
	for (int i = 0; i < myHistory.Size(); ++i)
	{
		output << myHistory[i] << std::endl;
	}
	output.flush();
	output.close();
}

void ConsoleHistoryManager::Load()
{
	std::fstream output;
	output.open(myHistoryFile, std::ios::in);
	std::string line;
	while (std::getline(output, line))
	{
		myHistory.Add(line);
	}
	output.close();
}

const std::string& ConsoleHistoryManager::GetCurrent()
{
	return myHistory[myCurrentIndex];
}


void ConsoleHistoryManager::GetNext()
{
	myCurrentIndex++;
	if (myCurrentIndex >= myHistory.Size())
	{
		myCurrentIndex = 0;
	}
}


void ConsoleHistoryManager::GetPrevious()
{
	myCurrentIndex--;
	if (myCurrentIndex < 0)
	{
		myCurrentIndex = myHistory.Size() - 1;
	}
}

void ConsoleHistoryManager::AddHistory(const std::string& aCommand)
{
	if (myHistory.Size() >= myHistory.GetCapacity())
	{
		myInsertIndex = 0;
	}
	myHistory[myInsertIndex] = aCommand;
	myInsertIndex++;
}