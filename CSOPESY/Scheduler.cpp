#include "Scheduler.h"
#include <iostream>
#include <fstream>
#include <map>
#include <thread>
#include <windows.h>
#include <functional>
#include <ctime>
#include <mutex>
#include <format>
#include <chrono>

extern unsigned int cpuCycle;
extern bool isTesting;
unsigned int dummyCounter = 0;
extern std::mutex mtx;

/*
class IMemoryAllocator
{
public:
	virtual void* allocate(size_t size) = 0;
	virtual void deallocate(void* ptr) = 0;
};

class FlatMemoryAllocator : public IMemoryAllocator
{
public:
	FlatMemoryAllocator(size_t maximumSize)
	{
		this->maximumSize = maximumSize;
		this->allocatedSize = 0;
		this->memory.reserve(maximumSize);
		initializeMemory();
	}

	~FlatMemoryAllocator()
	{
		memory.clear();
	}

	void* allocate(size_t size) override
	{
		for(size_t i = 0; i < maximumSize - size + 1; ++i)
		{
			if(!allocationMap[i] && canAllocateAt(i, size))
			{
				allocateAt(i, size);
				return &memory[i];
			}
		}

		return nullptr;
	}

	void deallocate(void* ptr) override
	{
		size_t index = static_cast<char*>(ptr) - &memory[0];
		if(allocationMap[index])
		{
			deallocateAt(index);
		}
	}
private:
	size_t maximumSize;
	size_t allocatedSize;
	std::vector<char> memory;
	std::unordered_map<size_t, bool> allocationMap;

	void initializeMemory()
	{
		std::fill(memory.begin(), memory.end(), '.');
		std::fill(allocationMap.begin(), allocationMap.end(), false);
	}

	bool canAllocateAt(size_t index, size_t size) const
	{
		return (index + size <= maximumSize);
	}

	void allocateAt(size_t index, size_t size)
	{
		std::fill(std::next(allocationMap.begin(), index), std::next(allocationMap.begin(), index + size), true);
		allocatedSize += size;
	}

	void deallocateAt(size_t index)
	{
		allocationMap[index] = false;
	}
};
*/

Scheduler* Scheduler::sharedInstance = nullptr;

Scheduler* Scheduler::getInstance()
{
	return sharedInstance;
}

void Scheduler::initialize()
{
	sharedInstance = new Scheduler();
}

void Scheduler::readConfig()
{
	std::ifstream configFile("config.txt");
	if (!configFile.is_open())
		std::cerr << "The config file cannot be opened." << std::endl;
	String line, varName, varValue;
	int spacePos;
	while(getline(configFile, line))
	{
		spacePos = static_cast<int>(line.find(" "));
		varName = line.substr(0, spacePos);
		varValue = line.substr(spacePos + 1, line.size());
		if(varName.compare("scheduler") != 0)
			this->configVars.insert({varName, std::stoi(varValue)});
		else
			this->scheduler = varValue;
	}
	for(int i = 0; i < this->configVars["num-cpu"]; i++)
		this->coreList.push_back(-1);
	for (int i = 0; i < this->configVars["num-cpu"]; i++)
		cpuCores.push_back(std::thread());

	isOn = true;
}

void Scheduler::registerProcess(std::shared_ptr<Process> newProcess)
{
	this->readyQueue.push_back(newProcess);
}

void Scheduler::startTester()
{
	isTesting = true;
	previousBF = cpuCycle;

}

void Scheduler::stopTester()
{
	isTesting = false;
}

void Scheduler::assignProcesses()
{
	if(previousQQ == 0)
	{
		previousQQ = cpuCycle;
	}
	if(previousBF == 0)
	{
		previousBF = cpuCycle;
	}
	mtx.lock();
	for(int i = 0; i < this->configVars["num-cpu"]; i++)
	{
		if(this->coreList[i] == -1 && !readyQueue.empty())
		{
			this->coreList[i] = this->readyQueue.front()->processId;
			if(this->scheduler == "\"rr\"")
				cpuCores[i] = std::thread(&Scheduler::runProcessesRR, sharedInstance, this->readyQueue.front(), i);
			else if(this->scheduler == "\"fcfs\"")
				cpuCores[i] = std::thread(&Scheduler::runProcessesFCFS, sharedInstance, this->readyQueue.front(), i);
			this->readyQueue.erase(this->readyQueue.begin());
			cpuCores[i].detach();
		}
	}
	mtx.unlock();
	if(cpuCycle-previousQQ == (configVars["quantum-cycles"]+1))
		previousQQ = cpuCycle;
	if(isTesting)
	{
		if(cpuCycle - previousBF == (configVars["batch-process-freq"]+1))
		{
			generateDummy(ConsoleManager::getInstance());
			previousBF = cpuCycle;
		}
	}

}

void Scheduler::runProcessesRR(std::shared_ptr<Process> runningProcess, int coreIndex)
{
	unsigned int previousCycle = cpuCycle;
	unsigned int endLine;
	if(runningProcess->totalLine <= runningProcess->currentLine + this->configVars["quantum-cycles"])
		endLine = runningProcess->totalLine;
	else
		endLine = runningProcess->currentLine + this->configVars["quantum-cycles"];
	for(int i = runningProcess->currentLine; i < endLine;)
	{
		if(cpuCycle-previousCycle >= configVars["delay-per-exec"]+1)
		{
			runningProcess->coreUsed = coreIndex;
			runningProcess->currentLine += 1;
			runningProcess->lastExecuted = time(NULL);
			previousCycle = cpuCycle;
			i++;
			
		}
	}
	mtx.lock();
	runningProcess->coreUsed = -1;
	this->coreList[coreIndex] = -1;
	if(runningProcess->currentLine != runningProcess->totalLine)
		this->readyQueue.push_back(runningProcess);
	else if(runningProcess->currentLine == runningProcess->totalLine)
		finishedProcesses.push_back(runningProcess);
	mtx.unlock();
}

void Scheduler::runProcessesFCFS(std::shared_ptr<Process> runningProcess, int coreIndex)
{
	unsigned int previousCycle = cpuCycle;
	for(int i = runningProcess->currentLine; i < runningProcess->totalLine;)
	{
		if(cpuCycle-previousCycle >= configVars["delay-per-exec"]+1)
		{
			previousCycle = cpuCycle;
			runningProcess->coreUsed = coreIndex;
			runningProcess->currentLine += 1;
			runningProcess->lastExecuted = time(NULL);
			i++;
		}
		
	}
	mtx.lock();
	runningProcess->coreUsed = -1;
	this->coreList[coreIndex] = -1;
	mtx.unlock();
}

int Scheduler::getMax()
{
	return this->configVars["max-ins"];
}

int Scheduler::getMin()
{
	return this->configVars["min-ins"];
}

int Scheduler::getMemMax()
{
	return this->configVars["max-mem-per-proc"];
}

int Scheduler::getMemMin()
{
	return this->configVars["min-mem-per-proc"];
}

void Scheduler::destroy()
{
	delete sharedInstance;
}

void Scheduler::coreSummary()
{	
	int totalCount = this->configVars["num-cpu"];
	int usedCounter = 0;
	std::vector<int> dummyList;
	for(const int& i : this->coreList)
	{
		dummyList.push_back(i);
		if(i != -1)
		{
			usedCounter += 1;
		}
	}

	float cpuUtil = (usedCounter * 100.0f) / totalCount;
	std::cout << "CPU utilization: " << cpuUtil << "%\n";
	std::cout << "Cores used: " << usedCounter << std::endl;
	std::cout << "Cored available: " << totalCount - usedCounter << std::endl;

	std::cout << "\nRunning Processes:\n";
	for(const int& i : dummyList)
	{
		std::cout << i << std::endl;
	}

	std::cout << "Finished Processes:\n";
	for(std::shared_ptr<Process> i : finishedProcesses)
	{
		std::cout << i->name << std::endl;
	}

	std::cout << std::endl;	
	
 }


void Scheduler::varTest()
{
	for (auto it = this->configVars.begin(); it != this->configVars.end(); it++) 
        std::cout << "Key: " << it->first << ", Value: " << it->second << std::endl;
}

void Scheduler::generateDummy(ConsoleManager* cmInstance)
{
	mtx.lock();
	std::string tempConsoleName = "Process_" + std::to_string(dummyCounter);
	std::shared_ptr<BaseConsole> tempConsole = std::make_shared<BaseConsole>
		(tempConsoleName);
	this->registerProcess(tempConsole->getAttachedProcess());
	cmInstance->registerDummy(tempConsole);
	dummyCounter++;
	mtx.unlock();
}


Scheduler::Scheduler()
{
	//initialize the scheduler
}