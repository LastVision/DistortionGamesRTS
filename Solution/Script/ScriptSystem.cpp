
#include <assert.h>
#include "ScriptSystem.h"
#include "LuaFiles\lualib.h"
#include "LuaFiles\lauxlib.h"

#include <Windows.h>
#include <numeric>
#include <algorithm>

#include <fstream>

int PrintFromLua(lua_State* aState)
{
	std::string stringArg = lua_tostring(aState, 1);

	LUA::ScriptSystem::GetInstance()->PrintToFile(stringArg);

	return 0;
}

namespace LUA
{
	ScriptSystem* ScriptSystem::myInstance = nullptr;

	void ScriptSystem::Create()
	{
		myInstance = new ScriptSystem();
	}

	ScriptSystem* ScriptSystem::GetInstance()
	{
		return myInstance;
	}

	void ScriptSystem::Destroy()
	{
		delete myInstance;
	}

	void ScriptSystem::Init(const std::function<void()>& aRegisterFunction)
	{
		myOutputFile.open("luaOutput.txt", std::ios::out);
		myCppRegisterFunction = aRegisterFunction;
		myLuaState = nullptr;
		myActiveFiles.push_back("Data\\Script\\main.lua");
		myFileWatcher.WatchFileChange("Data\\Script\\main.lua", std::bind(&ScriptSystem::ReInit, this, std::placeholders::_1));
		ReInit("123");
	}

	void ScriptSystem::RegisterFunction(const std::string& aNameInLua, lua_CFunction aFunction
		, const std::string& aArguments, const std::string& aHelpText)
	{
		Documentation doc;
		doc.myFunction = aNameInLua;
		doc.myArguments = aArguments;
		doc.myHelpText = aHelpText;
		myDocumentation.push_back(doc);

		lua_register(myLuaState, aNameInLua.c_str(), aFunction);
	}

	eFunctionStatus ScriptSystem::CallFunction(const std::string& aFunctionName, const LuaArguments& someArgs)
	{
		if (myArgumentsCount.find(aFunctionName) == myArgumentsCount.end())
		{
			return eFunctionStatus::NOT_FOUND;
			//assert(false && "Failed to find LUA-Function");
		}
		else if (myArgumentsCount[aFunctionName] != someArgs.myCount)
		{
			return eFunctionStatus::INVALID_ARGS;
			//assert(false && "Failed to call LUA-Function, missmatched number of arguments");
		}
		else
		{
			lua_getglobal(myLuaState, aFunctionName.c_str());

			if (lua_isnil(myLuaState, -1) == true)
			{
				lua_remove(myLuaState, -1);
				return eFunctionStatus::NOT_FOUND;
			}

			for (int i = 0; i < someArgs.myCount; ++i)
			{
				PushArg(someArgs.myArgs[i]);
			}

			CheckFunctionError(lua_pcall(myLuaState, someArgs.myCount, 0, 0));
		}

		return eFunctionStatus::OK;
	}


	void ScriptSystem::UseFile(const std::string& aFileName)
	{
		CheckError(luaL_dofile(myLuaState, aFileName.c_str()));
		myActiveFiles.push_back(aFileName);
		myFileWatcher.WatchFileChange(aFileName, std::bind(&ScriptSystem::ReInit, this, std::placeholders::_1));
	}

	void ScriptSystem::Update()
	{
		myFileWatcher.FlushChanges();
	}

	void ScriptSystem::PrintToFile(const std::string& aString)
	{
		myOutputFile << aString << std::endl;
		myOutputFile.flush();
	}

	void ScriptSystem::ReInit(const std::string&)
	{
		if (myLuaState != nullptr)
		{
			lua_close(myLuaState);
		}

		myArgumentsCount.clear();
		myDocumentation.clear();

		myLuaState = luaL_newstate();
		luaL_openlibs(myLuaState);
		myCppRegisterFunction();
		RegisterFunction("PrintToFile", PrintFromLua, "aString", "Prints message into file");

		AddLuaFunction("Update", 1);
		AddLuaFunction("UpdateCinematic", 2);

		for (unsigned int i = 0; i < myActiveFiles.size(); ++i)
		{
			CheckError(luaL_dofile(myLuaState, myActiveFiles[i].c_str()));
		}
	}

	void ScriptSystem::CheckError(int aCode)
	{
		if (aCode != 0)
		{
			MessageBox(NULL, lua_tostring(myLuaState, -1), "LUAError", MB_ICONERROR);
		}
	}

	void ScriptSystem::CheckFunctionError(int aCode)
	{
		if (aCode != 0)
		{
			std::string error = lua_tostring(myLuaState, -1);
			lua_remove(myLuaState, -1);
			int firstFnutt = error.find_first_of("'");
			int secondFnutt = error.find_first_of("'", firstFnutt + 1);

			std::string failedFunction(error.substr(firstFnutt + 1, secondFnutt - firstFnutt - 1));
			std::string suggestion = FindClosestFunction(failedFunction);

			int lineStartIndex = error.find_first_of(":");
			int lineEndIndex = error.find_first_of(":", lineStartIndex+1);
			
			std::string file(error.substr(0, lineStartIndex));
			std::string line(error.substr(lineStartIndex+1, lineEndIndex-lineStartIndex-1));

			MessageBox(NULL, std::string("In File: " + file + " at line: " + line + ",  you tried to call [" + failedFunction + "], did you mean [" + suggestion + "]").c_str(), "LUA-Error", MB_ICONERROR);
		}
		
	}

	void ScriptSystem::PushArg(const Arg& aArg)
	{
		switch (aArg.myType)
		{
		case Arg::eType::INT:
			lua_pushnumber(myLuaState, aArg.myInt);
			break;
		case Arg::eType::FLOAT:
			lua_pushnumber(myLuaState, aArg.myFloat);
			break;
		case Arg::eType::STRING:
			lua_pushstring(myLuaState, aArg.myString);
			break;
		case Arg::eType::BOOL:
			lua_pushboolean(myLuaState, aArg.myBool);
			break;
		default:
			assert(false && "Invalid LuaArg-Type");
			break;
		}
	}

	void ScriptSystem::AddLuaFunction(const std::string& aNameInLua, int aNumberOfArgs)
	{
		myArgumentsCount[aNameInLua] = aNumberOfArgs;
	}

	void ScriptSystem::PrintDocumentation()
	{
		std::fstream file;
		file.open("exposedScriptFunctions.lua", std::ios::out);

		file << "function Documentation()";
		for (unsigned int i = 0; i < myDocumentation.size(); ++i)
		{
			file << "\n\t" << myDocumentation[i].myFunction << "(" << myDocumentation[i].myArguments << ")\n";
			file << "\t--[[\n";
			file << "\t" << myDocumentation[i].myHelpText << "\n";
			file << "\t--]]\n";
		}
		file << "end";
		file.flush();
		file.close();
	}

#undef min
#undef max
	int ScriptSystem::GetLevenshteinDistance(const std::string &s1, const std::string &s2)
	{
		// To change the type this function manipulates and returns, change
		// the return type and the types of the two variables below.
		int s1len = s1.size();
		int s2len = s2.size();

		auto column_start = (decltype(s1len))1;

		auto column = new decltype(s1len)[s1len + 1];
		std::iota(column + column_start, column + s1len + 1, column_start);

		for (auto x = column_start; x <= s2len; x++) {
			column[0] = x;
			auto last_diagonal = x - column_start;
			for (auto y = column_start; y <= s1len; y++) {
				auto old_diagonal = column[y];
				auto possibilities = {
					column[y] + 1,
					column[y - 1] + 1,
					last_diagonal + (s1[y - 1] == s2[x - 1] ? 0 : 1)
				};
				column[y] = std::min(possibilities);
				last_diagonal = old_diagonal;
			}
		}
		auto result = column[s1len];
		delete[] column;
		return result;
	}

	float ScriptSystem::GetLevenshteinRatio(const std::string& aString, int aLevenshtienDistance)
	{
		if (aLevenshtienDistance == 0)
		{
			return 1.f;
		}

		return 1.f - float(aString.size()) / aLevenshtienDistance;
	}

	int ScriptSystem::GetSubstringBonus(const std::string& aInput, const std::string& aCorrectString, int aScore)
	{
		if (aInput.size() <= 1)
		{
			return 0;
		}

		if (aCorrectString.find(aInput) != std::string::npos)
		{
			return aScore;
		}


		std::string input(aInput);
		int middle = input.size() / 2;

		int subScoreOne = GetSubstringBonus(std::string(input.begin(), input.begin() + middle), aCorrectString, aScore / 2);
		int subScoreTwo = GetSubstringBonus(std::string(input.begin() + middle, input.end()), aCorrectString, aScore / 2);

		return std::max(subScoreOne, subScoreTwo);
	}

	std::string ScriptSystem::FindClosestFunction(const std::string& aInput)
	{
		int maxLevenshteinScore = 30;

		float closest = -1;
		std::string bestString;

		for (unsigned int i = 0; i < myDocumentation.size(); ++i)
		{
			const std::string& correctString = myDocumentation[i].myFunction;

			float score = 0.f;

			//If the current string contains the input-string, then we should bump up the score
			//to make it more likley to find a relevant string
			score += float(GetSubstringBonus(aInput, correctString, 5));
			/*if (correctString.find(aInput) != std::string::npos)
			{
			score += 5.f;
			}*/


			//If the LevensteinDistance is short, we should give the string a better score
			//If levDist is 0  we get full levenScore, else we get a percentage of it
			float levDist = float(GetLevenshteinDistance(aInput, correctString));
			if (levDist == 0)
			{
				score += maxLevenshteinScore;
			}
			else
			{
				float levRatio = GetLevenshteinRatio(aInput, levDist);
				levRatio = 1.f + (1.f - levRatio);
				levDist *= levRatio;
				score += (1.f / levDist) * maxLevenshteinScore;
			}

			if (score > closest)
			{
				closest = score;
				bestString = myDocumentation[i].myFunction;
			}
		}

		return bestString;
	}


	ScriptSystem::ScriptSystem()
	{
	}

	ScriptSystem::~ScriptSystem()
	{
		myOutputFile.close();
		PrintDocumentation();
	}
}