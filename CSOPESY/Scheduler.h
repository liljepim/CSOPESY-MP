#pragma once
#include <iostream>
#include <map>
#include <vector>
#include "Typedef.h"
#include "ConsoleManager.h"
#include "Process.h"

class Scheduler
{
public:
	static Scheduler* getInstance();
	static void initialize();
	static void destroy();
	void varTest();
	void readConfig();

private:
	static Scheduler* sharedInstance;
	std::map<String, int> configVars;
	String scheduler;
	static std::vector<Process> processList;
	Scheduler();
	~Scheduler() = default;
	Scheduler(Scheduler const&) {};
	Scheduler& operator=(Scheduler const&) {};
};
