#pragma once

#include <Windows.h>

// adjust to not keep track of duplicate vars from constants
class AssemblyHook
{
private:
	bool attached;
public:
	DWORD instructionAddress;
	DWORD handlerAddress;
	DWORD jumpbackAddress;
	DWORD instructionSize;
	char* instructionBytes;
	AssemblyHook(DWORD instructionAddress, void* handler, DWORD jumpbackAddress, DWORD instructionSize, char* instructionBytes);
	~AssemblyHook();
	void Attach();
	void Detach();
};

