#include "stdafx.h"
#include <CommonHelper.h>
#include "ConsoleHistoryManager.h"
#include <fstream>
#include <Text.h>


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
	for (int i = 0; i < myHistory.Size(); ++i)
	{
		SAFE_DELETE(myHistory[i].myRenderText);
	}
}

void ConsoleHistoryManager::Save()
{
	std::fstream output;
	output.open(myHistoryFile, std::ios::out);
	for (int i = 1; i < myHistory.Size(); ++i)
	{
		output << int(myHistory[i].myType) << " " << myHistory[i].myMessage << std::endl;
	}
	output.flush();
	output.close();
}

void ConsoleHistoryManager::Load()
{
	AddHistory(" ", eHistoryType::HISTORY);

	std::fstream output;
	output.open(myHistoryFile, std::ios::in);

	std::string line;
	while (std::getline(output, line))
	{
		std::string message = CU::GetSubString(line, ' ', true);

		std::string identifier = CU::GetSubString(line, ' ', false);
		std::stringstream error;
		error << int(eHistoryType::ERROR);
		std::stringstream help;
		help << int(eHistoryType::HELP);

		eHistoryType type;
		if (identifier == error.str().c_str())
		{
			type = eHistoryType::ERROR;
		}
		else if (identifier == help.str().c_str())
		{
			type = eHistoryType::HELP;
		}
		else
		{
			type = eHistoryType::HISTORY;
		}

		AddHistory(message, type);
	}
	output.close();
}

const std::string& ConsoleHistoryManager::GetCurrent(eHistoryType aType)
{
	return myHistory[myCurrentIndex].myMessage;
}


const std::string& ConsoleHistoryManager::GetNext(eHistoryType aType)
{
	myCurrentIndex++;
	if (myCurrentIndex >= myHistory.Size())
	{
		myCurrentIndex = 0;
	}
	CheckType(aType, false);
	return myHistory[myCurrentIndex].myMessage;
}

const std::string& ConsoleHistoryManager::GetPrevious(eHistoryType aType)
{
	myCurrentIndex--;
	if (myCurrentIndex < 0)
	{
		myCurrentIndex = myHistory.Size() - 1;
	}
	CheckType(aType, true);
	return myHistory[myCurrentIndex].myMessage;
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
	tempHistory.myMessage = aCommand;
	tempHistory.myType = anEnum;

	bool prevRuntime = Prism::MemoryTracker::GetInstance()->GetRunTime();
	Prism::MemoryTracker::GetInstance()->SetRunTime(false);
	tempHistory.myRenderText = new Prism::Text(*Prism::Engine::GetInstance()->GetFont(Prism::eFont::CONSOLE));
	Prism::MemoryTracker::GetInstance()->SetRunTime(prevRuntime);
	tempHistory.myRenderText->SetText(tempHistory.myMessage);

	switch (anEnum)
	{
	case eHistoryType::ERROR:
		tempHistory.myRenderText->SetColor({ 1.f, 0.f, 0.f, 1.f });
		break;
	case eHistoryType::HISTORY:
		tempHistory.myRenderText->SetColor({ 1.f, 1.f, 1.f, 1.f });
		break;
	case eHistoryType::HELP:
		tempHistory.myRenderText->SetColor({ 0.f, 1.f, 0.f, 1.f });
		break;
	default:
		break;
	}

	if (myInsertIndex >= myHistory.GetCapacity())
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
		SAFE_DELETE(myHistory[myInsertIndex].myRenderText);
		myHistory[myInsertIndex] = tempHistory;
	}
	myInsertIndex++;
}