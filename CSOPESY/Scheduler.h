#pragma once
#include <iostream>
#include <map>
#include <vector>
#include "Typedef.h"
#include "ConsoleManager.h"
#include "Process.h"

typedef std::string String;

class Scheduler
{
public:
	static Scheduler* getInstance();
	static void initialize();
	static void destroy();
	static void varTest();

private:
	static Scheduler* sharedInstance;
	static std::map<String, int> configVars;
	static String scheduler;
	static std::vector<Process> processList;
	Scheduler();
	~Scheduler() = default;
	Scheduler(Scheduler const&) {};
	Scheduler& operator=(Scheduler const&) {};
	
};
