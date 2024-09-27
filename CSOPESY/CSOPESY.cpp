#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <ctime>

typedef std::string String;

typedef struct Process
{
	String process_name;
	std::tm *time_created;
}

Process;

void printMenu() {
	std::cout << "     ____. ________              \n";
	std::cout << "    |    |/  _____/  ____  ______\n";
	std::cout << "    |    /   \\  ___ /  _ \\/  ___/\n";
	std::cout << "/\\__|    \\    \\_\\  (  <_> )___ \\ \n";
	std::cout << "\\________|\\______  /\\____/____  >\n";
	std::cout << "                 \\/           \\/ \n";
	std::cout << "Hello, welcome to the GabOS command line!\n";
	std::cout << "Type 'exit' to quit, 'clear' to clear the screen\n";

}

void displayProcess(Process process)
{
	system("cls");
	std::cout << "Process name: " + process.process_name + "\n";
	std::cout << "Current line of instruction: 500\n";
	std::cout << "Total line of instruction: 1000\n";
	std::tm* temp_time = process.time_created;
	std::cout << "Time created: " << (temp_time->tm_year + 1900) << "-" << (temp_time->tm_mon + 1) << "-" << (temp_time->tm_mday) << " " << (temp_time->tm_hour) << ":" << (temp_time->tm_min) << ":" << (temp_time->tm_sec) << "\n"; 
	
	bool processRunning = true;
	String processCommand;
	while(processRunning)
	{
		std::cout << "Enter a command: ";
		getline(std::cin, processCommand);
		if(!processCommand.compare("exit"))
		{
			std::cout << "Exiting to main menu.\n";
			processRunning = false;
		}
		else
			std::cout << processCommand + " not recognized.\n";
	}
	
	system("cls");
	printMenu();
	return;
}

int
main()
{
	printMenu();
	String command = "";
	bool isRunning = true;
	std::vector<Process> process_vector;
	while (isRunning)
	{
		std::cout << "Enter a command: ";
		getline(std::cin, command);

		if (!command.compare("initialize"))
			std::cout << command + " recognized. Doing something.\n";
		else if (!command.compare("screen"))
			std::cout << command + " recognized. Doing something.\n";
		else if (!command.substr(0, 9).compare("screen -s"))
		{
			if(command.size() > 10)
			{
				Process temp_process;
				temp_process.process_name = command.substr(10, command.size() - 1);
				
				std::time_t t = std::time(0);
				temp_process.time_created = std::localtime(&t);
				
				bool nameAlreadyExists = false;
				for(unsigned int i = 0; i < process_vector.size(); i++)
				{
					if(!process_vector[i].process_name.compare(temp_process.process_name))
						nameAlreadyExists = true;
				}
				
				if(!nameAlreadyExists)
				{
					process_vector.push_back(temp_process);
					displayProcess(process_vector.back());
				}
				else
					std::cout << temp_process.process_name << " already exists.\n";
			}
			else
				std::cout << "Name not specified.\n";
		}
		else if (!command.substr(0, 9).compare("screen -r"))
		{
			if(command.size() > 10)
			{
				bool nameExists = false;
				std::string temp_process_name = command.substr(10, command.size() - 1);
				for(unsigned int i = 0; i < process_vector.size(); i++)
				{
					if(!process_vector[i].process_name.compare(temp_process_name))
					{
						nameExists = true;
						displayProcess(process_vector[i]);
					}
				}
				
				if(!nameExists)
					std::cout << temp_process_name << " does not exist.\n";
			}
			else
				std::cout << "Name not specified.\n";
		}
		else if (!command.compare("scheduler-test"))
			std::cout << command + " recognized. Doing something.\n";
		else if (!command.compare("scheduler-stop"))
			std::cout << command + " recognized. Doing something.\n";
		else if (!command.compare("report-util"))
			std::cout << command + " recognized. Doing something.\n";
		else if (!command.compare("clear")) {
			system("cls");
			printMenu();
		}

		else if (!command.compare("exit"))
		{
			std::cout << command + " recognized. Exiting OS.\n";
			isRunning = false;
		}
		else
			std::cout << command + " not recognized.\n";
	}
	return 0;
}