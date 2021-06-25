#include "AssemblyHook.h"
#include <stdio.h>

AssemblyHook::AssemblyHook(DWORD instructionAddress, DWORD** handlerAddress, DWORD jumpbackAddress, DWORD instructionSize, char* instructionBytes) {
	this->instructionAddress = instructionAddress;
	this->handlerAddress = handlerAddress;
	this->jumpbackAddress = jumpbackAddress;
	this->instructionSize = instructionSize;
	this->instructionBytes = instructionBytes;
	this->attached = false;
}

AssemblyHook::~AssemblyHook() {
	this->Detach();
}

// not working
void AssemblyHook::Attach() {
	if (!attached) {
		if (this->instructionSize < 6) {
			printf("Failed to hook at %0x08x: instruction size of %u too small\n", this->instructionAddress, this->instructionSize);
			return;
		}
		DWORD dwOldProtect;
		VirtualProtect((void*)this->instructionAddress, this->instructionSize, PAGE_READWRITE, &dwOldProtect);

		BYTE handlerJump[] = { 0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
		handlerJump[2] = ((BYTE*)&(this->handlerAddress))[0];
		handlerJump[3] = ((BYTE*)&(this->handlerAddress))[1];
		handlerJump[4] = ((BYTE*)&(this->handlerAddress))[2];
		handlerJump[5] = ((BYTE*)&(this->handlerAddress))[3];

		memcpy((void*)this->instructionAddress, handlerJump, this->instructionSize);

		VirtualProtect((void*)this->instructionAddress, this->instructionSize, dwOldProtect, &dwOldProtect);
		FlushInstructionCache(GetCurrentProcess(), (void*)this->instructionAddress, this->instructionSize);
	}
}

void AssemblyHook::Detach() {
	if (attached) {
		DWORD dwOldProtect;
		VirtualProtect((void*)this->instructionAddress, this->instructionSize, PAGE_READWRITE, &dwOldProtect);

		memcpy((void*)this->instructionAddress, this->instructionBytes, this->instructionSize);

		VirtualProtect((void*)this->instructionAddress, this->instructionSize, dwOldProtect, &dwOldProtect);
		FlushInstructionCache(GetCurrentProcess(), (void*)this->instructionAddress, this->instructionSize);
	}
}
