/* Console Manager (Singleton)
 * - Manages Console Entities
 * - Responsible for creating Console objects for processes
 * - Switches between different consoles
 * - Deletes consoles
 */
#pragma once
#include <unordered_map>
#include <Windows.h>
#include "Typedef.h"
#include "AConsole.h"
#include "BaseConsole.h"
#include "MainConsole.h"

const String MAIN_CONSOLE = "MAIN_CONSOLE";

class ConsoleManager
{
public:
	typedef std::unordered_map<String, std::shared_ptr<AConsole>> ConsoleTable;

	static ConsoleManager* getInstance();
	static void initialize();
	static void destroy();

	void drawConsole() const;
	void process() const;
	void switchConsole(String consoleName);

	bool registerScreen(std::shared_ptr<BaseConsole> consoleRef);
	bool switchToScreen(String screenName);
	/*void unregisterScreen(String screenName);*/

	void returnToPreviousConsole();
	void exitApplication();
	bool isRunning() const;
	
	HANDLE getConsoleHandle() const;

	void setCursorPosition(int posX, int posY) const;
private:
	ConsoleManager();
	~ConsoleManager() = default;
	ConsoleManager(ConsoleManager const&) {};
	ConsoleManager& operator=(ConsoleManager const&) {};
	static ConsoleManager* sharedInstance;

	ConsoleTable consoleTable;
	std::shared_ptr<AConsole> currentConsole;
	std::shared_ptr<AConsole> previousConsole;

	HANDLE consoleHandle;
	bool running = true;

};