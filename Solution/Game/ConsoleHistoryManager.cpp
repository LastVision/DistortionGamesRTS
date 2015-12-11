#include "stdafx.h"
#include <CommonHelper.h>
#include "ConsoleHistoryManager.h"
#include <fstream>
#include <MathHelper.h>
#include <Text.h>


ConsoleHistoryManager::ConsoleHistoryManager()
	: myCurrentIndex(0)
	, myInsertIndex(0)
	, myHasWrapped(false)
	, myEmptyString("")
{
	std::string tempPath = CU::GetMyDocumentFolderPath() + "ICE";
	CreateDirectory(tempPath.c_str(), NULL);
	std::string temp = tempPath + "\\CommandHistory.txt";
	myHistoryFile = temp.c_str();
	myHistory.Init(256);
}

ConsoleHistoryManager::~ConsoleHistoryManager()
{
	for (int i = 0; i < myHistory.Size(); ++i)
	{
		SAFE_DELETE(myHistory[i]);
	}
}

void ConsoleHistoryManager::Save()
{
	std::fstream output;
	output.open(myHistoryFile, std::ios::out);
	for (int i = 1; i < myHistory.Size(); ++i)
	{
		output << int(myHistory[i]->myType) << " " << myHistory[i]->myMessage << std::endl;
	}
	output.flush();
	output.close();
}

void ConsoleHistoryManager::Load()
{
	//AddHistory(" ", eHistoryType::HISTORY);

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

const std::string& ConsoleHistoryManager::GetCurrent(eHistoryType)
{
	if (myHistory.Size() == 0)
	{
		return myEmptyString;
	}
	return myHistory[myCurrentIndex]->myMessage;
}


const std::string& ConsoleHistoryManager::GetNext(eHistoryType aType)
{
	if (myHistory.Size() == 0)
	{
			return myEmptyString;
	}
	myCurrentIndex++;
	if (myCurrentIndex >= myHistory.Size())
	{
		myCurrentIndex = 0;
	}
	if (CheckType(aType, false) == false)
	{
		return myEmptyString;
	}
	return myHistory[myCurrentIndex]->myMessage;
}

const std::string& ConsoleHistoryManager::GetPrevious(eHistoryType aType)
{
	if (myHistory.Size() == 0)
	{
		return myEmptyString;
	}
	myCurrentIndex--;
	if (myCurrentIndex < 0)
	{
		myCurrentIndex = myHistory.Size() - 1;
	}
	if (CheckType(aType, true) == false)
	{
		return myEmptyString;
	}
	return myHistory[myCurrentIndex]->myMessage;
}

bool ConsoleHistoryManager::IsInHistory(const std::string& aString) const
{
	if (aString == " " || aString == "")
	{
		return true;
	}
	//int index = aString.find_first_of("(");
	for (int i = 0; i < myHistory.Size(); ++i)
	{
		if (myHistory[i]->myMessage.length() < aString.length())
		{
			continue;
		}
		for (unsigned int j = 0; j < aString.length(); ++j)
		{
			if (myHistory[i]->myMessage[j] != aString[j])
			{
				break;
			}
		}
		return true;
	}
	return false;
}

bool ConsoleHistoryManager::CheckType(eHistoryType aType, bool aShouldGoBackwards)
{
	if (myHistory[myCurrentIndex]->myType == aType)
	{
		return true;
	}
	else
	{
		int aStartIndex = myCurrentIndex;
		while (myHistory[myCurrentIndex]->myType != aType)
		{
			if (aShouldGoBackwards == true)
			{
				myCurrentIndex--;
				if (myCurrentIndex < 0)
				{
					myCurrentIndex = myHistory.Size() - 1;
				}
				if (myCurrentIndex == aStartIndex)
				{
					return false;
				}
			}
			else
			{
				++myCurrentIndex;
				if (myCurrentIndex > myHistory.Size() - 1)
				{
					myCurrentIndex = 0;
				}
				if (myCurrentIndex == aStartIndex)
				{
					return false;
				}
			}
		}
	}

	return true;








	//bool currentlyWrong = true;
	//bool hasLooped = false;
	//while (currentlyWrong == true)
	//{
	//	if (myHistory[myCurrentIndex]->myType != aType)
	//	{
	//		if (aShouldGoBackwards == false)
	//		{
	//			myCurrentIndex++;
	//			if (myCurrentIndex >= myHistory.Size() && hasLooped == false)
	//			{
	//				myCurrentIndex = 0;
	//				hasLooped = true;
	//			}
	//			else if (myCurrentIndex >= myHistory.Size() && hasLooped == true)
	//			{
	//				break;
	//			}
	//		}
	//		else if (aShouldGoBackwards == true)
	//		{
	//			myCurrentIndex--;
	//			if (myCurrentIndex < 0 && hasLooped == false)
	//			{
	//				myCurrentIndex = myHistory.Size() - 1;
	//				hasLooped = true;
	//			}
	//			else if (myCurrentIndex < 0 && hasLooped == true)
	//			{
	//				break;
	//			}
	//		}
	//	}
	//	else
	//	{
	//		break;
	//	}
	//}
}


void ConsoleHistoryManager::AddHistory(const std::string& aCommand, eHistoryType anEnum)
{
	DL_ASSERT_EXP(aCommand.length() > 0, "Should not be able to save empty commands in history");
	History* tempHistory;
	
	bool prevRuntime = Prism::MemoryTracker::GetInstance()->GetRunTime();
	Prism::MemoryTracker::GetInstance()->SetRunTime(false);
	tempHistory = new History();
	tempHistory->myRenderText = new Prism::Text(*Prism::Engine::GetInstance()->GetFont(Prism::eFont::CONSOLE));
	Prism::MemoryTracker::GetInstance()->SetRunTime(prevRuntime);

	tempHistory->myMessage = aCommand;
	tempHistory->myRenderText->SetText(aCommand);
	tempHistory->myType = anEnum;
	static int numberHelpFunction = 0;
	float rbColor = 0.5f;
	switch (anEnum)
	{
	case eHistoryType::ERROR:
		tempHistory->myRenderText->SetColor({ 1.f, 0.f, 0.f, 1.f });
		break;
	case eHistoryType::HISTORY:
		tempHistory->myRenderText->SetColor({ 1.f, 1.f, 1.f, 1.f });
		break;
	case eHistoryType::HELP:
		numberHelpFunction++;
		if (numberHelpFunction % 2 == 0) rbColor = 0.1f;
		tempHistory->myRenderText->SetColor({ rbColor, 1.f, rbColor, 1.f });
		break;
	default:
		break;
	}

	if (myHistory.Size() >= myHistory.GetCapacity())
	{
	//	SAFE_DELETE(myHistory[myInsertIndex]->myRenderText);
		myHistory.DeleteNonCyclicAtIndex(0);
	}
	myHistory.Add(tempHistory);

	myInsertIndex++;
}


History::History()
{
}

History::~History()
{
	SAFE_DELETE(myRenderText);
}
