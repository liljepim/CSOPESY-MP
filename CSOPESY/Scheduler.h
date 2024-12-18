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
	void assignProcesses_Paging();
	void runProcessesRR_Paging(std::shared_ptr<Process> runningProcess, int coreIndex);
	void runProcessesFCFS_Paging(std::shared_ptr<Process> runningProcess, int coreIndex);
	void startTester();
	void stopTester();
	void coreSummary();
	std::map<String, int> configVars;
	int getMax();
	int getMin();
	int getMemMax();
	int getMemMin();
	std::vector<std::shared_ptr<Process>> readyQueue;
	std::vector<std::shared_ptr<Process>> finishedProcesses;
	unsigned int previousQQ = 0;
	unsigned int previousBF = 0;
	bool isOn = false;
	void saveSummary();
	void getCpuUtil();
	void generateSMI();
	int powMin;
	int powMax;
	void genstat();
	bool isPaging;
	int idleticks;
	int activeticks;
	int numPagein;
	int numPageout;
private:
	void generateDummy(ConsoleManager* cmInstance);
	static Scheduler* sharedInstance;
	String scheduler;
	std::vector<Process> processList;
	std::vector<int> coreList;
	Scheduler();
	~Scheduler() = default;
	Scheduler(Scheduler const&) {};
	Scheduler& operator=(Scheduler const&) {};
	std::vector<std::thread> cpuCores;
	std::string prevSummary;

};
