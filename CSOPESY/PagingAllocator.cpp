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
	Scheduler::getInstance()->numPagein += 1;
	if(freeIndices.size() == newProcess->requiredFrames)
	{
		for(int i = 0; i < newProcess->requiredFrames; i++)
		{
			availableMem -= memperf;
			occupiedMem += memperf;
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
		if(memoryMap[freeFrames[i]] == nullptr)
			freeIndices.push_back(freeFrames[i]);
		if(freeIndices.size() == newProcess->requiredFrames)
		{
			return freeIndices;
		}
	}
	return std::vector<int>();
}

void PagingAllocator::generateSummary()
{
	int usedTotal = 0;
	std::vector<std::shared_ptr<Process>> tempProc;
	if (!memoryMap.empty())
	{
		for (auto i : memoryMap)
		{
			if (i != nullptr)
			{
				if (!(std::find(tempProc.begin(), tempProc.end(), i) != tempProc.end()) || tempProc.empty())
				{
					usedTotal += i->requiredMem;
					tempProc.push_back(i);
				}
			}
		}
	}
	std::cout << "Memory Usage: " << usedTotal << " / " << this->totalMem;
	std::cout << "\nMemory Util: " << ((usedTotal * 100.f) / this->totalMem) << "%\n";
	std::cout << "\n";
	std::cout << "\n=========================================================\n";
	std::cout << "Running processes and memory usage:";
	std::cout << "\n---------------------------------------------------------\n";
	
	for( auto i : tempProc)
	{
		std::cout << i->name << " " << i->requiredMem << " KB " << std::endl;
	}
	
	std::cout << "\n---------------------------------------------------------\n";
}

void PagingAllocator::genstat()
{
	std::cout << "\nTotal Memory : " << this->totalMem << " KB\n";
	std::cout << "Used Memory : " << this->occupiedMem << " KB\n";
	std::cout << "Free Memory : " << this->availableMem << " KB\n";
}

void PagingAllocator::pageOut(std::shared_ptr<Process> newProcess)
{
	int pageOutCount = 0;
	Scheduler::getInstance()->numPageout += 1;
	newProcess->assignedFrames.clear();
	for(int i = 0 ; i < freeFrames.size(); i++)
	{
		if(memoryMap[freeFrames[i]] == newProcess)
		{
			occupiedMem -= memperf;
			availableMem += memperf;
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
	storeFile.open(subfolder + '/' + std::to_string(oldestPointer->processId) + ".txt");
	storeFile << oldestPointer->timeCreated << std::endl;
	storeFile << oldestPointer->lastExecuted << std::endl;
	storeFile << oldestPointer->timeFinished << std::endl;
	storeFile << oldestPointer->coreUsed << std::endl;
	storeFile << oldestPointer->requiredMem << std::endl;
	storeFile << oldestPointer->requiredFrames << std::endl;
	storeFile << oldestPointer->memIndex << std::endl;
	storeFile << oldestPointer->cycleAssigned << std::endl;
	storeFile << oldestPointer->isRunning << std::endl;
	storeFile.close();
}

PagingAllocator::PagingAllocator()
{
	cellCount = Scheduler::getInstance()->configVars["max-overall-mem"] / Scheduler::getInstance()->configVars["mem-per-frame"];
	memperf = Scheduler::getInstance()->configVars["mem-per-frame"];
	totalMem = memperf * cellCount;
	occupiedMem = 0;
	availableMem = totalMem;
	for(int i = 0; i < cellCount; i++)
	{
		this->memoryMap.push_back(nullptr);
		this->freeFrames.push_back(i);
	}
	std::random_device rd;
    std::mt19937 g(rd());
	std::shuffle(&freeFrames[0], &freeFrames[cellCount - 1], g);
}