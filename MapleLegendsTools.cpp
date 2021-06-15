#include <windows.h>
#include "MapleLegendsTools.h"
#include <TlHelp32.h>
#include <stdio.h>

HANDLE hThread = NULL;

// getting access violation, need right elevation? uac? 

// seperate relevant functions into a debugger class
HANDLE OpenMainThread() {
	DWORD processID = GetCurrentProcessId();
	THREADENTRY32 entry;
	entry.dwSize = sizeof(THREADENTRY32);
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (Thread32First(snapshot, &entry) == TRUE) {
		while (Thread32Next(snapshot, &entry) == TRUE) {
			if (entry.th32OwnerProcessID == processID) {
				CloseHandle(snapshot);
				return OpenThread(THREAD_ALL_ACCESS, FALSE, entry.th32ThreadID);
			}
		}
	}
	CloseHandle(snapshot);
	return NULL;
}

LONG WINAPI ExceptionHandler(struct _EXCEPTION_POINTERS* ExceptionInfo) {
	printf("handling exception 0x%08x\n", ExceptionInfo->ExceptionRecord->ExceptionCode);
	DWORD dwExcAddress = (DWORD)ExceptionInfo->ExceptionRecord->ExceptionAddress;
	if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_BREAKPOINT || ExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_WX86_BREAKPOINT) {
		CONTEXT ctx = *ExceptionInfo->ContextRecord;
		ctx.ContextFlags = CONTEXT_ALL;
		ctx.EFlags |= 16;
		SetThreadContext(hThread, &ctx);
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	else if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_SINGLE_STEP || ExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_WX86_SINGLE_STEP) {
		CONTEXT ctx = *ExceptionInfo->ContextRecord;
		ctx.ContextFlags = CONTEXT_ALL;
		ctx.EFlags |= 16;
		ctx.Dr0 = 0;
		ctx.Dr7 = 0;
		//SuspendThread(hThread);
		SetThreadContext(hThread, &ctx);
		//ResumeThread(hThread);

		return EXCEPTION_CONTINUE_EXECUTION;
	}
	return EXCEPTION_CONTINUE_SEARCH;
}

void StartTools() {
	#ifdef _DEBUG 
	if (AllocConsole()) {
		freopen("CONOUT$", "w", stdout);
		SetConsoleTitle("Console");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	}
	#endif

	printf("DLL loaded.\n");

	hThread = OpenMainThread();

	HANDLE hExceptionHandler = AddVectoredExceptionHandler(1, ExceptionHandler);

	SuspendThread(hThread);
	CONTEXT ctx = { 0 };
	ctx.ContextFlags = CONTEXT_ALL;
	GetThreadContext(hThread, &ctx);
	ctx.Dr7 |= 1;
	ctx.Dr0 = LISTING_INFO_INSTRUCTION;
	SetThreadContext(hThread, &ctx);
	ResumeThread(hThread);

	while (true) {

	}

	CloseHandle(hThread);
	CloseHandle(hExceptionHandler);
	//get main thread of self process?
	//just add vectored exception handler bababooey
	//try HW bps? 
}
