#include <stdio.h>
#include "AtheneNoctua.h"
#include "AssemblyHook.h"

registers regs = {};
bool updated = 0;
DWORD mapID = -1;
DWORD channel = -1;
character_name characterName = {};
memory_string mapName = {};
memory_string areaName = {};

__declspec(naked) void SaveRegisters(){
	__asm mov regs.eax, eax
	__asm mov regs.ebx, ebx
	__asm mov regs.ecx, ecx
	__asm mov regs.edx, edx
	__asm mov regs.esi, esi
	__asm mov regs.edi, edi
	__asm mov regs.ebp, ebp
	__asm mov regs.esp, esp
	__asm ret
}

__declspec(naked) void RestoreRegisters() {
	__asm mov eax, regs.eax
	__asm mov ebx, regs.ebx
	__asm mov ecx, regs.ecx
	__asm mov edx, regs.edx
	__asm mov esi, regs.esi
	__asm mov edi, regs.edi
	__asm mov ebp, regs.ebp
	__asm mov esp, regs.esp
	__asm ret
}

__declspec(naked) void ChannelChangeHandler() {
	__asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop

	__asm mov channel, eax

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

	__asm mov eax, regs.eax
	__asm jmp CHARACTER_CHANGE.next
}

__declspec(naked) void MapChangeHandler() {
	__asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop

	__asm mov mapID, eax

	__asm jmp MAP_CHANGE.next
}

__declspec(naked) void AreaNameChangeHandler() {
	__asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop
	__asm pushf
	__asm call SaveRegisters

	__asm mov ebx, [eax - 4]
	__asm mov areaName.length, ebx
	__asm mov areaName.address, eax
	areaName.str.assign(areaName.address, areaName.length);

	__asm call RestoreRegisters
	__asm popf
	__asm jmp AREA_NAME_CHANGE.next
}

__declspec(naked) void MapNameChangeHandler() {
	__asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop
	__asm pushf
	__asm call SaveRegisters

	__asm mov ebx, [eax - 4]
	__asm mov mapName.length, ebx
	__asm mov mapName.address, eax
	mapName.str.assign(mapName.address, mapName.length);
	__asm mov updated, 1

	__asm call RestoreRegisters
	__asm popf
	__asm jmp MAP_NAME_CHANGE.next
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

	AssemblyHook mapNameChangeHook = AssemblyHook(MAP_NAME_CHANGE.address, MapNameChangeHandler, MAP_NAME_CHANGE.next, MAP_NAME_CHANGE.size, MAP_NAME_CHANGE.bytes);
	mapNameChangeHook.Attach();

	AssemblyHook areaNameChangeHook = AssemblyHook(AREA_NAME_CHANGE.address, AreaNameChangeHandler, AREA_NAME_CHANGE.next, AREA_NAME_CHANGE.size, AREA_NAME_CHANGE.bytes);
	areaNameChangeHook.Attach();

	DWORD oldMapID = -1;
	DWORD oldChannel = -1;
	character_name oldCharacterName = {};
	std::string oldAreaNameStr = "";

	while (true) {
		Sleep(1000);
		if (updated) {
			updated = 0;
			if (mapID != oldMapID) {
				oldMapID = mapID;
				printf("Map changed to %s (%u)\n", mapName.str.c_str(), int(mapID));
				if (areaName.str.compare(oldAreaNameStr)) {
					oldAreaNameStr = areaName.str;
					printf("Area changed to %s\n", areaName.str.c_str());
				}
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
