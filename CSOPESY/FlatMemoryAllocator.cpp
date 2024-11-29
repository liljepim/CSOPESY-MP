#pragma once
#include <unordered_map>
#include <tuple>
#include <Windows.h>
#include <algorithm>
#include "FlatMemoryAllocator.h"
#include "Typedef.h"
#include "Scheduler.h"
#include "ConsoleManager.h"
#include <fstream>
#include <mutex> 

extern unsigned int cpuCycle;
extern std::mutex mtx;

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
	bool canAllocate;
	int finalIndex;
	canAllocate = FlatMemoryAllocator::canAllocate(newProcess, &finalIndex);
	if(canAllocate)
	{
		std::fill(memoryMap.begin() + finalIndex, memoryMap.begin() + finalIndex + newProcess->requiredMem, newProcess);
		newProcess->memIndex = finalIndex;
		newProcess->cycleAssigned = cpuCycle;
		processesStored.push_back(newProcess);
		availableMem -= newProcess->requiredMem;
		occupiedMem += newProcess->requiredMem;
	}
}

bool FlatMemoryAllocator::canAllocate(std::shared_ptr<Process> newProcess, int *finalIndexPtr)
{
	int emptySpaceCount = 0;
	bool canAllocate = false;
	int finalIndex = -1;
	for(int i = 0; i < maxOverallMem; i++)
	{
		if(memoryMap[i] != nullptr)
			emptySpaceCount = 0;
		else
			emptySpaceCount += 1;
		if(emptySpaceCount == newProcess->requiredMem)
		{
			canAllocate = true;
			finalIndex = i - newProcess->requiredMem + 1;
			break;
		}
	}
	*finalIndexPtr = finalIndex;
	return canAllocate;
}

void FlatMemoryAllocator::genstat()
{
	std::cout << "\nTotal Memory : " << this->maxOverallMem << " KB\n";
	std::cout << "Used Memory : " << this->occupiedMem << " KB\n";
	std::cout << "Free Memory : " << this->availableMem << " KB\n";
}

//dealloc
void FlatMemoryAllocator::deallocate(std::shared_ptr<Process> newProcess)
{
	std::fill(memoryMap.begin() + newProcess->memIndex, memoryMap.begin() + newProcess->memIndex + newProcess->requiredMem, nullptr);	
	newProcess->memIndex = -1;
	for(int i = 0; i < processesStored.size(); i++)
	{
		if(processesStored[i] == newProcess)
		{
			processesStored.erase(processesStored.begin() + i);
			availableMem += newProcess->requiredMem;
			occupiedMem -= newProcess->requiredMem;
			break;
		}
	}

}

void FlatMemoryAllocator::generateSummary()
{
	std::cout << "Memory Usage: " << this->occupiedMem << " / " << this->maxOverallMem;
	std::cout << "\nMemory Util: " << ((this->occupiedMem * 100.f) / this->maxOverallMem) << "%\n";
	std::cout << "\n";
	std::cout << "\n=========================================================\n";
	std::cout << "Running processes and memory usage:";
	std::cout << "\n---------------------------------------------------------\n";
	for(auto i : processesStored)
	{
		if(i->isRunning)
		{
			std::cout << i->name << " " << i->requiredMem << " KB " << std::endl;
		}
	}
	std::cout << "\n---------------------------------------------------------\n";
}


void FlatMemoryAllocator::backingStore(std::shared_ptr<Process> newProcess)
{
	deallocate(newProcess);
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

//visualization
// - memory in use count
// - processes (name and bytes occupied) in memory
// - fragmentation
void FlatMemoryAllocator::visualization()
{
	String memoryString = "";
	for(int i = 0; i < memoryMap.size(); i++)
	{
		if(memoryMap[i] == nullptr)
			memoryString += '.';
		else
			memoryString += '*';
	}
	std::cout << memoryString;
}

//fcfs swap criteria
//oldest
//can fit
//idle
//wait otherwise
void FlatMemoryAllocator::swapBackingStore(std::shared_ptr<Process> newProcess)
{
	std::vector<std::shared_ptr<Process>> eligibleProcesses;
	std::shared_ptr<Process> oldestProcess;
	for(int i = 0; i < processesStored.size(); i++)
	{
		if(!processesStored[i]->isRunning && processesStored[i]->requiredMem >= newProcess->requiredMem)
			eligibleProcesses.push_back(processesStored[i]);
	}
	if(eligibleProcesses.size() > 0)
	{
		oldestProcess = eligibleProcesses[0];
		for(int i = 0; i < eligibleProcesses.size(); i++)
		{
			if(eligibleProcesses[i]->cycleAssigned < oldestProcess->cycleAssigned)
				oldestProcess = eligibleProcesses[i];
		}

		backingStore(oldestProcess);
		std::fill(memoryMap.begin() + oldestProcess->memIndex, memoryMap.begin() + oldestProcess->memIndex + newProcess->requiredMem, newProcess);
		newProcess->memIndex = oldestProcess->memIndex;
		newProcess->cycleAssigned = cpuCycle;
	}
}

FlatMemoryAllocator::FlatMemoryAllocator()
{
	int cellCount = Scheduler::getInstance()->configVars["max-overall-mem"];
	for(int i = 0; i < cellCount; i++)
		this->memoryMap.push_back(nullptr);
	maxOverallMem = Scheduler::getInstance()->configVars["max-overall-mem"];
	availableMem = maxOverallMem;
	occupiedMem = 0;
}