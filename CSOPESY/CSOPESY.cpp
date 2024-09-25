#include <iostream>
#include <stdlib.h>

typedef std::string String;

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

int
main()
{
	printMenu();
	String command = "";
	bool isRunning = true;
	while (isRunning)
	{
		std::cout << "Enter a command: ";
		std::cin >> command;

		if (!command.compare("initialize"))
			std::cout << command + " recognized. Doing something.\n";
		else if (!command.compare("screen"))
			std::cout << command + " recognized. Doing something.\n";
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
			std::cout << command + " not  recognized.\n";
	}
	return 0;
}