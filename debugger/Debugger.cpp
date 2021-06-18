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

void Debugger::SetHWBreakpoint(DWORD dwAddress, bool bSuspend)
{
	printf("setting HWBP at 0x%08x\n", dwAddress);

	HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, this->dwThreadID);
	if (bSuspend) {
		SuspendThread(hThread);
	}
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
	if (bSuspend) {
		ResumeThread(hThread);
	}
	CloseHandle(hThread);
}

void Debugger::UnsetHWBreakpoint(DWORD dwAddress, bool bSuspend)
{
	printf("unsetting HWBP at 0x%08x\n", dwAddress);

	HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, this->dwThreadID);
	if (bSuspend) {
		SuspendThread(hThread);
	}
	CONTEXT ctx = { 0 };
	ctx.ContextFlags = CONTEXT_ALL;
	GetThreadContext(hThread, &ctx);

	if (ctx.Dr0 == dwAddress) {
		ctx.Dr0 = 0;
		ctx.Dr7 &= ~(1);
	}
	if (ctx.Dr1 == dwAddress) {
		ctx.Dr1 = 0;
		ctx.Dr7 &= ~(1 << 2);
	}
	if (ctx.Dr2 == dwAddress) {
		ctx.Dr2 = 0;
		ctx.Dr7 &= ~(1 << 4);
	}
	if (ctx.Dr3 == dwAddress) {
		ctx.Dr3 = 0;
		ctx.Dr7 &= ~(1 << 6);
	}

	SetThreadContext(hThread, &ctx);
	if (bSuspend) {
		ResumeThread(hThread);
	}
	CloseHandle(hThread);
}

void Debugger::ReadMemory(DWORD dwAddress, DWORD dwSize)
{
	//ReadProcessMemory(GetCurrentProcess(), dwAddress, , dwSize, );
	//return 
}