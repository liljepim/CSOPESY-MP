#include <iostream>
#include "ConsoleManager.h"
#include <stdlib.h>
#include <thread>
#include <chrono>
#include <ctime>

typedef std::string String;

bool osInitialized = false;


void tester()
{
	Sleep(5000);
	std::cout << "Hello 2\n";
}

int
main()
{
	ConsoleManager::initialize();

	bool running = true;

	while(running)
	{
		ConsoleManager::getInstance()->process();
		ConsoleManager::getInstance()->drawConsole();

		running = ConsoleManager::getInstance()->isRunning();
	}
	
}

//Time printing
//std::chrono::time_point now = std::chrono::system_clock::now();
//
//std::cout << std::format("{:%m/%d/%y %X}", now);