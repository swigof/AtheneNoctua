#include <windows.h>
#include <iostream>
#include <tlhelp32.h>

#define STATUS_WX86_SINGLE_STEP 			0x4000001E
#define STATUS_WX86_BREAKPOINT				0x4000001F

std::wstring wsProcessName = L"MapleLegends.exe";
DWORD dwPID = 0;
DWORD dwTID = 0;

DWORD dwListingInfoBreakpoint = 0x60EBDD; // listing count at [ESI] and price at [ESI+8]
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

LONG __stdcall ExceptionHandler(PEXCEPTION_POINTERS ExceptionInfo) {
	//todo
	return EXCEPTION_CONTINUE_EXECUTION;
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

	HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, dwTID);
	if (hThread == nullptr) {
		std::cout << "THREAD ACCESS DENIED " << GetLastError() << "\n";
		return 0;
	}

	if (!EnableDebugPrivileges()) {
		std::cout << "FAILED TO ACQUIRE DEBUG PRIVILEGES " << GetLastError() << "\n";
		return 0;
	}

	// todo

	CloseHandle(hThread);
	return 0;
}
