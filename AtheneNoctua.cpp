#include <stdio.h>
#include <Windows.h>
#include <time.h>
#include "AtheneNoctua.h"
#include "AssemblyHook.h"

playerdata playerData;

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
	__asm pushf
	__asm call SaveRegisters

	if (playerData.channel != regs.eax) {
		playerData.channel = regs.eax;
		playerData.changeFlags.channel = true;
		time(&playerData.timestamps.channel);
		printf("Channel changed to %u\n", int(playerData.channel));
	}

	__asm call RestoreRegisters
	__asm popf
	__asm jmp CHANNEL_CHANGE.next
}

__declspec(naked) void CharacterChangeHandler() {
	__asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop
	__asm pushf
	__asm call SaveRegisters

	if (memcmp(&playerData.characterName, (DWORD*)regs.edi + 1, 12)) {
		memcpy(&playerData.characterName, (DWORD*)regs.edi + 1, 12);
		playerData.changeFlags.characterName = true;
		time(&playerData.timestamps.characterName);
		printf("Character name changed to %.12s\n", playerData.characterName.bytes);	
	}

	__asm call RestoreRegisters
	__asm popf
	__asm jmp CHARACTER_CHANGE.next
}

__declspec(naked) void MapChangeHandler() {
	__asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop
	__asm pushf
	__asm call SaveRegisters

	if (playerData.mapID != regs.eax) {
		playerData.mapID = regs.eax;
		playerData.changeFlags.mapID = true;
		time(&playerData.timestamps.mapID);
		printf("Map ID changed to %u\n", int(playerData.mapID));
	}

	__asm call RestoreRegisters
	__asm popf
	__asm jmp MAP_CHANGE.next
}

__declspec(naked) void AreaNameChangeHandler() {
	__asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop
	__asm pushf
	__asm call SaveRegisters

	if (playerData.areaName.compare(0, *((DWORD*)regs.eax - 1), (char*)regs.eax)) {
		playerData.areaName.assign((char*)regs.eax, *((DWORD*)regs.eax - 1));
		playerData.changeFlags.areaName = true;
		time(&playerData.timestamps.areaName);
		printf("Area name changed to %s\n", playerData.areaName.c_str());
	}

	__asm call RestoreRegisters
	__asm popf
	__asm jmp AREA_NAME_CHANGE.next
}

__declspec(naked) void MapNameChangeHandler() {
	__asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop
	__asm pushf
	__asm call SaveRegisters

	if (playerData.mapName.compare(0, *((DWORD*)regs.eax - 1), (char*)regs.eax)) {
		playerData.mapName.assign((char*)regs.eax, *((DWORD*)regs.eax - 1));
		playerData.changeFlags.mapName = true;
		time(&playerData.timestamps.mapName);
		printf("Map name changed to %s\n", playerData.mapName.c_str());
	}

	__asm call RestoreRegisters
	__asm popf
	__asm jmp MAP_NAME_CHANGE.next
}

__declspec(naked) void OnMapHandler() {
	__asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop
	__asm pushf
	__asm call SaveRegisters

	if (!playerData.onMap) {
		playerData.onMap = true;
		playerData.changeFlags.onMap = true;
		time(&playerData.timestamps.onMap);
		printf("On map\n");
	}

	__asm call RestoreRegisters
	__asm popf
	__asm jmp ON_MAP.next
}

__declspec(naked) void OffMapHandler() {
	__asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop __asm nop
	__asm pushf
	__asm call SaveRegisters

	if (playerData.onMap) {
		playerData.onMap = false;
		playerData.changeFlags.onMap = true;
		time(&playerData.timestamps.onMap);
		printf("Off map\n");
	}

	__asm call RestoreRegisters
	__asm popf
	__asm jmp OFF_MAP.next
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
	AssemblyHook onMapHook = AssemblyHook(OnMapHandler, ON_MAP);
	onMapHook.Attach();
	AssemblyHook offMapHook = AssemblyHook(OffMapHandler, OFF_MAP);
	offMapHook.Attach();

	while (true) {
		Sleep(60000);
		// only send changed or send ping if nothing changed but still online
		// use lock of somekind to ensure not actively updating anything? 
		// or timer to wait out updates. concerns with channel to map name being grouped. locked together?
	}

	printf("Exiting\n");
}
