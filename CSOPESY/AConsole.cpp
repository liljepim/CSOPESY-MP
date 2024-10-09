#include "AConsole.h"

AConsole::AConsole(String processName)
{
	this->name = processName;
}

String AConsole::getName()
{
	return this->name;
}
