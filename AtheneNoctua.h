#pragma once

#include <Windows.h>

constexpr DWORD MAP_CHANGE_ADDRESS = 0x006DBC40;
constexpr DWORD MAP_CHANGE_INSTRUCTION_SIZE = 10;
constexpr DWORD MAP_CHANGE_JUMPBACK_ADDRESS = 0x006DBC4A;
constexpr char* MAP_CHANGE_INSTRUCTION_BYTES = "\xC7\x87\x38\x06\x00\x00\x01\x00\x00\x00"; 





// in order on any map/channel change
#define CHANNEL_CHANGE_INSTRUCTION 0x65BC28 // channel-1 in EAX

// in order on shop open
#define SHOP_OPEN_INSTRUCTION 0x5C04BF		// executed once on initial shop open and once when the shop has loaded
#define LISTING_INFO_INSTRUCTION 0x60EBDA	// listing count at [ESI] and price at [ESI+8] (change to find when each in register?)
#define ITEM_INFO_INSTRUCTION 0x4B5A78		// item ID in EAX (executes at other times as well not sure specifically what triggers it)

void StartTools();
