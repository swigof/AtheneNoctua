#include "Debugger.h"
#include <TlHelp32.h>
#include <stdio.h>

Debugger::Debugger()
{
	this->dwThreadID = 0;
	DWORD processID = GetCurrentProcessId();
	THREADENTRY32 entry;
	entry.dwSize = sizeof(THREADENTRY32);
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	bool searching = true;
	if (Thread32First(snapshot, &entry) == TRUE) {
		while (Thread32Next(snapshot, &entry) == TRUE && searching) {
			if (entry.th32OwnerProcessID == processID) {				
				this->dwThreadID = entry.th32ThreadID;
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

void Debugger::SetINT3Breakpoint(DWORD dwAddress)
{
	printf("setting INT3BP at 0x%08x\n", dwAddress);

	DWORD dwOldProtect;
	VirtualProtect(&dwAddress, 1, PAGE_READWRITE, &dwOldProtect);

	BYTE byteInstruction;
	memcpy(&byteInstruction, (void*)dwAddress, 1);
	INT3Breakpoint i3BP{dwAddress, byteInstruction};
	this->vecINT3Breakpoints.push_back(i3BP);
	memset((void*)dwAddress, INT3_INSTRUCTION, 1);

	VirtualProtect(&dwAddress, 1, dwOldProtect, &dwOldProtect);
	FlushInstructionCache(GetCurrentProcess(), &dwAddress, 1);
}

void Debugger::UnsetINT3Breakpoint(DWORD dwAddress)
{
	printf("unsetting INT3BP at 0x%08x\n", dwAddress);

	DWORD dwOldProtect;
	VirtualProtect(&dwAddress, 1, PAGE_READWRITE, &dwOldProtect);

	BYTE byteInstruction;
	for (auto it = vecINT3Breakpoints.begin(); it != vecINT3Breakpoints.end();) {
		if (it->dwAddress == dwAddress) {
			byteInstruction = it->byteInstruction;
			it = vecINT3Breakpoints.erase(it);
		}
		else {
			it++;
		}
	}
	memset((void*)dwAddress, byteInstruction, 1);

	VirtualProtect(&dwAddress, 1, dwOldProtect, &dwOldProtect);
	FlushInstructionCache(GetCurrentProcess(), &dwAddress, 1);
}

void Debugger::INT3Stepback(CONTEXT *ctxRecord) 
{
	printf("stepping back instruction pointer\n");
	ctxRecord->Eip--;
}

void Debugger::INT3UnsetStepback(CONTEXT *ctxRecord) 
{
	UnsetINT3Breakpoint(ctxRecord->Eip);
	INT3Stepback(ctxRecord);
}

void Debugger::SetSingleStepFlag(CONTEXT* ctxRecord) 
{
	printf("setting single step flag\n");
	ctxRecord->EFlags |= (1 << 8);
}

void Debugger::UnsetSingleStepFlag(CONTEXT* ctxRecord) 
{
	printf("unsetting single step flag\n");
	ctxRecord->EFlags &= ~(1 << 8);
}

void Debugger::SetResumeFlag(CONTEXT *ctxRecord)
{
	printf("setting resume flag\n");
	ctxRecord->EFlags |= (1 << 16);
}
