#pragma once

#include <Windows.h>

// could be usefull if using xed_ild_decode() to get size
// could then get away with not having fixed constants for instruction bytes, size and jumpback address
// could keep instruction bytes for a sanity check
class AssemblyHook
{
private:
	DWORD instructionAddress;
	DWORD** handlerAddress;
	DWORD jumpbackAddress;
	DWORD instructionSize;
	char* instructionBytes; 
	bool attached;
public:
	AssemblyHook(DWORD instructionAddress, DWORD** handlerAddress, DWORD jumpbackAddress, DWORD instructionSize, char* instructionBytes);
	~AssemblyHook();
	void Attach();
	void Detach();
};

