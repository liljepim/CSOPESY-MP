#include "Scheduler.h"
#include <iostream>
#include <fstream>
#include <map>

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