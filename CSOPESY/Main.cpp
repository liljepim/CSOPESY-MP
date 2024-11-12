#include <iostream>
#include "ConsoleManager.h"
#include "Scheduler.h"
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


void incrementCycle()
{
	while(ConsoleManager::getInstance()->isRunning())
	{
		if (osInitialized)
		{
			cpuCycle++;
			std::this_thread::sleep_for(std::chrono::nanoseconds(100));
		}
	}
	
}

int
main()
{
	ConsoleManager::initialize();
	Scheduler::initialize();
	std::thread cpuCycler(incrementCycle);
	cpuCycler.detach();
	bool running = true;

	while(running)
	{
		ConsoleManager::getInstance()->process();
		ConsoleManager::getInstance()->drawConsole();

		running = ConsoleManager::getInstance()->isRunning();
	}
	
	ConsoleManager::destroy();
	Scheduler::destroy();
}

//Time printing
//std::chrono::time_point now = std::chrono::system_clock::now();
//
//std::cout << std::format("{:%m/%d/%y %X}", now);