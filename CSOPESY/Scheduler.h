#pragma once
#include <iostream>
#include <map>
#include "Typedef.h"
#include "ConsoleManager.h"

typedef std::string String;

class Scheduler
{
public:
	static Scheduler* getInstance();
	static void initialize();
	static void destroy();

private:
	static Scheduler* sharedInstance;
	static std::map<String, int> configVars;
	static String scheduler;
	Scheduler();
	~Scheduler() = default;
	Scheduler(Scheduler const&) {};
	Scheduler& operator=(Scheduler const&) {};
	
};
