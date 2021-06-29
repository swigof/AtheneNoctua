#include <stdio.h>
#include "AtheneNoctua.h"
#include "AssemblyHook.h"

registers regs = {};
bool updated = 0;
DWORD mapID = -1;
DWORD channel = -1;
character_name characterName = {};

__declspec(naked) void MapChangeHandler() {
	__asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop

	__asm mov mapID, eax
	__asm mov updated, 1

	__asm jmp MAP_CHANGE.next
}

__declspec(naked) void ChannelChangeHandler() {
	__asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop

	__asm mov channel, eax
	__asm mov updated, 1

	__asm jmp CHANNEL_CHANGE.next
}

__declspec(naked) void CharacterChangeHandler() {
	__asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop
	__asm mov regs.eax, eax

	__asm mov eax, [edi + 4]
	__asm mov characterName, eax
	__asm mov eax, [edi + 8]
	__asm mov characterName + 4, eax
	__asm mov eax, [edi + 12]
	__asm mov characterName + 8, eax
	__asm mov updated, 1

	__asm mov eax, regs.eax
	__asm jmp CHARACTER_CHANGE.next
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

	AssemblyHook characterChangeHook = AssemblyHook(CHARACTER_CHANGE.address, CharacterChangeHandler, CHARACTER_CHANGE.next, CHARACTER_CHANGE.size, CHARACTER_CHANGE.bytes);
	characterChangeHook.Attach();

	DWORD oldMapID = -1;
	DWORD oldChannel = -1;
	character_name oldCharacterName = {};

	while (true) {
		Sleep(1000);
		if (updated) {
			updated = 0;
			if (mapID != oldMapID) {
				oldMapID = mapID;
				printf("Map changed to %u\n", int(mapID));
			}
			if (channel != oldChannel) {
				oldChannel = channel;
				printf("Channel changed to %u\n", int(channel));
			}
			if (memcmp(&characterName, &oldCharacterName, 12)) {
				oldCharacterName = characterName;
				printf("Character name changed to %.12s\n", characterName.bytes);
			}
		}
	}

	printf("Exiting\n");
}
