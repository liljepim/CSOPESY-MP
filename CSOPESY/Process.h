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
	time_t timeFinished;
private:
	static int totalCount;
};

