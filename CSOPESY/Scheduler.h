#pragma once
#include <iostream>
#include <map>

class Scheduler
{
public:
	static Scheduler* getInstance();
	static void initialize();
	static void destroy();

private:
	static Scheduler* sharedInstance;
	std::map<String, int> configVars = 
	{
		{"num-cpu", 0},
		{"quantum-cycles", 0},
		{"batch-process-freq", 0},
		{"min-ins", 0},
		{"mix-ins", 0},
		{"delay-per-exec", 0}
	};
	String scheduler;
}
