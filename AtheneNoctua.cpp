#include <windows.h>
#include <TlHelp32.h>
#include <stdio.h>
#include "AtheneNoctua.h"
#include "debugger/Debugger.h"

bool boolShopOpenFlag = false;
DWORD dwResetBP = 0;
Debugger debugger = Debugger();

LONG WINAPI ExceptionHandler(struct _EXCEPTION_POINTERS* ExceptionInfo) {
	DWORD dwExcAddress = (DWORD)ExceptionInfo->ExceptionRecord->ExceptionAddress;
	DWORD dwExcCode = ExceptionInfo->ExceptionRecord->ExceptionCode;

	printf("handling exception 0x%08x at 0x%08x\n", dwExcCode, dwExcAddress);

	if (dwExcCode == EXCEPTION_SINGLE_STEP || dwExcCode == STATUS_WX86_SINGLE_STEP) {
		if (dwResetBP != 0) {
			if (dwResetBP != dwExcAddress) {
				debugger.SetINT3Breakpoint(dwResetBP);
				debugger.UnsetSingleStepFlag(ExceptionInfo->ContextRecord);
				dwResetBP = 0;
			}
			else {
				debugger.SetSingleStepFlag(ExceptionInfo->ContextRecord);
			}
		}
		return EXCEPTION_CONTINUE_EXECUTION;
	}

	if (dwExcCode == EXCEPTION_BREAKPOINT || dwExcCode == STATUS_WX86_BREAKPOINT) {
		if (dwExcAddress == SHOP_OPEN_INSTRUCTION) {
			if (!boolShopOpenFlag) {
				boolShopOpenFlag = true;
				debugger.SetINT3Breakpoint(LISTING_INFO_INSTRUCTION);
			}
			else {
				boolShopOpenFlag = false;
				debugger.UnsetINT3Breakpoint(LISTING_INFO_INSTRUCTION);
			}
			dwResetBP = SHOP_OPEN_INSTRUCTION;
			debugger.UnsetINT3Breakpoint(SHOP_OPEN_INSTRUCTION);
			debugger.SetSingleStepFlag(ExceptionInfo->ContextRecord);
		}
		else if (dwExcAddress == LISTING_INFO_INSTRUCTION) {
			debugger.UnsetINT3Breakpoint(LISTING_INFO_INSTRUCTION);
			debugger.SetINT3Breakpoint(ITEM_INFO_INSTRUCTION);
		}
		else if (dwExcAddress == ITEM_INFO_INSTRUCTION) {
			debugger.UnsetINT3Breakpoint(ITEM_INFO_INSTRUCTION);
			debugger.SetINT3Breakpoint(LISTING_INFO_INSTRUCTION);
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

	printf("DLL loaded\n");

	HANDLE hExceptionHandler = AddVectoredExceptionHandler(1, ExceptionHandler);

	debugger.SetINT3Breakpoint(SHOP_OPEN_INSTRUCTION);

	while (true) {
		
	}

	RemoveVectoredExceptionHandler(hExceptionHandler);
	CloseHandle(hExceptionHandler);
	printf("Exiting.\n");
}
