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
	void pageOut(std::shared_ptr<Process> newProcess):
	void backingStore(std::shared_ptr<Process> newProcess);
	std::vector<int> canAllocate(std::shared_ptr<Process> newProcess);
	std::vector<std::shared_ptr<Process>> memoryMap;
	std::vector<int> freeFrames;
private:
	static PagingAllocator* sharedInstance;
	PagingAllocator();
	~PagingAllocator() = default;
	PagingAllocator(PagingAllocator const&) {};
};

