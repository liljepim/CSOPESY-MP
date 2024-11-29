#pragma once
#include "Typedef.h"
#include <vector>

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
	int cycleAssigned;
	bool isRunning;
	int requiredFrames;
	std::vector<int> assignedFrames;
private:
	static int totalCount;
};

