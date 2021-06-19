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
	void SetHWBreakpoint(DWORD dwAddress);
	void SetHWBreakpoint(DWORD dwAddress, CONTEXT* ctxRecord);
	void UnsetHWBreakpoint(DWORD dwAddress, CONTEXT* ctxRecord);
	void SetResumeFlag(CONTEXT* ctxRecord);
	void ReadMemory(DWORD dwAddress, DWORD dwSize);
};

