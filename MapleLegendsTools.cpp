#include <windows.h>
#include "MapleLegendsTools.h"
#include <TlHelp32.h>
#include <stdio.h>

HANDLE hThread = NULL;
bool boolShopOpenFlag = false;

// getting access violation, rights problem? some otther undefined behavior causing instructional problems?
// set bps in all threads?

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
// software bps?
// set returns bp #, use reset with this number to reset
void SetBreakpoint(DWORD address) {
	SuspendThread(hThread);
	CONTEXT ctx = { 0 };
	ctx.ContextFlags = CONTEXT_ALL;
	GetThreadContext(hThread, &ctx);
	ctx.Dr0 = address;
	ctx.Dr7 = 1;
	SetThreadContext(hThread, &ctx);
	ResumeThread(hThread);
}

//use trap flag method to be able to reset the same breakpoint after
LONG WINAPI ExceptionHandler(struct _EXCEPTION_POINTERS* ExceptionInfo) {
	DWORD dwExcAddress = (DWORD)ExceptionInfo->ExceptionRecord->ExceptionAddress;
	DWORD dwExcCode = ExceptionInfo->ExceptionRecord->ExceptionCode;
	printf("handling exception 0x%08x at 0x%08x\n", dwExcCode, dwExcAddress);
	if (dwExcCode == EXCEPTION_SINGLE_STEP || dwExcCode == STATUS_WX86_SINGLE_STEP) {
		if (dwExcAddress == SHOP_OPEN_INSTRUCTION) {
			if (!boolShopOpenFlag) {
				boolShopOpenFlag = true;
				CONTEXT ctx = *ExceptionInfo->ContextRecord;
				ctx.ContextFlags = CONTEXT_ALL;
				ctx.EFlags |= 16;
				ctx.Dr0 = LISTING_INFO_INSTRUCTION;
				ctx.Dr7 = 1;
				SetThreadContext(hThread, &ctx);
			}
			else {
				boolShopOpenFlag = false;
				CONTEXT ctx = *ExceptionInfo->ContextRecord;
				ctx.ContextFlags = CONTEXT_ALL;
				ctx.EFlags |= 16;
				ctx.Dr0 = ctx.Dr1 = ctx.Dr7 = 0;
				SetThreadContext(hThread, &ctx);
			}
		}
		if (dwExcAddress == LISTING_INFO_INSTRUCTION) {
			CONTEXT ctx = *ExceptionInfo->ContextRecord;
			ctx.ContextFlags = CONTEXT_ALL;
			ctx.EFlags |= 16;
			ctx.Dr0 = ITEM_INFO_INSTRUCTION;
			ctx.Dr1 = SHOP_OPEN_INSTRUCTION;
			ctx.Dr7 = 0b00000101;
			SetThreadContext(hThread, &ctx);
		}
		if (dwExcAddress == ITEM_INFO_INSTRUCTION) {
			CONTEXT ctx = *ExceptionInfo->ContextRecord;
			ctx.ContextFlags = CONTEXT_ALL;
			ctx.EFlags |= 16;
			ctx.Dr0 = LISTING_INFO_INSTRUCTION;
			SetThreadContext(hThread, &ctx);
		}
		else {
			CONTEXT ctx = *ExceptionInfo->ContextRecord;
			ctx.ContextFlags = CONTEXT_ALL;
			ctx.EFlags |= 16;
			ctx.Dr0 = 0;
			ctx.Dr7 = 0;
			SetThreadContext(hThread, &ctx);
		}
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

	SetBreakpoint(SHOP_OPEN_INSTRUCTION);

	while (true) {

	}

	CloseHandle(hThread);
	CloseHandle(hExceptionHandler);
}
