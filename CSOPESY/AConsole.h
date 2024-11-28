#pragma once
#include <iostream>
#include <string>
#include "Typedef.h"
#include "Process.h"
class AConsole
{
public:
	AConsole(String name);
	~AConsole() = default;

	String getName();
	virtual void onEnabled() = 0;	// Only called when screen is shown the first time
	virtual void display() = 0;		// Display is called per frame?
	virtual void process() = 0;		// Process: any processing commands or algorithms
	void printBanner();
	String name;
	friend class ConsoleManager;	// Friend: Allows ConsoleManager to access all members of AConsole
};