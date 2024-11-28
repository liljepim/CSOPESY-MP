#include <iostream>
#include "ConsoleManager.h"
#include "Scheduler.h"
#include "InputHandler.h"
#include <stdlib.h>
#include <thread>
#include <chrono>
#include <ctime>
#include <mutex>



std::mutex mtx;

typedef std::string String;

bool osInitialized = false;
bool isTesting = false;

unsigned int cpuCycle = 0;

int
main()
{
	ConsoleManager::initialize();
	Scheduler::initialize();
	InputHandler::initialize();
	bool running = true;

	while(running)
	{
		ConsoleManager::getInstance()->drawConsole();
		ConsoleManager::getInstance()->process();
		running = ConsoleManager::getInstance()->isRunning();
		if(osInitialized)
		{
			if(!Scheduler::getInstance()->isOn)
			{
				Scheduler::getInstance()->readConfig();
			} else
			{
				Scheduler::getInstance()->assignProcesses();
			}
				

		}
		cpuCycle++;
	}
	
	ConsoleManager::destroy();
	Scheduler::destroy();
	InputHandler::destroy();
}