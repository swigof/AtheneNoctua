#include "Debugger.h"
#include <TlHelp32.h>
#include <stdio.h>

Debugger::Debugger()
{
	dwThreadID = 0;
	DWORD processID = GetCurrentProcessId();
	THREADENTRY32 entry;
	entry.dwSize = sizeof(THREADENTRY32);
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	bool searching = true;
	if (Thread32First(snapshot, &entry) == TRUE) {
		while (Thread32Next(snapshot, &entry) == TRUE && searching) {
			if (entry.th32OwnerProcessID == processID) {				
				dwThreadID = entry.th32ThreadID;
				searching = false;
			}
		}
	}
	CloseHandle(snapshot);
}

Debugger::Debugger(DWORD dwThreadID) 
{
	this->dwThreadID = dwThreadID;
}

Debugger::~Debugger()
{
}

void Debugger::SetHWBreakpoint(DWORD dwAddress) 
{
	printf("setting HWBP at 0x%08x\n", dwAddress);

	HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, this->dwThreadID);
	SuspendThread(hThread);
	CONTEXT ctx = { 0 };
	ctx.ContextFlags = CONTEXT_ALL;
	GetThreadContext(hThread, &ctx);

	if ((ctx.Dr7 & 1) == 0) {
		ctx.Dr0 = dwAddress;
		ctx.Dr7 |= 1;
	}
	else if ((ctx.Dr7 & (1 << 2)) == 0) {
		ctx.Dr1 = dwAddress;
		ctx.Dr7 |= (1 << 2);
	}
	else if ((ctx.Dr7 & (1 << 4)) == 0) {
		ctx.Dr2 = dwAddress;
		ctx.Dr7 |= (1 << 4);
	}
	else if ((ctx.Dr7 & (1 << 6)) == 0) {
		ctx.Dr3 = dwAddress;
		ctx.Dr7 |= (1 << 6);
	}

	SetThreadContext(hThread, &ctx);
	ResumeThread(hThread);
	CloseHandle(hThread);
}

void Debugger::SetHWBreakpoint(DWORD dwAddress, CONTEXT* ctxRecord)
{
	printf("setting HWBP at 0x%08x\n", dwAddress);

	if ((ctxRecord->Dr7 & 1) == 0) {
		ctxRecord->Dr0 = dwAddress;
		ctxRecord->Dr7 |= 1;
	}
	else if ((ctxRecord->Dr7 & (1 << 2)) == 0) {
		ctxRecord->Dr1 = dwAddress;
		ctxRecord->Dr7 |= (1 << 2);
	}
	else if ((ctxRecord->Dr7 & (1 << 4)) == 0) {
		ctxRecord->Dr2 = dwAddress;
		ctxRecord->Dr7 |= (1 << 4);
	}
	else if ((ctxRecord->Dr7 & (1 << 6)) == 0) {
		ctxRecord->Dr3 = dwAddress;
		ctxRecord->Dr7 |= (1 << 6);
	}
}

void Debugger::UnsetHWBreakpoint(DWORD dwAddress, CONTEXT *ctxRecord)
{
	printf("unsetting HWBP at 0x%08x\n", dwAddress);

	if (ctxRecord->Dr0 == dwAddress) {
		ctxRecord->Dr0 = 0;
		ctxRecord->Dr7 &= ~(1);
	}
	if (ctxRecord->Dr1 == dwAddress) {
		ctxRecord->Dr1 = 0;
		ctxRecord->Dr7 &= ~(1 << 2);
	}
	if (ctxRecord->Dr2 == dwAddress) {
		ctxRecord->Dr2 = 0;
		ctxRecord->Dr7 &= ~(1 << 4);
	}
	if (ctxRecord->Dr3 == dwAddress) {
		ctxRecord->Dr3 = 0;
		ctxRecord->Dr7 &= ~(1 << 6);
	}
}

void Debugger::SetResumeFlag(CONTEXT *ctxRecord)
{
	ctxRecord->EFlags |= (1 << 16);
}

void Debugger::ReadMemory(DWORD dwAddress, DWORD dwSize)
{
	//ReadProcessMemory(GetCurrentProcess(), dwAddress, , dwSize, );
	//return 
}