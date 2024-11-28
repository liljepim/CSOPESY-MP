#pragma once
#include "Typedef.h"


class Process
{
public:
	Process(String processName);
	~Process();
	String name;
	int currentLine;
	int totalLine;
	int processId;
	time_t timeCreated;
	time_t lastExecuted;
	time_t timeFinished;
	void processBody();
	int coreUsed;
	int requiredMem;
	int memIndex;
private:
	static int totalCount;
};

