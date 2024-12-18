#include "BaseConsole.h"
#include "InputHandler.h"
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

std::shared_ptr<Process> BaseConsole::getAttachedProcess()
{
	return this->attachedProcess;
}

void BaseConsole::process()
{
	if(!(this->refreshed))
	{
		if(!promptShown)
		{
			std::cout << "Enter command: ";
			promptShown = true;
		}
		bool commandEntered = InputHandler::getInstance()->pollKeyboard(&command);
		if(commandEntered)
		{
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
			promptShown = false;
			command = "";
		}
		
	}
}

void BaseConsole::printProcessInfo() const
{
	std::cout << "Process Name: " << this->name << std::endl;
	std::cout << "PID: " << this->attachedProcess->processId<< std::endl;

	if (this->attachedProcess->currentLine == this->attachedProcess->totalLine)
	{
		std::cout << "Finished!" << std::endl;
	}
	else
	{
		std::cout << "Current Instruction Line: " << this->attachedProcess->currentLine << std::endl;
		std::cout << "Lines of Code: " << this->attachedProcess->totalLine << std::endl;
	}
}



void BaseConsole::display()
{
	if(this->refreshed)
	{
		this->printProcessInfo();
		this->refreshed = false;
	}
	
	
}
