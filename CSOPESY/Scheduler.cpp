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
	std::ifstream configFile("config.txt");
	if (!configFile.is_open())
		std::cerr << "The config file cannot be opened." << std::endl;
	String line, varName, varValue;
	int spacePos;
	while(getline(configFile, line))
	{
		spacePos = line.find(" ");
		varName = line.substr(0, spacePos);
		varValue = line.substr(spacePos + 1, line.size());
		if(varName.compare("scheduler") != 0)
			configVars[varName] = std::stoi(varValue);
		else
			scheduler = varValue;
	}
}

void Scheduler::destroy()
{
	delete sharedInstance;
}
