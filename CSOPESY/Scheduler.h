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
	void startTester();
	void stopTester();

private:
	static Scheduler* sharedInstance;
	std::map<String, int> configVars;
	String scheduler;
	std::vector<Process> processList;
	std::vector<std::shared_ptr<Process>> readyQueue;
	std::vector<int> coreList;
	Scheduler();
	~Scheduler() = default;
	Scheduler(Scheduler const&) {};
	Scheduler& operator=(Scheduler const&) {};
	void generateDummy();
};
