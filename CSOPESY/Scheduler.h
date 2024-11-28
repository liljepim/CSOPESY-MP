#pragma once
#include <iostream>
#include <map>
#pragma once
#include <vector>
#include "Typedef.h"
#include "ConsoleManager.h"
#include "Process.h"
#include "BaseConsole.h"
#include <thread>
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
	void coreSummary();
	int getMax();
	int getMin();
	std::vector<std::shared_ptr<Process>> readyQueue;
	std::vector<std::shared_ptr<Process>> finishedProcesses;
	unsigned int previousQQ = 0;
	unsigned int previousBF = 0;
	bool isOn = false;
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
	std::vector<std::thread> cpuCores;
	
};
