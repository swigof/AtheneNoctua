#include <windows.h>
#include <stdio.h>
#include "AtheneNoctua.h"
#include <xstddef>
#include "AssemblyHook.h"

DWORD mapID = 0;
DWORD channel = 0;

DWORD **handlerAddress;

__declspec(naked) void MapChangeHandler() {
	__asm mov[edi + 0x00000638], 0x00000001 // overwritten instruction

	__asm mov mapID, eax
	printf("Map changed to %u\n", int(mapID));

	__asm jmp MAP_CHANGE_JUMPBACK_ADDRESS
}

void AttachHook(DWORD address, DWORD instructionSize, void* handler) {
	if (instructionSize < 6) {
		printf("Failed to hook at %0x08x: instruction size of %u too small\n", address, instructionSize);
		return;
	}
	DWORD dwOldProtect;
	VirtualProtect((void*)address, instructionSize, PAGE_READWRITE, &dwOldProtect);

	handlerAddress = (DWORD**)&handler;
	BYTE handlerJump[] = {0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90};
	handlerJump[2] = ((BYTE*)&handlerAddress)[0];
	handlerJump[3] = ((BYTE*)&handlerAddress)[1];
	handlerJump[4] = ((BYTE*)&handlerAddress)[2];
	handlerJump[5] = ((BYTE*)&handlerAddress)[3];

	memcpy((void*)address, handlerJump, instructionSize);
	
	VirtualProtect((void*)address, instructionSize, dwOldProtect, &dwOldProtect);
	FlushInstructionCache(GetCurrentProcess(), (void*)address, instructionSize);
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

	AttachHook(MAP_CHANGE_ADDRESS, MAP_CHANGE_INSTRUCTION_SIZE, MapChangeHandler);
	//AssemblyHook mapChangeHook = AssemblyHook(MAP_CHANGE_ADDRESS, (DWORD**)&MapChangeHandler, MAP_CHANGE_JUMPBACK_ADDRESS, MAP_CHANGE_INSTRUCTION_SIZE, MAP_CHANGE_INSTRUCTION_BYTES);
	//mapChangeHook.Attach();

	while (true) {
		
	}

	printf("Exiting.\n");
}
