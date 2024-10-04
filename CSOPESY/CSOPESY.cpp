#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <ctime>

typedef std::string String;

typedef struct Process
{
	String process_name;
	//std::tm *time_created;
	String time_created;
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
	std::cout << "Time created: " << process.time_created << "\n";
	
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

String formatProcessName(String processName)
{
	String stringBuffer;
	if(processName.size() < 44)
	{
		stringBuffer = processName;
		for(unsigned int i = 0; i < 45 - processName.size(); i++)
			stringBuffer.append(" ");
	}
	else
	{
		stringBuffer = "...";
		stringBuffer.append(processName.substr(processName.size() - 41, 41));
		stringBuffer.append(" ");
	}
	return stringBuffer;
}

void displaySMI(std::vector<Process> process_vector)
{
	system("cls");
	time_t timestamp;
	time(&timestamp);
	bool isRunning = true;
	String command;
	
	while(isRunning)
	{
		std::cout << ctime(&timestamp);
        std::cout << "+---------------------------------------------------------------------------------------------+\n";
        std::cout << "| NVIDIA-SMI 551.86                    Driver Version: 551.86            CUDA VERSION: 12.4   |\n";
        std::cout << "|--------------------------------------------+-------------------------+----------------------|\n";
        std::cout << "| GPU NAME                         TCC/WDDM  | Bus-Id             DISP | Volatile Uncorr. ECC |\n";
        std::cout << "| Fan Temp PERF                 Pwr:Usage/Cap|            Memory-Usage | GPU UTIL Compute M.  |\n";
        std::cout << "|                                            |                         |              Mig M.  |\n";
        std::cout << "|============================================+=========================+======================|\n";
        std::cout << "|   0   NVIDIA Geforce GTX 1080         WDDM |      00000000:26:00.0 ON|                 N/A  |\n";
        std::cout << "| 28%      37C   P8               11W/  180W |        701MiB /  8192Mib|        0%     Default|\n";
        std::cout << "|                                            |                         |                 N/A  |\n";
        std::cout << "+---------------------------------------------------------------------------------------------+\n\n";

        std::cout << "+---------------------------------------------------------------------------------------------+\n";
        std::cout << "| Processes                                                                                   |\n";
        std::cout << "|  GPU   GI    CI       PID    TYPE    Process name                            GPU MEMORY     |\n";
        std::cout << "|        ID    ID                                                                 USAGE       |\n";
        std::cout << "|=============================================================================================|\n";
        
		std::cout << "|    0   N/A   N/A     " << "9999" << "     " << "C+G" << "    " << formatProcessName("C:\\Windows\\System32\\dwn.exe") << "N/A" << "       |\n";
		std::cout << "|    0   N/A   N/A     " << "9999" << "     " << "C+G" << "    " << formatProcessName("weky238fbsdoasdi381912asd8fasdu34\\XboxGameBarWidgets.exe") << "N/A" << "       |\n";
		std::cout << "|    0   N/A   N/A     " << "9999" << "     " << "C+G" << "    " << formatProcessName("C:\\Program Files (x86)\\Java\\jdk-1.8\\jre\\bin\\dtplugin.exe") << "N/A" << "       |\n";
		std::cout << "|    0   N/A   N/A     " << "9999" << "     " << "C+G" << "    " << formatProcessName("C:\\noita\\mame\\castool.exe") << "N/A" << "       |\n";
		std::cout << "|    0   N/A   N/A     " << "9999" << "     " << "C+G" << "    " << formatProcessName("C:\\Program Files (x86)\\SASM\\MinGW\\bin\\gcc.exe") << "N/A" << "       |\n";

		std::cout << "|=============================================================================================|\n";
		
		std::cout << "Enter a command: ";
		getline(std::cin, command);
		
		if(!command.compare("exit"))
		{
			std::cout << command + " recognized. Exiting smi.\n";
			isRunning = false;
		}
		else
			std::cout << command + " not recognized.\n";
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
				
				time_t rawtime;
				struct tm * timeinfo;
				char time_buffer[80];
				time (&rawtime);
				timeinfo = localtime(&rawtime);
				strftime(time_buffer, sizeof(time_buffer),"%d-%m-%Y %H:%M:%S", timeinfo);
				std::string time_string(time_buffer);
				temp_process.time_created = time_string;
				
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
		else if (!command.compare("smi"))
			displaySMI(process_vector);
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