#include "Process.h"

int Process::totalCount = 1;

Process::Process(String processName)
{
	this->name = processName;
	this->processId = Process::totalCount;
	this->totalCount++;
	this->currentLine = 0;
	this->totalLine = 100;
	
}

void Process::processBody(){
	std::cout<<"Hello, World " << this->currentLine;
}

Process::~Process()
{
	this->totalCount--;
}