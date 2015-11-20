#include "Tracker.h"

#include <cstdlib>

void* operator new(size_t aBytes)
{
	void* address = ::malloc(aBytes);
	Prism::Tracker::GetInstance()->Add(address, aBytes, Prism::eMemoryType::NEW);
	return address;
}

namespace Prism
{
	Tracker* Tracker::myInstance = nullptr;

	void Tracker::Create()
	{
		myInstance = new Tracker();
	}

	void Tracker::Destroy()
	{
		delete myInstance;
	}

	Tracker* Tracker::GetInstance()
	{
		if (myInstance == nullptr)
		{
			//Stackoverflow
			myInstance = static_cast<Tracker*>(malloc(sizeof(Tracker)));
			*myInstance = Tracker();
			//myInstance = new Tracker();
		}

		return myInstance;
	}

	void Tracker::Allocate(unsigned int aLine, const char* aFile, const char* aFunction)
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

	void Tracker::Add(void* aAddress, size_t aBytes, eMemoryType aMemoryType)
	{
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

	Tracker::Tracker()
		: myData(nullptr)
		, myAllocations(0)
	{
		myData = reinterpret_cast<MemoryData*>(::malloc(sizeof(MemoryData)));
	}


	Tracker::~Tracker()
	{
	}
}