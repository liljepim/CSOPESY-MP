#pragma once
#include "Typedef.h"
#include <vector>
#include <windows.h>
class InputHandler
{
public:
	static InputHandler* getInstance();
	static void initialize();
	static void destroy();
	
	bool pollKeyboard(String *inputCommand);
	void processCommand();
	void showPrompt();
private:
	InputHandler();
	~InputHandler() = default;
	InputHandler(InputHandler const&) {};
	InputHandler& operator=(InputHandler const&) {};
	static InputHandler* sharedInstance;
	std::string inputBuffer = "";
	HANDLE hnd;
	bool promptShown;
};

