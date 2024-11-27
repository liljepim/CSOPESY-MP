#pragma once
#include <iostream>
#include <map>
#pragma once
#include <vector>
#include "Typedef.h"
#include "ConsoleManager.h"
#include "Process.h"
#include "BaseConsole.h"

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
	void runProcessesRR(std::shared_ptr<Process> runningProcess, int coreIndex);
	void runProcessesFCFS(std::shared_ptr<Process> runningProcess, int coreIndex);
	void startTester();
	void stopTester();
	std::string coreSummary();
	int getMax();
	int getMin();
	int memoryMap[4];
	std::vector<std::shared_ptr<Process>> readyQueue;
private:
	void generateDummy(ConsoleManager* cmInstance);
	static Scheduler* sharedInstance;
	std::map<String, int> configVars;
	String scheduler;
	std::vector<Process> processList;
	std::vector<int> coreList;
	Scheduler();
	~Scheduler() = default;
	Scheduler(Scheduler const&) {};
	Scheduler& operator=(Scheduler const&) {};
};