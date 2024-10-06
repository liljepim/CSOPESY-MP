#include <iostream>
#include "Typedef.h"

class AConsole
{
public:
	AConsole(String name);
	~AConsole() = default;

	String getName();
	virtual void onEnabled() = 0;
	virtual void display() = 0;
	virtual void process() = 0;

	String name;
	friend class ConsoleManager;
};