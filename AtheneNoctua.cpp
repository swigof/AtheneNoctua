#include <stdio.h>
#include <Windows.h>
#include "AtheneNoctua.h"
#include "AssemblyHook.h"

// add online/offline/cs flag
bool updated = 0;
playerdata memoryPlayerData;
//check for changes in assembly handlers instead of externally, could hurt performance

struct {
	DWORD eax;
	DWORD ebx;
	DWORD ecx;
	DWORD edx;
	DWORD esi;
	DWORD edi;
	DWORD ebp;
	DWORD esp;
} regs;

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

	__asm mov memoryPlayerData.channel, eax

	__asm jmp CHANNEL_CHANGE.next
}

__declspec(naked) void CharacterChangeHandler() {
	__asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop
	__asm mov regs.eax, eax

	__asm mov eax, [edi + 4]
	__asm mov memoryPlayerData.characterName, eax
	__asm mov eax, [edi + 8]
	__asm mov memoryPlayerData.characterName + 4, eax
	__asm mov eax, [edi + 12]
	__asm mov memoryPlayerData.characterName + 8, eax

	__asm mov eax, regs.eax
	__asm jmp CHARACTER_CHANGE.next
}

__declspec(naked) void MapChangeHandler() {
	__asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop

	__asm mov memoryPlayerData.mapID, eax

	__asm jmp MAP_CHANGE.next
}

__declspec(naked) void AreaNameChangeHandler() {
	__asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop
	__asm pushf
	__asm call SaveRegisters

	__asm mov ebx, [eax - 4]
	__asm mov memoryPlayerData.areaName.length, ebx
	__asm mov memoryPlayerData.areaName.address, eax
	memoryPlayerData.areaName.str.assign(memoryPlayerData.areaName.address, memoryPlayerData.areaName.length);

	__asm call RestoreRegisters
	__asm popf
	__asm jmp AREA_NAME_CHANGE.next
}

__declspec(naked) void MapNameChangeHandler() {
	__asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop
	__asm pushf
	__asm call SaveRegisters

	__asm mov ebx, [eax - 4]
	__asm mov memoryPlayerData.mapName.length, ebx
	__asm mov memoryPlayerData.mapName.address, eax
	memoryPlayerData.mapName.str.assign(memoryPlayerData.mapName.address, memoryPlayerData.mapName.length);
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

	AssemblyHook mapChangeHook = AssemblyHook(MapChangeHandler, MAP_CHANGE);
	mapChangeHook.Attach();
	AssemblyHook channelChangeHook = AssemblyHook(ChannelChangeHandler, CHANNEL_CHANGE);
	channelChangeHook.Attach();
	AssemblyHook characterChangeHook = AssemblyHook(CharacterChangeHandler, CHARACTER_CHANGE);
	characterChangeHook.Attach();
	AssemblyHook mapNameChangeHook = AssemblyHook(MapNameChangeHandler, MAP_NAME_CHANGE);
	mapNameChangeHook.Attach();
	AssemblyHook areaNameChangeHook = AssemblyHook(AreaNameChangeHandler, AREA_NAME_CHANGE);
	areaNameChangeHook.Attach();

	playerdata stablePlayerData;
	while (true) {
		Sleep(1000);
		// update delta
		if (updated) {
			updated = 0;
			// extract below behaviors to functions
			if (memoryPlayerData.mapID != stablePlayerData.mapID) {
				stablePlayerData.mapID = memoryPlayerData.mapID;
				stablePlayerData.changeFlags.mapID = true;
				if (memoryPlayerData.mapName.str.compare(stablePlayerData.mapName.str)) {
					stablePlayerData.mapName = memoryPlayerData.mapName;
					stablePlayerData.changeFlags.mapName = true;
				}
				if (memoryPlayerData.areaName.str.compare(stablePlayerData.areaName.str)) {
					stablePlayerData.areaName = memoryPlayerData.areaName;
					stablePlayerData.changeFlags.areaName = true;
					printf("Area changed to %s\n", stablePlayerData.areaName.str.c_str());
				}
				printf("Map changed to %s (%u)\n", stablePlayerData.mapName.str.c_str(), int(stablePlayerData.mapID));
			}
			if (memoryPlayerData.channel != stablePlayerData.channel) {
				stablePlayerData.channel = memoryPlayerData.channel;
				printf("Channel changed to %u\n", int(stablePlayerData.channel));
			}
			if (memcmp(&memoryPlayerData.characterName, &stablePlayerData.characterName, 12)) {
				stablePlayerData.characterName = memoryPlayerData.characterName;
				printf("Character name changed to %.12s\n", stablePlayerData.characterName.bytes);
			}
		}
		// else if delta 1 minute, only send changed
		// or send ping if nothing changed but still online
		// use oldPlayer 
	}

	printf("Exiting\n");
}
