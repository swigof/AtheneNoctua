#include <windows.h>
#include <TlHelp32.h>
#include <stdio.h>
#include "AtheneNoctua.h"
#include "debugger/Debugger.h"

bool boolShopOpenFlag = false;
Debugger debugger;

LONG WINAPI ExceptionHandler(struct _EXCEPTION_POINTERS* ExceptionInfo) {
	DWORD dwExcAddress = (DWORD)ExceptionInfo->ExceptionRecord->ExceptionAddress;
	DWORD dwExcCode = ExceptionInfo->ExceptionRecord->ExceptionCode;

	printf("handling exception 0x%08x at 0x%08x\n", dwExcCode, dwExcAddress);

	if (dwExcCode == EXCEPTION_SINGLE_STEP || dwExcCode == STATUS_WX86_SINGLE_STEP) {
		if (dwExcAddress == SHOP_OPEN_INSTRUCTION) {
			if (!boolShopOpenFlag) {
				boolShopOpenFlag = true;
				debugger.SetHWBreakpoint(LISTING_INFO_INSTRUCTION);
				debugger.SetContinueFlag();
			}
			else {
				boolShopOpenFlag = false;
				debugger.ResetHWBreakpoint(LISTING_INFO_INSTRUCTION);
				debugger.SetContinueFlag();
			}
		}
		else if (dwExcAddress == LISTING_INFO_INSTRUCTION) {
			debugger.ResetHWBreakpoint(LISTING_INFO_INSTRUCTION);
			debugger.SetHWBreakpoint(ITEM_INFO_INSTRUCTION);
			debugger.SetContinueFlag();
		}
		else if (dwExcAddress == ITEM_INFO_INSTRUCTION) {
			debugger.ResetHWBreakpoint(ITEM_INFO_INSTRUCTION);
			debugger.SetHWBreakpoint(LISTING_INFO_INSTRUCTION);
			debugger.SetContinueFlag();
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

	HANDLE hExceptionHandler = AddVectoredExceptionHandler(1, ExceptionHandler);
	debugger = Debugger();
	debugger.SetHWBreakpoint(SHOP_OPEN_INSTRUCTION);

	while (true) {
		
	}

	CloseHandle(hExceptionHandler);
	printf("Exiting.\n");
}
