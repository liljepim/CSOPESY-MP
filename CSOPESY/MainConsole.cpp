#include "MainConsole.h"
#include "Scheduler.h"
#include "InputHandler.h"
#include <format>
#include <string>
#include <chrono>
#include <mutex>
#include <fstream>
#include <filesystem>

extern std::mutex mtx;
extern bool osInitialized;
extern unsigned int cpuCycle;
extern bool isTesting;

void MainConsole::printBanner() const
{
	std::cout << "    {__     {__ __      {____     {_______  {________  {__ __  {__      {__\n";
	std::cout << " {__   {__{__    {__  {__    {__  {__    {__{__      {__    {__ {__    {__ \n";
	std::cout << "{__        {__      {__        {__{__    {__{__       {__        {__ {__   \n";
	std::cout << "{__          {__    {__        {__{_______  {______     {__        {__     \n";
	std::cout << "{__             {__ {__        {__{__       {__            {__     {__     \n";
	std::cout << " {__   {__{__    {__  {__     {__ {__       {__      {__    {__    {__     \n";
	std::cout << "   {____    {__ __      {____     {__       {________  {__ __      {__     \n";

	std::cout << "Hello, welcome to the CSOPESY command line!\n";
	std::cout << "Type 'exit' to quit, 'clear' to clear the screen\n";
}

void MainConsole::display()
{
	if (this->refreshed)
	{
		ConsoleManager::getInstance()->setCursorPosition(0, 0);
		printBanner();
		this->refreshed = false;
	}
}

void MainConsole::process()
{
	if(!(this->refreshed))
	{
		if(!promptShown)
		{
			std::cout << "Enter Command: ";
			promptShown = true;
		}
		
		bool commandEntered = InputHandler::getInstance()->pollKeyboard(&command);
		if(commandEntered)
		{
			if (osInitialized)
			{
				if (command == "clear")
				{
					system("cls");
					printBanner();
				}
				else if (command.starts_with("screen -s "))
				{
					String tempName = "";
					if (command.length() > 9)
					{
						tempName.append(command, 10, command.length() - 9);
					}
					std::cout << "Creating process " << tempName << "..." << std::endl;
					std::shared_ptr<BaseConsole> tempConsole = std::make_shared<BaseConsole>(tempName);
					bool success = ConsoleManager::getInstance()->registerScreen(tempConsole);
					if (success)
						ConsoleManager::getInstance()->switchToScreen(tempName);
					this->refreshed = false;
					Scheduler::getInstance()->registerProcess((tempConsole->getAttachedProcess()));
				}
				else if (command.starts_with("screen -r "))
				{
					String tempName = "";
					if (command.length() > 9)
					{
						tempName.append(command, 10, command.length() - 9);
					}

					bool success = ConsoleManager::getInstance()->switchToScreen(tempName);
					if (success)
						this->refreshed = false;
				}
				else if (command == "screen -ls")
				{
					Scheduler::getInstance()->coreSummary();
				}
				else if (command == "scheduler-test")
				{
					Scheduler::getInstance()->startTester();
				}
				else if (command == "scheduler-stop")
				{
					Scheduler::getInstance()->stopTester();
				}
				else if (command == "show-cycle")
				{
					std::cout << cpuCycle << std::endl;
					std::cout << Scheduler::getInstance()->previousBF << std::endl;
					std::cout << Scheduler::getInstance()->previousQQ << std::endl;
				}
				else if (command == "report-util")
				{
					Scheduler::getInstance()->saveSummary();
				}
				else if (command == "process-smi")
				{
					Scheduler::getInstance()->generateSMI();
				}
				else if (command == "exit")
				{
					ConsoleManager::getInstance()->exitApplication();
					return;
				}
				else
				{
					std::cout << "Command not recognized. Try again.\n";
				}
			}
			else if (!osInitialized)
			{
				if (command == "initialize")
				{
					osInitialized = true;
					std::cout << "OS Initialized.\n";
					Scheduler::getInstance()->readConfig();
				}
				else if (command == "exit")
				{
					ConsoleManager::getInstance()->exitApplication();
					return;
				}
				else if (command == "show-cycle")
				{
					std::cout << cpuCycle << std::endl;
					std::cout << Scheduler::getInstance()->previousBF << std::endl;
					std::cout << Scheduler::getInstance()->previousQQ << std::endl;
				}
				else
				{
					std::cout << "Command not recognized. Try again.\n";
				}
			}
			promptShown = false;
			command = "";
		}
		
		
	}
}

void MainConsole::onEnabled()
{
	this->refreshed = true;
}

MainConsole::MainConsole() : AConsole("MAIN_CONSOLE")
{
	this->name = "MAIN_CONSOLE";
}