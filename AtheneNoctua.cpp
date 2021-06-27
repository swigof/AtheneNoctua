#include <stdio.h>
#include "AtheneNoctua.h"
#include "AssemblyHook.h"

DWORD mapID = 0;
DWORD channel = 0; // doesn't grab on first load

__declspec(naked) void MapChangeHandler() {
	__asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop

	__asm mov mapID, eax
	printf("Map changed to %u\n", int(mapID));

	__asm jmp MAP_CHANGE.next
}

__declspec(naked) void ChannelChangeHandler() {
	__asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop

	__asm mov channel, eax
	printf("Channel changed to %u\n", int(channel));

	__asm jmp CHANNEL_CHANGE.next
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

	AssemblyHook mapChangeHook = AssemblyHook(MAP_CHANGE.address, MapChangeHandler, MAP_CHANGE.next, MAP_CHANGE.size, MAP_CHANGE.bytes);
	mapChangeHook.Attach();

	AssemblyHook channelChangeHook = AssemblyHook(CHANNEL_CHANGE.address, ChannelChangeHandler, CHANNEL_CHANGE.next, CHANNEL_CHANGE.size, CHANNEL_CHANGE.bytes);
	channelChangeHook.Attach();

	while (true) {}

	printf("Exiting\n");
}
