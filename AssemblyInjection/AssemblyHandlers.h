#pragma once
#include <string>
#include <windows.h>

union charactername {
	DWORD dwords[3];
	BYTE bytes[12];
};

struct playerdata {
	charactername characterName;
	DWORD mapID = -1;
	DWORD channel = -1;
	std::string mapName;
	std::string areaName;
	bool onMap = false;
	time_t lastChangeTime = 0;
	struct {
		bool characterName = false;
		bool mapID = false;
		bool channel = false;
		bool mapName = false;
		bool areaName = false;
		bool onMap = false;
	} changeFlags;
};

struct registers {
	DWORD eax;
	DWORD ebx;
	DWORD ecx;
	DWORD edx;
	DWORD esi;
	DWORD edi;
	DWORD ebp;
	DWORD esp;
};

void ChannelChangeHandler();
void CharacterChangeHandler();
void MapChangeHandler();
void AreaNameChangeHandler();
void MapNameChangeHandler();
void OnMapHandler();
void OffMapHandler();
