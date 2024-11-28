#include "Process.h"
#include "Scheduler.h"
#include <random>

int Process::totalCount = 1;

Process::Process(String processName)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> distrib(Scheduler::getInstance()->getMin(), Scheduler::getInstance()->getMax());
	std::uniform_int_distribution<> distribMem(Scheduler::getInstance()->getMemMin(), Scheduler::getInstance()->getMemMax());

	this->name = processName;
	this->processId = Process::totalCount;
	this->totalCount++;
	this->currentLine = 0;
	this->totalLine = distrib(gen);
	this->coreUsed = -1;
	this->requiredMem = distribMem(gen);
	this->memIndex = -1;
}

void Process::processBody(){
	std::cout<<"Hello, World " << this->currentLine;
}

Process::~Process()
{
	this->totalCount--;
}