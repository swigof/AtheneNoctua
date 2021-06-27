#include <stdio.h>
#include "AtheneNoctua.h"
#include "AssemblyHook.h"

DWORD mapID = 0;
DWORD channel = 0;

__declspec(naked) void MapChangeHandler() {
	__asm mov[edi + 0x00000638], 0x00000001 // use 15 nops then replace with bytecode from class context

	__asm mov mapID, eax
	printf("Map changed to %u\n", int(mapID));

	__asm jmp MAP_CHANGE_JUMPBACK_ADDRESS
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

	AssemblyHook mapChangeHook = AssemblyHook(MAP_CHANGE_ADDRESS, MapChangeHandler, MAP_CHANGE_JUMPBACK_ADDRESS, MAP_CHANGE_INSTRUCTION_SIZE, MAP_CHANGE_INSTRUCTION_BYTES);
	mapChangeHook.Attach();

	while (true) {
		
	}

	printf("Exiting.\n");
}
