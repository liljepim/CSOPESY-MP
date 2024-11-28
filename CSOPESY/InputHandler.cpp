#include "InputHandler.h"
#include "ConsoleManager.h"
#include "MainConsole.h"
#include "AConsole.h"
#include <conio.h>

extern bool osInitialized;

InputHandler* InputHandler::sharedInstance = nullptr;

InputHandler* InputHandler::getInstance()
{
	return sharedInstance;
}

void InputHandler::initialize()
{
	sharedInstance = new InputHandler();
}

void InputHandler::destroy()
{
	delete sharedInstance;
}

void InputHandler::processCommand()
{

	if(osInitialized)
	{
		if(inputBuffer == "clear")
		{
			std::cout << "INSIDE" << std::endl;
			system("cls");
			ConsoleManager::getInstance()->refreshConsole();
		}
	} else
	{
		if(inputBuffer == "initialize")
		{
			osInitialized = true;
			std::cout << "OS Initialized.\n";

		}
	}

	inputBuffer = "";
	std::cout << std::endl;
	promptShown = false;

}

void InputHandler::showPrompt()
{
	std::cout << "Enter Command: ";
}

bool InputHandler::pollKeyboard(String *inputCommand)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	if(_kbhit())
	{
		char inp = _getch();
		if(inp == '\r')
		{
			std::cout << std::endl;
			return true;
		}
		else if (inp == 127 || inp == 8)
		{
			if (!inputCommand->empty())
			{
				GetConsoleScreenBufferInfo(hnd, &csbi);
				ConsoleManager::getInstance()->setCursorPosition(csbi.dwCursorPosition.X - 1, csbi.dwCursorPosition.Y);
				std::cout << " ";
				inputCommand->pop_back();
				ConsoleManager::getInstance()->setCursorPosition(csbi.dwCursorPosition.X - 1, csbi.dwCursorPosition.Y);
			}
		}
		else
		{

			(*inputCommand) += inp;
			std::cout << inp;
		}
		return false;
	}
	return false;
	
}

//char InputHandler::pollKeyboard()
//{
//
//	if(_kbhit())
//	{
//		return _getch();
//	}
//}

InputHandler::InputHandler()
{
	hnd = ConsoleManager::getInstance()->getConsoleHandle();
	this->promptShown = false;
}

