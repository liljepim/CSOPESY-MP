#pragma once
#include "AConsole.h"
#include "Typedef.h"
#include "ConsoleManager.h"
#include "BaseConsole.h"
class MainConsole : public AConsole
{
public:
	MainConsole();
	void onEnabled() override;
	void process() override;
	void display() override;
	void printBanner() const;
	void processList();
	void generateLog();
private:
	bool refreshed = false;
	bool initialized = true;
	bool promptShown = false;
	String command = "";
};

