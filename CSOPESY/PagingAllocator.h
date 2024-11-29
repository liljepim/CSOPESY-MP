#pragma once
#include <Windows.h>
#include "Process.h"
#include "Scheduler.h"

class PagingAllocator
{
public:
	static PagingAllocator* getInstance();
	static void initialize();
	static void destroy();
	void pageIn(std::shared_ptr<Process> newProcess);
	void pageOut(std::shared_ptr<Process> newProcess);
	void storeOldest();
	std::vector<int> canAllocate(std::shared_ptr<Process> newProcess);
	std::vector<std::shared_ptr<Process>> memoryMap;
	std::vector<int> freeFrames;
	void generateSummary();
	void genstat();
	int cellCount = 0;
	int totalMem;
	int occupiedMem;
	int availableMem;
	int memperf;
private:
	static PagingAllocator* sharedInstance;
	PagingAllocator();
	~PagingAllocator() = default;
	PagingAllocator(PagingAllocator const&) {};
};

