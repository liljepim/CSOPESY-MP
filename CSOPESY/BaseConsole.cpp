#include "BaseConsole.h"

#include "ConsoleManager.h"

BaseConsole::BaseConsole(String processName) : AConsole(processName)
{
	this->name = processName;
	this->attachedProcess = std::make_shared<Process>(processName);
}

void BaseConsole::onEnabled()
{
	this->refreshed = true;
}

void BaseConsole::process()
{
	if(!(this->refreshed))
	{
		String command = "";
		std::cout << "Enter command: ";
		std::getline(std::cin, command);
		if (command == "process-smi")
		{
			this->printProcessInfo();
		}
		else if (command == "exit")
		{
			ConsoleManager::getInstance()->returnToPreviousConsole();
		}
		else
		{
			std::cout << "Invalid Command. Try Again.";
		}
	}
}

void BaseConsole::printProcessInfo() const
{
	std::cout << "Process Name: " << this->name << std::endl;
	std::cout << "PID: " << this->attachedProcess->processId<< std::endl;
	std::cout << "Current Instruction Line: " << this->attachedProcess->currentLine << std::endl;
	std::cout << "Lines of Code: " << this->attachedProcess->totalLine << std::endl;

}



void BaseConsole::display()
{
	if(this->refreshed)
	{
		this->printProcessInfo();
		this->refreshed = false;
	}
	
	
}
