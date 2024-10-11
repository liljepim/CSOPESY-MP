#include "Scheduler.h"
#include <iostream>
#include <fstream>
#include <map>
#include <thread>
#include <windows.h>
#include <functional>

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
	for(int i = runningProcess->currentLine; i < runningProcess->totalLine; i++)
	{
		runningProcess->currentLine += 1;
		Sleep(1000);
	}
	//std::cout << "running" << std::endl;
	this->coreList[coreIndex] = -1;
}

void Scheduler::destroy()
{
	delete sharedInstance;
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