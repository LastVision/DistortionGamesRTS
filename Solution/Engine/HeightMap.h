#pragma once

namespace Prism
{
	struct HeightMap
	{
		HeightMap(int aWidth, int aDepth, const unsigned char* const someData) : myWidth(aWidth), myDepth(aDepth), myData(someData){}
		~HeightMap(){ delete[] myData; }
		void operator=(HeightMap&) = delete;

		const int myWidth;
		const int myDepth;
		const unsigned char* const myData;
	};
}