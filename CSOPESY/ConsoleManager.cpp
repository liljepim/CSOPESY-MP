#include "ConsoleManager.h"
#include <iostream>

ConsoleManager* ConsoleManager::sharedInstance = nullptr;

ConsoleManager* ConsoleManager::getInstance()
{
	return sharedInstance;
}

void ConsoleManager::initialize()
{
	sharedInstance = new ConsoleManager();
}

void ConsoleManager::destroy()
{
	delete sharedInstance;
}

ConsoleManager::ConsoleTable* ConsoleManager::getConsoleTable()
{
	return &this->consoleTable;
}


void ConsoleManager::drawConsole() const
{
	if (this->currentConsole != nullptr)
	{
		this->currentConsole->display();
	}
	else
	{
		std::cerr << "No assigned console. Please check." << std::endl;
	}
}

void ConsoleManager::process() const
{
	if(this->currentConsole != nullptr)
	{
		this->currentConsole->process();
	}
	else
	{
		std::cerr << "No assigned console. Please check." << std::endl;
	}
}

void ConsoleManager::switchConsole(String consoleName)
{
	if (this->consoleTable.contains(consoleName))
	{
		// Clear the screen
		system("cls");
		this->previousConsole = this->currentConsole;
		this->currentConsole = this->consoleTable[consoleName];
		this->currentConsole->onEnabled();
	}
	else
	{
		std::cerr << "Console name " << consoleName << " not found. Was it initialized?" << std::endl;
	}
}

bool ConsoleManager::registerScreen(std::shared_ptr<BaseConsole> consoleRef)
{
	if(this->consoleTable.contains(consoleRef->getName()))
	{
		std::cerr << "Process name " << consoleRef->getName() << " already exists. Please use a different name." << std::endl;
		return false;
	}
	this->consoleTable[consoleRef->getName()] = consoleRef;
	return true;
}

void ConsoleManager::returnToPreviousConsole()
{
	if(this->previousConsole != nullptr)
	{
		this->switchToScreen(this->previousConsole->getName());
		
	}
}

bool ConsoleManager::switchToScreen(String consoleName)
{
	if(this->consoleTable.contains(consoleName))
	{
		//Clear Screen
		system("cls");
		this->previousConsole = this->currentConsole;
		this->currentConsole = consoleTable[consoleName];
		this->currentConsole->onEnabled();
		return true;
	}
	else
	{
		std::cerr << "Console name " << consoleName << " not found. Was it initialized?" << std::endl;
		return false;
	}
}

void ConsoleManager::exitApplication()
{
	system("cls");
	this->running = false;
}

bool ConsoleManager::isRunning() const
{
	return this->running;
}

HANDLE ConsoleManager::getConsoleHandle() const
{
	return this->consoleHandle;
}

void ConsoleManager::setCursorPosition(int posX, int posY) const
{
	SetConsoleCursorPosition(this->consoleHandle, { SHORT(posX), SHORT(posY) });
}


ConsoleManager::ConsoleManager()
{
	this->running = true;

	//initialize the consoles
	this->consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	const std::shared_ptr<MainConsole> mainConsole = std::make_shared<MainConsole>();

	this->consoleTable[MAIN_CONSOLE] = mainConsole;

	this->switchConsole(MAIN_CONSOLE);
}