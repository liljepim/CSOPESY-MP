#include "Process.h"
#include "Scheduler.h"
#include <random>

int Process::totalCount = 1;

Process::Process(String processName)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::random_device rdMem;
	std::mt19937 genMem(rdMem());
	std::uniform_int_distribution<> distrib(Scheduler::getInstance()->getMin(), Scheduler::getInstance()->getMax());
	std::uniform_int_distribution<> distribMem(Scheduler::getInstance()->getMemMin(), Scheduler::getInstance()->getMemMax());

	int memperf = Scheduler::getInstance()->configVars["mem-per-frame"];

	this->name = processName;
	this->processId = Process::totalCount;
	this->totalCount++;
	this->currentLine = 0;
	this->totalLine = distrib(gen);
	this->coreUsed = -1;
	this->requiredMem = 2 << (distribMem(genMem)-1);
	this->memIndex = -1;
	this->cycleAssigned = -1;
	this->isRunning = false;
	this->requiredFrames = (requiredMem / memperf) + (requiredMem % memperf != 0);
}

Process::~Process()
{
	this->totalCount--;
}