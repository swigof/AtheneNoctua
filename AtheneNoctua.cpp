#include <windows.h>
#include <stdio.h>
#include "AtheneNoctua.h"
#include <xstddef>

DWORD mapID = 0;
DWORD channel = 0;

DWORD mapchangejmpback = 0x006DBC4A;
BYTE handlerAddressBytes[] = { 0x00, 0x00, 0x00, 0x00 };

__declspec(naked) void MapChangeHandler() { //make class for this?, 15 nop header, write to it with byte array of old instruction, jmp var trailer where var is calc from og addr + size
	__asm mov [edi + 0x00000638], 0x00000001
	__asm mov mapID, eax
	printf("Map changed to %u\n", int(mapID));

	__asm jmp mapchangejmpback
}

void AttachHook(DWORD address, DWORD instructionSize, void* handler) {
	if (instructionSize < 6) {
		printf("Failed to hook at %0x08x: instruction size of %u too small\n", address, instructionSize);
		return;
	}
	DWORD dwOldProtect;
	VirtualProtect((void*)address, instructionSize, PAGE_READWRITE, &dwOldProtect);

	BYTE instruction[15];
	memcpy(instruction, (void*)address, instructionSize);

	printf("handler pointer %p\n", handler);
	printf("handler address bytes pointer %p\n", handlerAddressBytes);

	// all this is disgusting find a cleaner way to do it
	handlerAddressBytes[0] = ((BYTE*)&handler)[0];
	handlerAddressBytes[1] = ((BYTE*)&handler)[1];
	handlerAddressBytes[2] = ((BYTE*)&handler)[2];
	handlerAddressBytes[3] = ((BYTE*)&handler)[3];
	void* handlerAddressBytesAddress = std::addressof(handlerAddressBytes);
	BYTE handlerJump[] = {0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90};
	handlerJump[2] = ((BYTE*)&handlerAddressBytesAddress)[0];
	handlerJump[3] = ((BYTE*)&handlerAddressBytesAddress)[1];
	handlerJump[4] = ((BYTE*)&handlerAddressBytesAddress)[2];
	handlerJump[5] = ((BYTE*)&handlerAddressBytesAddress)[3];

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

	AttachHook(0x006DBC40, 10, MapChangeHandler);// intel xed_ild_decode() to get size instead of fixed?

	while (true) {
		
	}

	printf("Exiting.\n");
}
