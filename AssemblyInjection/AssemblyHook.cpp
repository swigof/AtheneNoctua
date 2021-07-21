#include "AssemblyHook.h"
#include <stdio.h>

AssemblyHook::AssemblyHook(void* handler, hookpoint hookPoint) {
	this->instructionAddress = hookPoint.address;
	this->handlerAddress = (DWORD)handler;
	this->jumpbackAddress = hookPoint.next;
	this->instructionSize = hookPoint.size;
	this->instructionBytes = hookPoint.bytes;
	this->attached = false;
	this->WriteInstructionToHandler();
}

AssemblyHook::~AssemblyHook() {
	this->Detach();
}

bool AssemblyHook::Attach() {
	if (!attached) {
		if (this->instructionSize < 5) {
			printf("Failed to attach at 0x%08x: instruction size of %u too small\n", this->instructionAddress, this->instructionSize);
			return false;
		}
		if (memcmp((void*)this->instructionAddress, this->instructionBytes, this->instructionSize)) {
			printf("Failed to attach at 0x%08x: instruction bytes do not match\n", this->instructionAddress);
			return false;
		}
		DWORD dwOldProtect;
		VirtualProtect((void*)this->instructionAddress, this->instructionSize, PAGE_READWRITE, &dwOldProtect);

		DWORD jumpOffset = handlerAddress - (instructionAddress + 5);
		BYTE handlerJump[] = { 0xE9, 0x00, 0x00, 0x00, 0x00, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
		handlerJump[1] = ((BYTE*)&jumpOffset)[0];
		handlerJump[2] = ((BYTE*)&jumpOffset)[1];
		handlerJump[3] = ((BYTE*)&jumpOffset)[2];
		handlerJump[4] = ((BYTE*)&jumpOffset)[3];

		memcpy((void*)this->instructionAddress, handlerJump, this->instructionSize);

		VirtualProtect((void*)this->instructionAddress, this->instructionSize, dwOldProtect, &dwOldProtect);
		FlushInstructionCache(GetCurrentProcess(), (void*)this->instructionAddress, this->instructionSize);
		this->attached = true;
		return true;
	}
	else {
		printf("Failed to attach at 0x%08x: already attached\n", this->instructionAddress);
		return false;
	}
}

void AssemblyHook::Detach() {
	if (attached) {
		DWORD dwOldProtect;
		VirtualProtect((void*)this->instructionAddress, this->instructionSize, PAGE_READWRITE, &dwOldProtect);

		memcpy((void*)this->instructionAddress, this->instructionBytes, this->instructionSize);

		VirtualProtect((void*)this->instructionAddress, this->instructionSize, dwOldProtect, &dwOldProtect);
		FlushInstructionCache(GetCurrentProcess(), (void*)this->instructionAddress, this->instructionSize);
		this->attached = false;
	}
}

void AssemblyHook::WriteInstructionToHandler() {
	DWORD dwOldProtect;
	VirtualProtect((void*)this->handlerAddress, this->instructionSize, PAGE_READWRITE, &dwOldProtect);

	memcpy((void*)this->handlerAddress, this->instructionBytes, this->instructionSize);

	VirtualProtect((void*)this->handlerAddress, this->instructionSize, dwOldProtect, &dwOldProtect);
	FlushInstructionCache(GetCurrentProcess(), (void*)this->handlerAddress, this->instructionSize);
}
