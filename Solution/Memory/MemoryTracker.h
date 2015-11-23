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
			: myFileName(""), myFunctionName(""), myLine(-1), myType(eMemoryType::UNKNOWN) {}

		void* myAddress;
		size_t myBytes;
		const char* myFileName;
		const char* myFunctionName;
		int myLine;
		eMemoryType myType;
	};

	class MemoryTracker
	{
		friend void* ::operator new(size_t aBytes);
		friend void* ::operator new[](size_t aBytes);
		friend void ::operator delete(void* aAddress);
		friend void ::operator delete[](void* aAddress);
	public:
		static void Destroy();
		static MemoryTracker* GetInstance();

		void Allocate(int aLine, const char* aFile, const char* aFunction);
		void Free(void* aAddress);

	private:
		static void Create();
		MemoryTracker();
		~MemoryTracker();

		void Add(void* aAddress, size_t aBytes, eMemoryType aMemoryType);
		void Remove(void* aAddress, bool aLock = true);

		MemoryStatistics myMemoryStatistics;
		MemoryData myTopicalData;
		MemoryData* myData;
		int myAllocations;


		static MemoryTracker* myInstance;
	};
}