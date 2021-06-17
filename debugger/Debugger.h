#pragma once

#include <windows.h>

class Debugger
{
private:
	HANDLE hThread;
public:
	Debugger();
	~Debugger();
	void SetHWBreakpoint(DWORD dwAddress);
	void ResetHWBreakpoint(DWORD dwAddress);
	void SetContinueFlag();
	void ReadMemory(DWORD dwAddress, DWORD dwSize);
};

