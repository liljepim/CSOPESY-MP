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
private:
	
	static int totalCount;
};

