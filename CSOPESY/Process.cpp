#include "Process.h"

int Process::totalCount = 1;

Process::Process(String processName)
{
	this->name = processName;
	this->processId = Process::totalCount;
	this->totalCount++;
	this->currentLine = 0;
	this->totalLine = 1000;
	
}

Process::~Process()
{
	this->totalCount--;
}