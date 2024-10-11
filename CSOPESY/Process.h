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
	time_t timeQueued = 0;
	time_t timeFinished= 0;

	String getTime();
private:
	static int totalCount;
};

