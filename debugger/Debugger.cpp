#include "Debugger.h"
#include <TlHelp32.h>
#include <stdio.h>

// thread handle not null, same thread id as previously, just doesn't work.
Debugger::Debugger()
{
	this->hThread = NULL;
	DWORD processID = GetCurrentProcessId();
	THREADENTRY32 entry;
	entry.dwSize = sizeof(THREADENTRY32);
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	bool searching = true;
	if (Thread32First(snapshot, &entry) == TRUE) {
		while (Thread32Next(snapshot, &entry) == TRUE && searching) {
			if (entry.th32OwnerProcessID == processID) {
				this->hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, entry.th32ThreadID);
				searching = false;
			}
		}
	}
	CloseHandle(snapshot);
}

Debugger::~Debugger()
{
	CloseHandle(this->hThread);
}

void Debugger::SetHWBreakpoint(DWORD dwAddress)
{
	SuspendThread(this->hThread);
	CONTEXT ctx = { 0 };
	GetThreadContext(this->hThread, &ctx);
	ctx.ContextFlags = CONTEXT_ALL;
	
	if ((ctx.Dr7 & 0b00000001) == 0) {
		ctx.Dr0 = dwAddress;
		ctx.Dr7 |= 0b00000001;
	}
	else if ((ctx.Dr7 & 0b00000100) == 0) {
		ctx.Dr1 = dwAddress;
		ctx.Dr7 |= 0b00000100;
	}
	else if ((ctx.Dr7 & 0b00010000) == 0) {
		ctx.Dr2 = dwAddress;
		ctx.Dr7 |= 0b00010000;
	}
	else if ((ctx.Dr7 & 0b01000000) == 0) {
		ctx.Dr3 = dwAddress;
		ctx.Dr7 |= 0b01000000;
	}

	SetThreadContext(this->hThread, &ctx);
	ResumeThread(this->hThread);
}

void Debugger::ResetHWBreakpoint(DWORD dwAddress)
{
	SuspendThread(this->hThread);
	CONTEXT ctx = { 0 };
	GetThreadContext(this->hThread, &ctx);
	ctx.ContextFlags = CONTEXT_ALL;

	if (ctx.Dr0 == dwAddress) {
		ctx.Dr0 = 0;
		ctx.Dr7 &= ~(0b00000001);
	}
	if (ctx.Dr1 == dwAddress) {
		ctx.Dr1 = 0;
		ctx.Dr7 &= ~(0b00000100);
	}
	if (ctx.Dr2 == dwAddress) {
		ctx.Dr2 = 0;
		ctx.Dr7 &= ~(0b00010000);
	}
	if (ctx.Dr3 == dwAddress) {
		ctx.Dr3 = 0;
		ctx.Dr7 &= ~(0b01000000);
	}

	SetThreadContext(this->hThread, &ctx);
	ResumeThread(this->hThread);
}

void Debugger::SetContinueFlag() {
	SuspendThread(this->hThread);
	CONTEXT ctx = { 0 };
	GetThreadContext(this->hThread, &ctx);
	ctx.ContextFlags = CONTEXT_ALL;
	ctx.EFlags |= 0b1000000000000000;
	SetThreadContext(this->hThread, &ctx);
	ResumeThread(this->hThread);
}

void Debugger::ReadMemory(DWORD dwAddress, DWORD dwSize)
{
	//ReadProcessMemory(GetCurrentProcess(), dwAddress, , dwSize, );
	//return 
}