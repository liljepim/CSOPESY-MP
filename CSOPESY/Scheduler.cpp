#include "Scheduler.h"
#include "FlatMemoryAllocator.h"
#include "PagingAllocator.h"
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
#include <filesystem>

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
		//std::cout << varName << std::endl;
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

	FlatMemoryAllocator::initialize();
	isOn = true;
	
	std::map<String, int>::iterator it;

	for(it = this->configVars.begin(); it != configVars.end(); it++)
	{
		std::cout << it->first << " : " << it->second << std::endl;
	}
	this->powMin = static_cast<int>(log2(this->configVars["min-mem-per-proc"]));
	this->powMax = static_cast<int>(log2(this->configVars["max-mem-per-proc"]));
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
		if(this->coreList[i] == -1)
		{
			idleticks += 1;
		}
	}
	for(int i = 0; i < this->configVars["num-cpu"]; i++)
	{
		if(this->coreList[i] == -1 && !readyQueue.empty())
		{
			int finalIndex;
			bool canAllocate = FlatMemoryAllocator::getInstance()->canAllocate(this->readyQueue.front(), &finalIndex);
			if(canAllocate)
			{
				this->coreList[i] = this->readyQueue.front()->processId;
				FlatMemoryAllocator::getInstance()->allocate(this->readyQueue.front());
				if(this->scheduler == "\"rr\"")
					cpuCores[i] = std::thread(&Scheduler::runProcessesRR, sharedInstance, this->readyQueue.front(), i);
				else if(this->scheduler == "\"fcfs\"")
					cpuCores[i] = std::thread(&Scheduler::runProcessesFCFS, sharedInstance, this->readyQueue.front(), i);
				this->readyQueue.erase(this->readyQueue.begin());
				cpuCores[i].detach();
			}
			else
			{
				if(this->scheduler == "\"rr\"")
				{
					this->readyQueue.push_back(this->readyQueue.front());
					this->readyQueue.erase(this->readyQueue.begin());
				}
			}
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

void Scheduler::assignProcesses_Paging()
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
		if(this->coreList[i] == -1)
		{
			idleticks += 1;
		}
	}
	for(int i = 0; i < this->configVars["num-cpu"]; i++)
	{
		if(this->coreList[i] == -1 && !readyQueue.empty())
		{
			int finalIndex;
			//bool canAllocate = FlatMemoryAllocator::getInstance()->canAllocate(this->readyQueue.front(), &finalIndex);
			std::vector<int> freeIndices = PagingAllocator::getInstance()->canAllocate(this->readyQueue.front());
			if(freeIndices.size() == this->readyQueue.front()->requiredFrames)
			{
				this->coreList[i] = this->readyQueue.front()->processId;
				//FlatMemoryAllocator::getInstance()->allocate(this->readyQueue.front());
				PagingAllocator::getInstance()->pageIn(this->readyQueue.front());
				if(this->scheduler == "\"rr\"")
					cpuCores[i] = std::thread(&Scheduler::runProcessesRR_Paging, sharedInstance, this->readyQueue.front(), i);
				else if(this->scheduler == "\"fcfs\"")
					cpuCores[i] = std::thread(&Scheduler::runProcessesFCFS_Paging, sharedInstance, this->readyQueue.front(), i);
				this->readyQueue.erase(this->readyQueue.begin());
				cpuCores[i].detach();
			}
			else
			{
				//replace with backing store funcs
				if(this->scheduler == "\"rr\"")
				{
					this->readyQueue.push_back(this->readyQueue.front());
					this->readyQueue.erase(this->readyQueue.begin());
					PagingAllocator::getInstance()->storeOldest();
				}
			}
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
	runningProcess->isRunning = true;
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
			mtx.lock();
			Scheduler::getInstance()->activeticks += 1;
			mtx.unlock();
			i++;
			
		}
	}
	mtx.lock();
	runningProcess->coreUsed = -1;
	this->coreList[coreIndex] = -1;
	if(runningProcess->currentLine != runningProcess->totalLine)
	{
		this->readyQueue.push_back(runningProcess);
		runningProcess->isRunning = false;
		FlatMemoryAllocator::getInstance()->backingStore(runningProcess);
	}
	else if(runningProcess->currentLine == runningProcess->totalLine)
	{
		FlatMemoryAllocator::getInstance()->deallocate(runningProcess);
		this->finishedProcesses.push_back(runningProcess);
		runningProcess->timeFinished = time(NULL);
	}
	mtx.unlock();
	runningProcess->isRunning = false;
}

void Scheduler::runProcessesRR_Paging(std::shared_ptr<Process> runningProcess, int coreIndex)
{
	runningProcess->isRunning = true;
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
			mtx.lock();
			Scheduler::getInstance()->activeticks += 1;
			mtx.unlock();
			i++;
			
		}
	}
	mtx.lock();
	runningProcess->coreUsed = -1;
	this->coreList[coreIndex] = -1;
	if(runningProcess->currentLine != runningProcess->totalLine)
	{
		this->readyQueue.push_back(runningProcess);
		runningProcess->isRunning = false;
		//FlatMemoryAllocator::getInstance()->backingStore(runningProcess);
		PagingAllocator::getInstance()->pageOut(runningProcess);
	}
	else if(runningProcess->currentLine == runningProcess->totalLine)
	{
		//FlatMemoryAllocator::getInstance()->deallocate(runningProcess);
		PagingAllocator::getInstance()->pageOut(runningProcess);
		this->finishedProcesses.push_back(runningProcess);
		runningProcess->timeFinished = time(NULL);
	}
	mtx.unlock();
	runningProcess->isRunning = false;
}

void Scheduler::runProcessesFCFS(std::shared_ptr<Process> runningProcess, int coreIndex)
{
	runningProcess->isRunning = true;
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
	FlatMemoryAllocator::getInstance()->deallocate(runningProcess);
	runningProcess->timeFinished = time(NULL);
	this->finishedProcesses.push_back(runningProcess);
	mtx.unlock();
	runningProcess->isRunning = false;
}

void Scheduler::runProcessesFCFS_Paging(std::shared_ptr<Process> runningProcess, int coreIndex)
{
	runningProcess->isRunning = true;
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
	//FlatMemoryAllocator::getInstance()->deallocate(runningProcess);
	PagingAllocator::getInstance()->pageOut(runningProcess);
	runningProcess->timeFinished = time(NULL);
	this->finishedProcesses.push_back(runningProcess);
	mtx.unlock();
	runningProcess->isRunning = false;
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
	return this->powMax;
}

int Scheduler::getMemMin()
{
	return this->powMin;
}

void Scheduler::destroy()
{
	delete sharedInstance;
}

void Scheduler::coreSummary()
{
	mtx.lock();
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
	std::stringstream buffer;
	buffer << "CPU utilization: " << cpuUtil << "%\n";
	buffer << "Cores used: " << usedCounter << std::endl;
	buffer << "Cored available: " << totalCount - usedCounter << std::endl;

	buffer << "\n------------------------------------------------------------------\n";
	buffer << "\nRunning Processes:\n";
	for(auto i : dummyList)
	{
		if(i != -1)
		{
			std::shared_ptr<Process> tempProc = ConsoleManager::getInstance()->findConsole(i);
			if (tempProc->coreUsed != -1)
			{
				buffer << tempProc->name << "\t" << std::format("({:%m/%d/%Y %r})", std::chrono::current_zone()->to_local(std::chrono::system_clock::from_time_t(tempProc->lastExecuted))) << "\tCore:" << tempProc->coreUsed << "\t" << tempProc->currentLine << "/" << tempProc->totalLine << std::endl;
			}
		}
	}

	buffer << "\nFinished Processes:\n";
	for(auto tempProc : finishedProcesses)
	{
		buffer << tempProc->name << "\t" << std::format("({:%m/%d/%Y %r})", std::chrono::current_zone()->to_local(std::chrono::system_clock::from_time_t(tempProc->timeFinished))) << "\t" << "Finished" << "\t" << tempProc->currentLine << "/" << tempProc->totalLine << std::endl;
	}
	buffer << "\n------------------------------------------------------------------\n";
	std::cout << buffer.str();
	if(!prevSummary.empty())
	{
		prevSummary.clear();
	}
	prevSummary = buffer.str();

	mtx.unlock();
 }

void Scheduler::saveSummary()
{
	if(!prevSummary.empty())
	{
		std::ofstream log("csopesy-log.txt");
		log << prevSummary;
		log.close();
		std::cout << "Report generated at " << std::filesystem::current_path() << "\\csopesy-log.txt" << std::endl;
	}
	else
	{
		std::cout << "Summary not yet generated. Use command screen -ls to generate summary." << std::endl;
	}
}

void Scheduler::generateSMI()
{
	mtx.lock();
	int totalCount = this->configVars["num-cpu"];
	int usedCounter = 0;
	std::vector<int> dummyList;
	for (const int& i : this->coreList)
	{
		dummyList.push_back(i);
		if (i != -1)
		{
			usedCounter += 1;
		}
	}

	float cpuUtil = (usedCounter * 100.0f) / totalCount;
	std::cout << "\n---------------------------------------------------------\n";
	std::cout << "|	     PROCESS-SMI V1.0.0 Driver Version 01.00      |";
	std::cout << "\n---------------------------------------------------------\n";
	std::cout << "CPU-Util: " << cpuUtil << "%\n";
	FlatMemoryAllocator::getInstance()->generateSummary();
	mtx.unlock();
}


void Scheduler::varTest()
{
	for (auto it = this->configVars.begin(); it != this->configVars.end(); it++) 
        std::cout << "Key: " << it->first << ", Value: " << it->second << std::endl;
}

void Scheduler::genstat()
{
	/*
	 *TOTAL MEMORY
	 *USED MEMORY
	 *FREE MEMORY
	 *IDLE CPU TICK
	 *ACTIVE CPU TICKS
	 *NUM PAGED IN
	 *NUM PAGED OUT
	 */

	std::cout << "\n=============VMSTAT=============\n";
	FlatMemoryAllocator::getInstance()->genstat();
	std::cout << "Idle CPU Ticks : " << this->idleticks << std::endl;
	std::cout << "Active CPU Ticks : " << this->activeticks << std::endl;
	std::cout << "Total CPU Ticks : " << this->idleticks + this->activeticks << std::endl;
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
	prevSummary = "";
	int idleTicks = 0;
	int activeTicks = 0;
	if(configVars["max-overall-mem"] == configVars["mem-per-frame"])
		isPaging = false;
	else
		isPaging = true;
}