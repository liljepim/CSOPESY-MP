#pragma once
#include "Typedef.h"
#include "AConsole.h"

class BaseConsole : public AConsole
{
public:
	BaseConsole(String processName);
	void onEnabled() override;
	void process() override;
	void display() override;
	std::shared_ptr<Process> getAttachedProcess();
private:
	void printProcessInfo() const;
	bool refreshed = false;
	std::shared_ptr<Process> attachedProcess;
	bool promptShown = false;
	String command = "";
};