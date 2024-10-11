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
	void registerProcess(std::shared_ptr<Process> newProcess);
	void assignProcesses();
	void runProcesses(std::shared_ptr<Process> runningProcess, int coreIndex);

private:
	static Scheduler* sharedInstance;
	std::map<String, int> configVars;
	String scheduler;
	std::vector<Process> processList;
	std::vector<std::shared_ptr<Process>> readyQueue;
	int coreList[4] = {-1, -1, -1, -1};
	Scheduler();
	~Scheduler() = default;
	Scheduler(Scheduler const&) {};
	Scheduler& operator=(Scheduler const&) {};
};
