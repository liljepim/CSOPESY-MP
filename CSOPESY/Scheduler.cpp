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

extern unsigned int cpuCycle;
extern bool isTesting;
unsigned int dummyCounter = 0;
extern std::mutex mtx;

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
	this->processCount += 1;
}

void Scheduler::assignProcesses()
{
	std::vector<std::thread> cpuCores;
	
	for(int i = 0; i < this->configVars["num-cpu"]; i++)
		cpuCores.push_back(std::thread());
	
	while(true)
	{
		mtx.lock();
		for(int i = 0; i < this->configVars["num-cpu"]; i++)
		{
			
			if(this->coreList[i] == -1 && !readyQueue.empty())
			{
				this->coreList[i] = this->readyQueue.front()->processId;
				if(this->scheduler == "\"rr\"")
					cpuCores[i] = std::thread(&Scheduler::runProcessesRR, sharedInstance, this->readyQueue.front(), i);
				else
					cpuCores[i] = std::thread(&Scheduler::runProcessesFCFS, sharedInstance, this->readyQueue.front(), i);
				this->readyQueue.erase(this->readyQueue.begin());
				cpuCores[i].detach();
				
			}
		} 
		mtx.unlock();
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
		if(cpuCycle-previousCycle == configVars["delay-per-exec"])
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
	this->readyQueue.push_back(runningProcess);
	mtx.unlock();
}

void Scheduler::runProcessesFCFS(std::shared_ptr<Process> runningProcess, int coreIndex)
{
	unsigned int previousCycle = cpuCycle;
	for(int i = runningProcess->currentLine; i < runningProcess->totalLine;)
	{
		if(cpuCycle-previousCycle == configVars["delay-per-exec"])
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
	this->processCount -= 1;
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
		if(cpuCycle-prevCycle >= this->configVars["batch-process-freq"])
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
}