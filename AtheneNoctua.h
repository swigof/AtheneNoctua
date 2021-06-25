#pragma once

// in order on any map/channel change
#define CHANNEL_CHANGE_INSTRUCTION 0x65BC28 // channel-1 in EAX
#define MAP_CHANGE_INSTRUCTION 0x6DBC3A     // map ID in EAX 

// in order on shop open
#define SHOP_OPEN_INSTRUCTION 0x5C04BF		// executed once on initial shop open and once when the shop has loaded
#define LISTING_INFO_INSTRUCTION 0x60EBDA	// listing count at [ESI] and price at [ESI+8] (change to find when each in register?)
#define ITEM_INFO_INSTRUCTION 0x4B5A78		// item ID in EAX (executes at other times as well not sure specifically what triggers it)

void StartTools();
