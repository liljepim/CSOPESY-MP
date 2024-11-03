#include "MainConsole.h"
#include <format>
#include <string>
#include <chrono>

extern bool osInitialized;
extern bool isTesting;
extern unsigned int cpuCycle;

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

void MainConsole::processList()
{
	std::cout << "\nRunning processes:" << std::endl;
	int runningCount = 0;
	for(auto i = ConsoleManager::getInstance()->getConsoleTable()->begin(); i != ConsoleManager::getInstance()->getConsoleTable()->end(); i++)
	{
		std::shared_ptr<BaseConsole> tempHolder = std::dynamic_pointer_cast<BaseConsole>(i->second);
		if(i->first != "MAIN_CONSOLE" && tempHolder->getAttachedProcess()->currentLine != tempHolder->getAttachedProcess()->totalLine && tempHolder->getAttachedProcess()->coreUsed != -1)
		{
			std::cout << tempHolder->getName() << "\t" << std::format("({:%m/%d/%Y %r})", std::chrono::current_zone()->to_local(std::chrono::system_clock::from_time_t(tempHolder->getAttachedProcess()->lastExecuted))) << "\tCore:" << tempHolder->getAttachedProcess()->coreUsed << "\t" << tempHolder->getAttachedProcess()->currentLine << "/" << tempHolder->getAttachedProcess()->totalLine << std::endl;
			runningCount++;
		}
		
	}
	if(runningCount == 0 )
	{
		std::cout << "No running processes." << std::endl;
	}

	std::cout << "\nFinished processes:" << std::endl;
	for (auto i = ConsoleManager::getInstance()->getConsoleTable()->begin(); i != ConsoleManager::getInstance()->getConsoleTable()->end(); i++)
	{
		std::shared_ptr<BaseConsole> tempHolder = std::dynamic_pointer_cast<BaseConsole>(i->second);
		if (i->first != "MAIN_CONSOLE" && tempHolder->getAttachedProcess()->currentLine == tempHolder->getAttachedProcess()->totalLine)
		{
			std::cout << tempHolder->getName() << "\t" << std::format("({:%m/%d/%Y %r})", std::chrono::current_zone()->to_local(std::chrono::system_clock::from_time_t(tempHolder->getAttachedProcess()->lastExecuted))) << "\tFinished\t" <<  tempHolder->getAttachedProcess()->currentLine << "/" << tempHolder->getAttachedProcess()->totalLine << std::endl;
		}

	}
}

void MainConsole::process()
{
	if(!(this->refreshed))
	{
		String command = "";
		std::cout << "Enter Command: ";
		std::getline(std::cin, command);
		if(osInitialized)
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

				processList();
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
			if(command == "initialize")
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
			}
			else
			{
				std::cout << "Command not recognized. Try again.\n";
			}
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