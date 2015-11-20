#pragma once

#include "MemoryStatistics.h"

namespace Prism
{
	enum class eMemoryType
	{
		UNKNOWN,
		NEW,
		NEW_ARRAY,
		MALLOC
	};

	struct MemoryData
	{

		MemoryData()
			: myFileName("") {}

		void* myAddress;
		size_t myBytes;
		const char* myFileName;
		const char* myFunctionName;
		unsigned int myLine;
		eMemoryType myType;
	};

	class Tracker
	{
		friend void* ::operator new(size_t aBytes);
	public:
		static void Destroy();
		static Tracker* GetInstance();

		void Allocate(unsigned int aLine, const char* aFile, const char* aFunction);

	private:
		static void Create();
		Tracker();
		~Tracker();

		void Add(void* aAddress, size_t aBytes, eMemoryType aMemoryType);

		MemoryStatistics myMemoryStatistics;
		MemoryData myTopicalData;
		MemoryData* myData;
		int myAllocations;


		static Tracker* myInstance;
	};
}