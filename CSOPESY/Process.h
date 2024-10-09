#pragma once
#include "Typedef.h"
class Process
{
public:
	Process(String processName);

	String name;
	int currentLine;
	int totalLine;
	int processId;
private:
	static int totalCount;
};

