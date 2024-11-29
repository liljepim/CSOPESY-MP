#pragma once
#include <unordered_map>
#include <tuple>
#include <Windows.h>
#include "Typedef.h"
#include "Scheduler.h"

class FlatMemoryAllocator
{
public:
	static FlatMemoryAllocator* getInstance();
	static void initialize();
	static void destroy();
	void allocate(std::shared_ptr<Process> newProcess);
	bool canAllocate(std::shared_ptr<Process> newProcess, int *finalIndexPtr);
	void deallocate(std::shared_ptr<Process> newProcess);
	void visualization();
	void swapBackingStore(std::shared_ptr<Process> newProcess);
	void backingStore(std::shared_ptr<Process> newProcess);
	std::vector<std::shared_ptr<Process>> memoryMap;
	std::vector<std::shared_ptr<Process>> processesStored;
private:
	static FlatMemoryAllocator* sharedInstance;
	FlatMemoryAllocator();
	~FlatMemoryAllocator() = default;
	FlatMemoryAllocator(FlatMemoryAllocator const&) {};
	int maxOverallMem;
	int availableMem;
	int occupiedMem;
};
