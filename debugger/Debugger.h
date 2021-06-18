#pragma once

#include <windows.h>

class Debugger
{
private:
	DWORD dwThreadID;
public:
	Debugger();
	Debugger(DWORD dwThreadID);
	~Debugger();
	void SetHWBreakpoint(DWORD dwAddress, bool bSuspend = true);
	void UnsetHWBreakpoint(DWORD dwAddress, bool bSuspend = true);
	void ReadMemory(DWORD dwAddress, DWORD dwSize);
};

