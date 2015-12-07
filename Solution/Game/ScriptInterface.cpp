#include "stdafx.h"
#include "ScriptInterface.h"

#include <LUACinematicMessage.h>
#include <LUAMoveCameraMessage.h>
#include <PostMaster.h>
#include <ScriptSystem.h>
#include <ToggleGUIMessage.h>

namespace Script_Interface
{
	int Print(lua_State* aState) //std::string aString
	{
		std::string stringArg = lua_tostring(aState, 1);

		DEBUG_PRINT_LUA(stringArg);

		return 0;
	}

	int MoveCamera(lua_State* aState) //int aX, int aY, int aDistance
	{
		float x = float(lua_tonumber(aState, 1));
		float y = float(lua_tonumber(aState, 2));
		float distance = float(lua_tonumber(aState, 3));


		CU::Vector2<float> moveAmount = CU::Vector2<float>(x, y);
		CU::Normalize(moveAmount);
		moveAmount *= distance;

		PostMaster::GetInstance()->SendMessage(LUAMoveCameraMessage(moveAmount));

		return 0;
	}

	int RenderBox(lua_State* aState) //int aCenterX, int aCenterY, int aCenterZ, int aSize, int aColor, bool aWireframe
	{
		float x = float(lua_tonumber(aState, 1));
		float y = float(lua_tonumber(aState, 2));
		float z = float(lua_tonumber(aState, 3));

		float size = float(lua_tonumber(aState, 4));
		int color = int(lua_tonumber(aState, 5));
		bool wireframe = lua_toboolean(aState, 6);

		Prism::RenderBox({ x, y, z }, static_cast<eColorDebug>(color), size, wireframe);

		return 0;
	}
	
	int RenderLine(lua_State* aState) //int aStartX, int aStartY, int aStartZ, int aEndX, int aEndY, int aEndZ, int aColor
	{
		float x1 = float(lua_tonumber(aState, 1));
		float y1 = float(lua_tonumber(aState, 2));
		float z1 = float(lua_tonumber(aState, 3));

		float x2 = float(lua_tonumber(aState, 4));
		float y2 = float(lua_tonumber(aState, 5));
		float z2 = float(lua_tonumber(aState, 6));

		int color = int(lua_tonumber(aState, 7));

		Prism::RenderLine3D({ x1, y1, z1 }, { x2, y2, z2 }, static_cast<eColorDebug>(color));

		return 0;
	}

	int StartCinematic(lua_State* aState)//int aCinematicIndex
	{
		int cinematicIndex = int(lua_tonumber(aState, 1));
		PostMaster::GetInstance()->SendMessage(LUACinematicMessage(cinematicIndex, eCinematicAction::START));

		return 0;
	}

	int EndCinematic(lua_State*)//void
	{
		PostMaster::GetInstance()->SendMessage(LUACinematicMessage(0, eCinematicAction::END));
		return 0;
	}

	int ShowGUI(lua_State* aState)//void
	{
		float fadeTime = float(lua_tonumber(aState, 1));
		PostMaster::GetInstance()->SendMessage(ToggleGUIMessage(true, fadeTime));
		return 0;
	}

	int HideGUI(lua_State* aState)//void
	{
		float fadeTime = float(lua_tonumber(aState, 1));
		PostMaster::GetInstance()->SendMessage(ToggleGUIMessage(false, fadeTime));
		return 0;
	}
}

void ScriptInterface::RegisterFunctions()
{
	LUA::ScriptSystem* system = LUA::ScriptSystem::GetInstance();
	system->RegisterFunction("Print", Script_Interface::Print, "aString", "Prints stuff to the screen");
	system->RegisterFunction("MoveCamera", Script_Interface::MoveCamera, "aXDir, aZDir, aDistance"
		, "Moves the Camera InGame\n\taXDir and aZDir will get normalized in the function\n\taDistance needs to be muliplied with DeltaTime on the LUA-Side");
	system->RegisterFunction("RenderBox", Script_Interface::RenderBox, "aX, aY, aZ, aSize, aColor, aWireframe"
		, "Draws a Debugbox centered on (aX, aY, aZ) with HalfWidth(aSize).\n\tAvailable colors are:\n\t0: White\n\t1: Black\n\t2: Red\n\t3: Green\n\t4: Blue\n\t5: Pink\n\t6: Yellow");
	system->RegisterFunction("RenderLine", Script_Interface::RenderLine, "aStartX, aStartY, aStartZ, aEndX, aEndY, aEndZ, aColor"
		, "Draws a DebugLine:\n\t\t from: (startX, startY, startZ)\n\t\t to: (endX, endY, endZ).\n\tAvailable colors are:\n\t0: White\n\t1: Black\n\t2: Red\n\t3: Green\n\t4: Blue\n\t5: Pink\n\t6: Yellow");
	system->RegisterFunction("StartCinematic", Script_Interface::StartCinematic, "aCinematicIndex", "Starts a Cinematic with the given Index");
	system->RegisterFunction("EndCinematic", Script_Interface::EndCinematic, "void", "Stops any running Cinematic and returns to normal game-mode");
	system->RegisterFunction("ShowGUI", Script_Interface::ShowGUI, "aFadeTime", "Shows the GUI with a fade-in");
	system->RegisterFunction("HideGUI", Script_Interface::HideGUI, "aFadeTime", "Hides the GUI with a fade-out");
}
