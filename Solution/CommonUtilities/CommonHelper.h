#pragma once
#include <algorithm>
#include <DL_Debug.h>
#include "Matrix.h"
#include <string>
#include "Vector.h"
#include <ShlObj.h>
#include <sstream>

namespace CU
{
	inline std::string ToLower(const std::string& aString)
	{
		std::string data = aString;
		std::transform(data.begin(), data.end(), data.begin(), ::tolower);
		return data;
	}

	inline std::string ToLower(const char* aString)
	{
		std::string data = aString;
		std::transform(data.begin(), data.end(), data.begin(), ::tolower);
		return data;
	}

	inline float Clip(float aNumber, float aLower, float aUpper)
	{
		return fmax(aLower, fmin(aNumber, aUpper));
	}

	inline std::string GetSubString(std::string aStringToReadFrom, char aCharToFind, bool aReadAfterChar)
	{
		std::string toReturn;
		if (aReadAfterChar == false)
		{
			toReturn = aStringToReadFrom.substr(0, aStringToReadFrom.find(aCharToFind));
		}
		else if (aReadAfterChar == true)
		{
			toReturn = aStringToReadFrom.substr(aStringToReadFrom.find_first_of(aCharToFind) + 1);
		}

		return toReturn;
	}

	inline std::string GetSubString(const std::string& aStringToReadFrom, const std::string& aWordToFind, bool aReadAfterChar
		, int someCharsToSkip = 0)
	{
		std::string toReturn;
		if (aStringToReadFrom.rfind(aWordToFind) != std::string::npos)
		{
			if (aReadAfterChar == false)
			{
				return toReturn = aStringToReadFrom.substr(0, aStringToReadFrom.rfind(aWordToFind));
			}
			else if (aReadAfterChar == true)
			{
				return toReturn = aStringToReadFrom.substr(aStringToReadFrom.rfind(aWordToFind) + (someCharsToSkip - 1));
			}
		}
		return aStringToReadFrom;
	}
	inline std::string GetMyDocumentFolderPath()
	{
		char documents[MAX_PATH];
		HRESULT hr = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, documents);
		if (hr != S_OK) 
		{
			DL_ASSERT("Can't get document folder!");
		}
		std::stringstream ss;
		ss << documents;
		ss << "\\Distortion Games\\";
		return ss.str();
	}
	inline std::string Concatenate(const char* aFormattedString, ...)
	{
		char buffer[1024];
		va_list args;
		va_start(args, aFormattedString);
		vsprintf_s(buffer, aFormattedString, args);
		perror(buffer);
		va_end(args);

		return buffer;
	}

	inline float StringToFloat(const std::string& aString)
	{
		const char* value = aString.c_str();
		char* end;
		float floatValue = strtof(value, &end);

		DL_ASSERT_EXP(value != end, "Error reading float value: " + aString);
		return floatValue;
	}

	inline int StringToInt(const std::string& aString)
	{
		const char* value = aString.c_str();
		char* end;
		int intValue = strtol(value, &end, 10);

		DL_ASSERT_EXP(value != end, "Error reading int value: " + aString);
		return intValue;
	}


	inline Matrix44<float> GetOrientation(Matrix44<float>& aMatrixToRotate, const Vector3<float>& aRotationInDegrees)
	{
		//to radians:
		Vector3<float> rotation(aRotationInDegrees);
		rotation *= 0.0174532925f;
		
		aMatrixToRotate = Matrix44<float>::CreateRotateAroundZ(rotation.z) * aMatrixToRotate;
		aMatrixToRotate = Matrix44<float>::CreateRotateAroundY(rotation.y) * aMatrixToRotate;
		aMatrixToRotate = Matrix44<float>::CreateRotateAroundX(rotation.x) * aMatrixToRotate;

		return aMatrixToRotate;
	}

	inline bool IsValidTextureSize(UINT aValue)
	{
		return aValue == 2 || aValue == 4 || aValue == 8 || aValue == 16 || aValue == 32 || aValue == 64 || aValue == 128
			|| aValue == 256 || aValue == 512 || aValue == 1024 || aValue == 2048 || aValue == 4096 || aValue == 8192
			|| aValue == 1024 * 6;
	}
}