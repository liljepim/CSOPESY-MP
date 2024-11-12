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
	std::thread assignThread(&Scheduler::assignProcesses, sharedInstance);
	assignThread.detach();
}

void Scheduler::registerProcess(std::shared_ptr<Process> newProcess)
{
	this->readyQueue.push_back(newProcess);
}

void Scheduler::assignProcesses()
{
	unsigned int previousCycle = cpuCycle;
	std::vector<std::thread> cpuCores;
	bool memIsFree = false;
	
	for(int i = 0; i < this->configVars["num-cpu"]; i++)
		cpuCores.push_back(std::thread());
	
	while(true)
	{
		mtx.lock();
		for(int i = 0; i < this->configVars["num-cpu"]; i++)
		{
			int freeIndex;
			for(freeIndex = 0; freeIndex < 4; freeIndex++)
			{
				if(this->memoryMap[freeIndex] == -1)
				{
					memIsFree = true;
					break;
				}
				else
					memIsFree = false;
			}
			if(this->coreList[i] == -1 && !readyQueue.empty())
			{
				bool inMap = false;
				for(int j = 0; j < 4; j++)
				{
					if(this->memoryMap[j] == this->readyQueue.front()->processId)
					{
						inMap = true;
						break;
					}
				}
				if(inMap)
				{
					this->coreList[i] = this->readyQueue.front()->processId;
					if(this->scheduler == "\"rr\"")
						cpuCores[i] = std::thread(&Scheduler::runProcessesRR, sharedInstance, this->readyQueue.front(), i);
					else if(this->scheduler == "\"fcfs\"")
						cpuCores[i] = std::thread(&Scheduler::runProcessesFCFS, sharedInstance, this->readyQueue.front(), i);
					this->readyQueue.erase(this->readyQueue.begin());
					cpuCores[i].detach();
				}
				else if(!inMap)
				{
					if(memIsFree)
					{
						this->coreList[i] = this->readyQueue.front()->processId;
						this->memoryMap[freeIndex] = this->readyQueue.front()->processId;
						if(this->scheduler == "\"rr\"")
							cpuCores[i] = std::thread(&Scheduler::runProcessesRR, sharedInstance, this->readyQueue.front(), i);
						else if(this->scheduler == "\"fcfs\"")
							cpuCores[i] = std::thread(&Scheduler::runProcessesFCFS, sharedInstance, this->readyQueue.front(), i);
						this->readyQueue.erase(this->readyQueue.begin());
						cpuCores[i].detach();
					}
					else
					{
						this->readyQueue.push_back(this->readyQueue.front());
						this->readyQueue.erase(this->readyQueue.begin());
					}
				}
			}
		}
		mtx.unlock();
		if(cpuCycle-previousCycle >= (configVars["quantum-cycles"]+1))
		{
			previousCycle = cpuCycle;
			std::ofstream memoryStamp;
			memoryStamp.open("memory_stamps/memory_stamp_" + std::to_string(cpuCycle) + ".txt");
			memoryStamp << "Timestamp: " << std::format("({:%m/%d/%Y %r})", std::chrono::current_zone()->to_local(std::chrono::system_clock::from_time_t(time(NULL)))) << std::endl;
			memoryStamp << "Number of processes in memory: " ;
			int processCounter = 0;
			for(int i = 0; i < 4; i++)
			{
				if(this->memoryMap[i] != -1)
					processCounter += 1;
			}
			memoryStamp << processCounter << std::endl;
			bool hasExtFrag = false;
			int firstFree = -1;
			int nextFree = -1;

			for(int i = 0; i < 4; i++)
			{
				if(this->memoryMap[i] == -1)
				{
					if(firstFree == -1)
					{
						firstFree = i;
					}
					else
					{
						nextFree = i;
					}
				}
				if((nextFree - firstFree) >= 2)
				{
					hasExtFrag = true;
					break;
				}
			}

			int extFrag = 0;
			if (hasExtFrag)
			{
				for(int i = 0; i < 4; i++)
				{
					if(memoryMap[i] == -1)
					{
						extFrag += 4096;
					}
				}
			}
			memoryStamp << "Total external fragmentation in KB: " << extFrag << std::endl;
			
			memoryStamp << "----end---- = 16384" << std::endl << std::endl;
			for(int k = 4; k > 0; k--)
			{
				if(this->memoryMap[k - 1] != -1)
				{
					memoryStamp << k * 4096 << std::endl;
					memoryStamp << "P" << this->memoryMap[k - 1] << std::endl;
					memoryStamp << (k - 1) * 4096 << std::endl << std::endl;
				}
			}
			memoryStamp << "----start---- = 0" << std::endl;
			memoryStamp.close();
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
	if(runningProcess->currentLine != runningProcess->totalLine)
		this->readyQueue.push_back(runningProcess);
	else if(runningProcess->currentLine == runningProcess->totalLine)
	{
		for(int i = 0; i < 4; i++)
		{
			if(runningProcess->processId == this->memoryMap[i])
			{
				this->memoryMap[i] = -1;
				break;
			}
		}
		
	}
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

void Scheduler::destroy()
{
	delete sharedInstance;
}

std::string Scheduler::coreSummary()
{	
	int totalCount = this->configVars["num-cpu"];
	int usedCounter = 0;
	std::vector<int> dummyList = this->coreList;
	for(auto i = dummyList.begin(); i != dummyList.end(); i++)
	{
		if((*i) != -1)
		{
			usedCounter++;
		}
	}
	float percent = (usedCounter*100.0f) / totalCount;
	int remainingCore = totalCount - usedCounter;
	std::string summary = std::format("CPU Utilization: {0:.2f}%", percent);
	summary += "\nCores Used: " + std::to_string(usedCounter) + "\n";
	summary += "Cores Available: " + std::to_string(remainingCore);	
	return summary;
}


void Scheduler::varTest()
{
	for (auto it = this->configVars.begin(); it != this->configVars.end(); it++) 
        std::cout << "Key: " << it->first << ", Value: " << it->second << std::endl;
}

void Scheduler::generateDummy(ConsoleManager* cmInstance)
{
	int prevCycle = cpuCycle;
	while(isTesting)
	{
		if(cpuCycle-prevCycle >= (this->configVars["batch-process-freq"]+1))
		{
			mtx.lock();
			std::string tempConsoleName = "Process_" + std::to_string(dummyCounter);
			std::shared_ptr<BaseConsole> tempConsole = std::make_shared<BaseConsole>
			(tempConsoleName);
			this->registerProcess(tempConsole->getAttachedProcess());
			cmInstance->registerDummy(tempConsole);
			dummyCounter++;
			prevCycle = cpuCycle;
			mtx.unlock();
		}
		Sleep(100);
	}
}

void Scheduler::startTester()
{
	isTesting = true;
	std::thread dummyGenerator(&Scheduler::generateDummy, sharedInstance, ConsoleManager::getInstance());
	dummyGenerator.detach();
}

void Scheduler::stopTester()
{
	isTesting = false;
}

Scheduler::Scheduler()
{
	//initialize the scheduler
	this->memoryMap[0] = -1;
	this->memoryMap[1] = -1;
	this->memoryMap[2] = -1;
	this->memoryMap[3] = -1;
}