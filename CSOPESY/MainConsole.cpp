#include "MainConsole.h"

#include <string>

extern bool osInitialized;

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
		String command = "";
		std::cout << "Enter Command: ";
		std::getline(std::cin, command);
		if (command == "exit")
		{
			ConsoleManager::getInstance()->exitApplication();
			return;
		}
		else if (command == "clear")
		{
			system("cls");
			printBanner();
		}
		else if (command.starts_with("screen -s "))
		{
			String tempName = "";
			if(command.length() > 9)
			{
				tempName.append(command,10, command.length()-9);
			}
			std::cout << "Creating process " << tempName << "..." << std::endl;
			std::shared_ptr<BaseConsole> tempConsole = std::make_shared<BaseConsole>(tempName);
			bool success = ConsoleManager::getInstance()->registerScreen(tempConsole);
			if(success)
				ConsoleManager::getInstance()->switchToScreen(tempName);
				this->refreshed = false;
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
		else
		{
			std::cout << "Command not recognized. Try again.\n";
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