#include <windows.h>
#include <iostream>
#include <tlhelp32.h>

#define STATUS_WX86_SINGLE_STEP 			0x4000001E
#define STATUS_WX86_BREAKPOINT				0x4000001F

std::wstring wsProcessName = L"notepad++.exe";

DWORD dwPID = 0;
DWORD dwTID = 0;
HANDLE hThread = nullptr;

DWORD dwListingInfoBreakpoint = 0x6B95AAAB; // 0x60EBDD; // listing count at [ESI] and price at [ESI+8]
DWORD dwItemInfoBreakpoint = 0x4B5A78; // item ID in EAX and on top of stack
DWORD dwShopOpenStartEndBreakpoint = 0x5C04BF; // executed once at shop open start then once at shop open end

HANDLE OpenProcessByName() {
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (Process32First(snapshot, &entry) == TRUE) {
		while (Process32Next(snapshot, &entry) == TRUE) {
			if (wcscmp(entry.szExeFile, wsProcessName.c_str()) == 0) {
				CloseHandle(snapshot);
				return OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
			}
		}
	}
	CloseHandle(snapshot);
	return nullptr;
}

DWORD GetProcessMainThreadID(DWORD const& processID) {
	THREADENTRY32 entry;
	entry.dwSize = sizeof(THREADENTRY32);
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (Thread32First(snapshot, &entry) == TRUE) {
		while (Thread32Next(snapshot, &entry) == TRUE) {
			if (entry.th32OwnerProcessID == processID) {
				CloseHandle(snapshot);
				return entry.th32ThreadID;
			}
		}
	}
	CloseHandle(snapshot);
	return 0;
}

bool EnableDebugPrivileges() {
	HANDLE hToken;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken)) {
		return false;
	}
	LUID luid;
	if (LookupPrivilegeValue(nullptr, SE_DEBUG_NAME, &luid)) {
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount = 1;
		tp.Privileges[0].Luid = luid;
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		if (!AdjustTokenPrivileges(hToken, false, &tp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES) nullptr,
			(PDWORD) nullptr)) {
			CloseHandle(hToken);
			return false;
		}
	}
	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED) {
		CloseHandle(hToken);
		return false;
	}
	CloseHandle(hToken);
	return true;
}

LONG WINAPI ExceptionHandler(struct _EXCEPTION_POINTERS *ExceptionInfo) {	
	std::cout << "handling exception";
	DWORD dwExcAddress = (DWORD)ExceptionInfo->ExceptionRecord->ExceptionAddress; 
	if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_BREAKPOINT || ExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_WX86_BREAKPOINT) {
		std::cout << "bp";
		CONTEXT ctx = *ExceptionInfo->ContextRecord;
		ctx.ContextFlags = CONTEXT_ALL;
		ctx.EFlags |= 16;
		SetThreadContext(hThread, &ctx);
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	else if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_SINGLE_STEP || ExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_WX86_SINGLE_STEP) {
		std::cout << "ss";
		CONTEXT ctx = *ExceptionInfo->ContextRecord;
		ctx.ContextFlags = CONTEXT_ALL;
		ctx.EFlags |= 16;
		SetThreadContext(hThread, &ctx);
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	return EXCEPTION_CONTINUE_SEARCH;
}

void addHWBreakpoint(const DWORD address) {
	SuspendThread(hThread);
	CONTEXT ctx = { 0 };
	ctx.ContextFlags = CONTEXT_ALL;
	GetThreadContext(hThread, &ctx);
	ctx.Dr7 |= 1;
	ctx.Dr0 = address;
	SetThreadContext(hThread, &ctx);
	ResumeThread(hThread);
	std::cout << "bp added";
}

int main() {
	HANDLE hProcess = OpenProcessByName();
	if (hProcess == nullptr) {
		std::cout << "PROCESS NOT FOUND" << "\n";
		return 0;
	}
	dwPID = GetProcessId(hProcess);
	CloseHandle(hProcess);

	dwTID = GetProcessMainThreadID(dwPID);
	if (dwTID == 0) {
		std::cout << "MAIN THREAD ID NOT FOUND" << "\n";
		return 0;
	}

	hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, dwTID);
	if (hThread == nullptr) {
		std::cout << "THREAD ACCESS DENIED " << GetLastError() << "\n";
		return 0;
	}

	if (!EnableDebugPrivileges()) {
		std::cout << "FAILED TO ACQUIRE DEBUG PRIVILEGES " << GetLastError() << "\n";
		return 0;
	}

	HANDLE hExceptionHandler = AddVectoredExceptionHandler(1, ExceptionHandler);
	if (hExceptionHandler == nullptr) {
		std::cout << "FAILED TO ATTACH EXCEPTION HANDLER " << GetLastError() << "\n";
		return 0;
	}

	addHWBreakpoint(dwListingInfoBreakpoint);

	// Needs some kind of dll injection 

	while (true) {

	}

	RemoveVectoredExceptionHandler(hExceptionHandler);
	CloseHandle(hExceptionHandler);
	CloseHandle(hThread);
	return 0;
}
