#include <cstdlib>
#include <DL_Assert.h>
#include "MemoryTracker.h"
#include <sstream>

void* operator new(size_t aBytes)
{
	if (Prism::MemoryTracker::GetInstance()->myRuntime == true)
	{
		Prism::MemoryTracker::GetInstance()->myRuntime = false;
		DL_ASSERT_VA("Tried to NEW in runtime");
	}

	void* address = ::malloc(aBytes);
	Prism::MemoryTracker::GetInstance()->Add(address, aBytes, Prism::eMemoryType::NEW);
	return address;
}

void* operator new[](size_t aBytes)
{
	if (Prism::MemoryTracker::GetInstance()->myRuntime == true)
	{
		Prism::MemoryTracker::GetInstance()->myRuntime = false;
		DL_ASSERT_VA("Tried to NEW[] in runtime");
	}

	void* address = ::malloc(aBytes);
	Prism::MemoryTracker::GetInstance()->Add(address, aBytes, Prism::eMemoryType::NEW_ARRAY);
	return address;
}

void operator delete(void* aAddress) throw()
{
	Prism::MemoryTracker::GetInstance()->Remove(aAddress);
}

void operator delete[](void* aAddress) throw()
{
	Prism::MemoryTracker::GetInstance()->Remove(aAddress);
}

namespace Prism
{
	MemoryTracker* MemoryTracker::myInstance = nullptr;

	void MemoryTracker::Create()
	{
		myInstance = new MemoryTracker();
	}

	void MemoryTracker::Destroy()
	{
		delete myInstance;
	}

	MemoryTracker* MemoryTracker::GetInstance()
	{
		if (myInstance == nullptr)
		{
			//Stackoverflow
			myInstance = static_cast<MemoryTracker*>(malloc(sizeof(MemoryTracker)));
			myInstance = new(myInstance)MemoryTracker();
		}

		return myInstance;
	}

	void MemoryTracker::Allocate(int aLine, const char* aFile, const char* aFunction)
	{
		myTopicalData.myLine = aLine;
		myTopicalData.myFileName = aFile;
		myTopicalData.myFunctionName = aFunction;

		if (myTopicalData.myFileName == nullptr)
		{
			myTopicalData.myFileName = "Unknown";
		}

		if (myTopicalData.myFunctionName == nullptr)
		{
			myTopicalData.myFunctionName = "Unknown";
		}
	}

	void MemoryTracker::Free(void* aAddress)
	{
		Remove(aAddress);
	}

	void MemoryTracker::SetRunTime(bool aStatus)
	{
		myRuntime = aStatus;
	}


	void MemoryTracker::Add(void* aAddress, size_t aBytes, eMemoryType aMemoryType)
	{
		if (myTopicalData.myLine < 1)
		{
			myTopicalData.myFileName = "";
			myTopicalData.myFunctionName = "";
			myTopicalData.myLine = -1;
			myTopicalData.myType = eMemoryType::UNKNOWN;
			return;
		}

		myMemoryStatistics.myAccumulatedMemoryAllocated += aBytes;
		myMemoryStatistics.myCurrentMemoryAllocated += aBytes;
		++myMemoryStatistics.myAccumulatedNumberOfAllocations;

		//Mutex.Lock()

		myTopicalData.myAddress = aAddress;
		myTopicalData.myBytes = aBytes;
		myTopicalData.myType = aMemoryType;

		++myAllocations;

		MemoryData* newData = static_cast<MemoryData*>(realloc(myData, myAllocations * sizeof(MemoryData)));
		if (newData == nullptr)
		{
			--myAllocations;
			return;
		}

		myData = newData;
		myData[myAllocations - 1] = myTopicalData;

		myTopicalData.myFileName = "";
		myTopicalData.myFunctionName = "";
		myTopicalData.myLine = -1;
		myTopicalData.myType = eMemoryType::UNKNOWN;

		//Mutex.Unlock();
	}

	void MemoryTracker::Remove(void* aAddress, bool aLock)
	{
		if (aLock == true)
		{
			//Mutex.Lock
		}

		for (int i = 0; i < myAllocations; ++i)
		{
			if (myData[i].myAddress == aAddress)
			{
				--myAllocations;
				myMemoryStatistics.myCurrentMemoryAllocated -= myData[i].myBytes;
				myData[i] = myData[myAllocations];
				break;
			}
		}



		if (aLock == true)
		{
			//Mutex.Unlock()
		}
	}

	MemoryTracker::MemoryTracker()
		: myData(nullptr)
		, myAllocations(0)
		, myRuntime(false)
	{
		myData = reinterpret_cast<MemoryData*>(::malloc(sizeof(MemoryData)));
	}


	MemoryTracker::~MemoryTracker()
	{
		if (myAllocations > 0)
		{
			DL_PRINT_VA("--- MEMORY LEAKS ---\n");
			DL_PRINT_VA("Bytes:\tLine:\tFile:\t\t\t\t\t\tFunction:");

			const char* first;
			for (int i = 0; i < myAllocations; ++i)
			{

				const char* shortPath = strstr(myData[i].myFileName, "solution");
				shortPath += 9;

				DL_PRINT_VA("%i\t\t%i\t\t%s\t\t%s", myData[i].myBytes, myData[i].myLine, shortPath, myData[i].myFunctionName);
			}

			const char* shortPath = strstr(myData[0].myFileName, "solution");
			shortPath += 9;

			DL_ASSERT_VA("\nMEMORYLEAK!\nFile: %s\nFunction: %s\nLine: %i\n\nTotal Leaks: %i"
				, shortPath, myData[0].myFunctionName, myData[0].myLine, myAllocations);
			::free(myData);
		}
	}
}