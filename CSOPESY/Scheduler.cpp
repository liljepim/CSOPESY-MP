#include "Scheduler.h"
#include <iostream>
#include <fstream>
#include <map>
#include <thread>
#include <windows.h>
#include <functional>
#include <ctime>

extern unsigned int cpuCycle;
extern bool isTesting;
unsigned int dummyCounter = 0;

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
	/*for(int i = 0; i < this->readyQueue.size(); i++)
		std::cout << this->readyQueue[i]->processId << std::endl;*/
}

void Scheduler::assignProcesses()
{
	//std::cout << "assigned" << std::endl;
	std::vector<std::thread> cpuCores;
	
	for(int i = 0; i < this->configVars["num-cpu"]; i++)
		cpuCores.push_back(std::thread());
	
	while(true)
	{
		for(int i = 0; i < this->configVars["num-cpu"]; i++)
		{
			if(this->coreList[i] == -1 && !readyQueue.empty())
			{
				this->coreList[i] = this->readyQueue.front()->processId;
				cpuCores[i] = std::thread(&Scheduler::runProcesses, sharedInstance, this->readyQueue.front(), i);
				this->readyQueue.erase(this->readyQueue.begin());
				cpuCores[i].detach();
			}
		} 
	}
}

void Scheduler::runProcesses(std::shared_ptr<Process> runningProcess, int coreIndex)
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
			Sleep(this->configVars["delay-per-exec"]);
			i++;
		}
		
	}
	runningProcess->coreUsed = -1;
	this->coreList[coreIndex] = -1;
	this->readyQueue.push_back(runningProcess);
}

void Scheduler::destroy()
{
	delete sharedInstance;
}

void Scheduler::generateDummy()
{
	int prevCycle = cpuCycle;

	while(isTesting)
	{
		if(cpuCycle-prevCycle >= this->configVars["batch-process-freq"])
		{
			std::string tempConsoleName = "Process_" + dummyCounter;
			std::shared_ptr<BaseConsole> tempConsole = std::make_shared<BaseConsole>(tempConsoleName);
			ConsoleManager::getInstance()->registerDummy(tempConsole);
			dummyCounter++;
			prevCycle = cpuCycle;
		}
	}
}

void Scheduler::startTester()
{
	isTesting = true;
	std::thread dummyGenerator(&Scheduler::generateDummy, sharedInstance);
	dummyGenerator.detach();
}

void Scheduler::stopTester()
{
	isTesting = false;
}


void Scheduler::varTest()
{
	for (auto it = this->configVars.begin(); it != this->configVars.end(); it++) 
        std::cout << "Key: " << it->first << ", Value: " << it->second << std::endl;
}

Scheduler::Scheduler()
{
	//initialize the scheduler
}