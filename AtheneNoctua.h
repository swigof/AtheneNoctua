#pragma once
#include <string>
#include "AssemblyHook.h"

union charactername {
	DWORD dwords[3];
	BYTE bytes[12];
};

struct memorystring {
	char* address;
	DWORD length;
	std::string str;
};

struct playerdata {
	charactername characterName;
	DWORD mapID = -1;
	DWORD channel = -1;
	memorystring mapName;
	memorystring areaName;
	bool isOnline = false;
	bool inCashShop = false;
	struct {
		bool characterName = false;
		bool mapID = false;
		bool channel = false;
		bool mapName = false;
		bool areaName = false;
		bool isOnline = false;
		bool inCashShop = false;
	} changeFlags;
	struct {
		time_t characterName;
		time_t mapID;
		time_t channel;
		time_t mapName;
		time_t areaName;
		time_t isOnline;
		time_t inCashShop;
	} timestamps;
};


constexpr hookpoint CHANNEL_CHANGE = { 0x0065BC25, 5, 0x0065BC2A, "\x39\x45\xF0\x89\x03" };
constexpr hookpoint CHARACTER_CHANGE = { 0x004B7636, 6, 0x004B763C, "\x8B\x4D\xC8\x0F\xB6\xC0" };
constexpr hookpoint MAP_CHANGE = { 0x006DBC40, 10, 0x006DBC4A, "\xC7\x87\x38\x06\x00\x00\x01\x00\x00\x00" };
constexpr hookpoint AREA_NAME_CHANGE = { 0x006D94C6, 6, 0x006D94CC, "\x3B\xC3\xC6\x45\xFC\x0A" }; 
constexpr hookpoint MAP_NAME_CHANGE = { 0x006D9529, 6, 0x006D952F, "\x3B\xC3\xC6\x45\xFC\x0F" }; // last hit on any character login, channel change or map/area change

// in order on shop open
#define SHOP_OPEN_INSTRUCTION 0x5C04BF		// executed once on initial shop open and once when the shop has loaded
#define LISTING_INFO_INSTRUCTION 0x60EBDA	// listing count at [ESI] and price at [ESI+8] (change to find when each in register?)
#define ITEM_INFO_INSTRUCTION 0x4B5A78		// item ID in EAX (executes at other times as well not sure specifically what triggers it)

void StartTools();
