#pragma once
#include <Windows.h>
#include <algorithm>
#include <vector>
#include <random>
#include <fstream>
#include "Process.h"
#include "Scheduler.h"
#include "PagingAllocator.h"

extern unsigned int cpuCycle;

PagingAllocator* PagingAllocator::sharedInstance = nullptr;

PagingAllocator* PagingAllocator::getInstance()
{
	return sharedInstance;
}

void PagingAllocator::initialize()
{
	sharedInstance = new PagingAllocator();
}

void PagingAllocator::destroy()
{
	delete sharedInstance;
}

void PagingAllocator::pageIn(std::shared_ptr<Process> newProcess)
{
	//get index from fflist
	//check if frame is free
	std::vector<int> freeIndices = PagingAllocator::canAllocate(newProcess);
	if(freeIndices.size() == newProcess->requiredFrames)
	{
		for(int i = 0; i < newProcess->requiredFrames; i++)
		{
			memoryMap[freeIndices[i]] = newProcess;
			newProcess->assignedFrames.push_back(freeIndices[i]);
			newProcess->cycleAssigned = cpuCycle;
		}
	}
}

std::vector<int> PagingAllocator::canAllocate(std::shared_ptr<Process> newProcess)
{
	std::vector<int> freeIndices;
	for(int i = 0; i < freeFrames.size(); i++)
	{
		if(memoryMap[freeFrames[i]])
			freeIndices.push_back(freeFrames[i]);
		if(freeIndices.size() == newProcess->requiredFrames)
		{
			return freeIndices;
		}
	}
	return std::vector<int>();
}

void PagingAllocator::pageOut(std::shared_ptr<Process> newProcess)
{
	int pageOutCount = 0;
	newProcess->assignedFrames.clear();
	for(int i = 0 ; i < freeFrames.size(); i++)
	{
		if(memoryMap[freeFrames[i]] == newProcess)
		{
			memoryMap[freeFrames[i]] = nullptr;
			freeFrames.push_back(freeFrames[i]);
			freeFrames.erase(freeFrames.begin() + i);
			pageOutCount += 1;
		}
		if(pageOutCount == newProcess->requiredFrames)
			return;
	}
}

void PagingAllocator::storeOldest()
{
	//find oldest in memory
	std::shared_ptr<Process> oldestPointer = nullptr;
	for(int i = 0; i < memoryMap.size(); i++)
	{
		if(memoryMap[i] != nullptr)
		{
			if(oldestPointer == nullptr)
				oldestPointer = memoryMap[i];
			else if(oldestPointer != nullptr && memoryMap[i]->cycleAssigned < oldestPointer->cycleAssigned)
				oldestPointer = memoryMap[i];
		}
	}
	//page out
	PagingAllocator::pageOut(oldestPointer);
	//storefile
	String subfolder = "backing_store";
	std::ofstream storeFile;
	storeFile.open(subfolder + '/' + std::to_string(newProcess->processId) + ".txt");
	storeFile << newProcess->timeCreated << std::endl;
	storeFile << newProcess->lastExecuted << std::endl;
	storeFile << newProcess->timeFinished << std::endl;
	storeFile << newProcess->coreUsed << std::endl;
	storeFile << newProcess->requiredMem << std::endl;
	storeFile << newProcess->requiredFrames << std::endl;
	storeFile << newProcess->memIndex << std::endl;
	storeFile << newProcess->cycleAssigned << std::endl;
	storeFile << newProcess->isRunning << std::endl;
	storeFile.close();
}

PagingAllocator::PagingAllocator()
{
	int cellCount = Scheduler::getInstance()->configVars["max-overall-mem"] / Scheduler::getInstance()->configVars["mem-per-frame"];
	for(int i = 0; i < cellCount; i++)
	{
		this->memoryMap.push_back(nullptr);
		this->freeFrames.push_back(i);
	}
	std::random_device rd;
    std::mt19937 g(rd());
	std::shuffle(&freeFrames[0], &freeFrames[cellCount - 1], g);
}