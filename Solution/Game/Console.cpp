#include "stdafx.h"
#include "Console.h"
#include <InputWrapper.h>

Console::Console()
{
}


Console::~Console()
{
}

void Console::Update()
{
	ReadInput();

	DEBUG_PRINT(myInput);
}

void Console::ReadInput()
{
	CU::InputWrapper* input = CU::InputWrapper::GetInstance();

	bool shift = input->KeyIsPressed(DIK_LSHIFT) || input->KeyIsPressed(DIK_RSHIFT);

	myInput += input->KeyDown(DIK_A) ? shift ? "A" : "a" : "";
	myInput += input->KeyDown(DIK_B) ? shift ? "B" : "b" : "";
	myInput += input->KeyDown(DIK_C) ? shift ? "C" : "c" : "";
	myInput += input->KeyDown(DIK_D) ? shift ? "D" : "d" : "";
	myInput += input->KeyDown(DIK_E) ? shift ? "E" : "e" : "";
	myInput += input->KeyDown(DIK_F) ? shift ? "F" : "f" : "";
	myInput += input->KeyDown(DIK_G) ? shift ? "G" : "g" : "";
	myInput += input->KeyDown(DIK_H) ? shift ? "H" : "h" : "";
	myInput += input->KeyDown(DIK_I) ? shift ? "I" : "i" : "";
	myInput += input->KeyDown(DIK_J) ? shift ? "J" : "j" : "";
	myInput += input->KeyDown(DIK_K) ? shift ? "K" : "k" : "";
	myInput += input->KeyDown(DIK_L) ? shift ? "L" : "l" : "";
	myInput += input->KeyDown(DIK_M) ? shift ? "M" : "m" : "";
	myInput += input->KeyDown(DIK_N) ? shift ? "N" : "n" : "";
	myInput += input->KeyDown(DIK_O) ? shift ? "O" : "o" : "";
	myInput += input->KeyDown(DIK_P) ? shift ? "P" : "p" : "";
	myInput += input->KeyDown(DIK_Q) ? shift ? "Q" : "q" : "";
	myInput += input->KeyDown(DIK_R) ? shift ? "R" : "r" : "";
	myInput += input->KeyDown(DIK_S) ? shift ? "S" : "s" : "";
	myInput += input->KeyDown(DIK_T) ? shift ? "T" : "t" : "";
	myInput += input->KeyDown(DIK_U) ? shift ? "U" : "u" : "";
	myInput += input->KeyDown(DIK_V) ? shift ? "V" : "v" : "";
	myInput += input->KeyDown(DIK_W) ? shift ? "W" : "w" : "";
	myInput += input->KeyDown(DIK_X) ? shift ? "X" : "x" : "";
	myInput += input->KeyDown(DIK_Y) ? shift ? "Y" : "y" : "";
	myInput += input->KeyDown(DIK_Z) ? shift ? "Z" : "z" : "";

	if (input->KeyDown(DIK_BACKSPACE) && myInput.size() > 0)
	{
		myInput.erase(myInput.end() - 1, myInput.end());
	}

	if (input->KeyDown(DIK_SPACE))
	{
		myInput += " ";
	}

	if (input->KeyDown(DIK_TAB) && myInput.size() > 0)
	{
		//myInput = mySuggestion;
	}
}

