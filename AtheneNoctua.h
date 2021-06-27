#pragma once

#include <Windows.h>

struct hook_point {
	DWORD address;
	DWORD size;
	DWORD next;
	char* bytes;
};

constexpr hook_point MAP_CHANGE = { 0x006DBC40, 10, 0x006DBC4A, "\xC7\x87\x38\x06\x00\x00\x01\x00\x00\x00" };
constexpr hook_point CHANNEL_CHANGE = { 0x0065BC25, 5, 0x0065BC2A, "\x39\x45\xF0\x89\x03" };


// in order on shop open
#define SHOP_OPEN_INSTRUCTION 0x5C04BF		// executed once on initial shop open and once when the shop has loaded
#define LISTING_INFO_INSTRUCTION 0x60EBDA	// listing count at [ESI] and price at [ESI+8] (change to find when each in register?)
#define ITEM_INFO_INSTRUCTION 0x4B5A78		// item ID in EAX (executes at other times as well not sure specifically what triggers it)

void StartTools();
