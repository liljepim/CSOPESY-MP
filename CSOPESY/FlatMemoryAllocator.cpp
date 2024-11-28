#pragma once
#include <unordered_map>
#include <Windows.h>
#include "FlatMemoryAllocator.h"
#include "Typedef.h"
#include "Scheduler.h"

FlatMemoryAllocator* FlatMemoryAllocator::sharedInstance = nullptr;

FlatMemoryAllocator* FlatMemoryAllocator::getInstance()
{
	return sharedInstance;
}

void FlatMemoryAllocator::initialize()
{
	sharedInstance = new FlatMemoryAllocator();
}

void FlatMemoryAllocator::destroy()
{
	delete sharedInstance;
}

//alloc
void FlatMemoryAllocator::allocate(std::shared_ptr<Process> newProcess)
{
	int emptySpaceCount;
	bool canAllocate = false;
	int finalIndex = -1;
	for(int i = 0; i < Scheduler::getInstance()->configVars["max-overall-mem"] - newProcess->requiredMem + 1; i++)
	{
		emptySpaceCount = 0;
		for(int j = i; j < i + newProcess->requiredMem; j++)
		{
			if(memoryMap[j] == -1)
				emptySpaceCount++;
		}
		if(emptySpaceCount == newProcess->requiredMem)
		{
			canAllocate = true;
			finalIndex = i;
			break;
		}
	}
	if(canAllocate)
	{
		std::fill(memoryMap.begin() + finalIndex, memoryMap.begin() + finalIndex + newProcess->requiredMem, newProcess->processId);
		newProcess->memIndex = finalIndex;
	}
}

//dealloc
void FlatMemoryAllocator::deallocate(std::shared_ptr<Process> newProcess)
{
	std::fill(memoryMap.begin() + newProcess->memIndex, memoryMap.begin() + newProcess->memIndex + newProcess->requiredMem, -1);	
	newProcess->memIndex = -1;
}

//backing store

//visualization
// - memory in use count
// - processes (name and bytes occupied) in memory
// - fragmentation

FlatMemoryAllocator::FlatMemoryAllocator()
{
	int cellCount = Scheduler::getInstance()->configVars["max-overall-mem"];
	for(int i = 0; i < cellCount; i++)
		this->memoryMap.push_back(-1);
}