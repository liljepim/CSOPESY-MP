#pragma once
#include <unordered_map>
#include <Windows.h>
#include "Typedef.h"
#include "AConsole.h"
#include "BaseConsole.h"
#include "MainConsole.h"

class FlatMemoryAllocator
{
public:
	static FlatMemoryAllocator* getInstance();
	static void initialize();
	static void destroy();
	void allocate(std::shared_ptr<Process> newProcess);
	void deallocate(std::shared_ptr<Process> newProcess);
	std::vector<int> memoryMap;
private:
	static FlatMemoryAllocator* sharedInstance;
	FlatMemoryAllocator();
	~FlatMemoryAllocator() = default;
	FlatMemoryAllocator(FlatMemoryAllocator const&) {};
};
