#pragma once

#define INT3_INSTRUCTION 0xCC

#include <windows.h>
#include <vector>

struct INT3Breakpoint {
	DWORD dwAddress;
	BYTE byteInstruction;
};

class Debugger
{
private:
	DWORD dwThreadID;
	std::vector<INT3Breakpoint> vecINT3Breakpoints;
public:
	Debugger();
	Debugger(DWORD dwThreadID);
	~Debugger();
	void SetHWBreakpoint(DWORD dwAddress);
	void SetHWBreakpoint(DWORD dwAddress, CONTEXT* ctxRecord);
	void UnsetHWBreakpoint(DWORD dwAddress, CONTEXT* ctxRecord);
	void SetINT3Breakpoint(DWORD dwAddress);
	void UnsetINT3Breakpoint(DWORD dwAddress);
	void INT3Stepback(CONTEXT* ctxRecord);
	void INT3UnsetStepback(CONTEXT* ctxRecord);
	void SetSingleStepFlag(CONTEXT* ctxRecord);
	void UnsetSingleStepFlag(CONTEXT* ctxRecord);
	void SetResumeFlag(CONTEXT* ctxRecord);
};
