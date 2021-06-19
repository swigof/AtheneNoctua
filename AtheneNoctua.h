#pragma once

// hash check for client version?
// scan for these instead of static? take down instruction values to refind

// need shop names (and/or ids?) on open 
// need player map and channel
// list of shop names/ids on map enter to check which have dissapeared?
#define SHOP_OPEN_INSTRUCTION 0x5C04BF		// executed once on initial shop open and once when the shop has loaded
#define LISTING_INFO_INSTRUCTION 0x60EBDA	// listing count at [ESI] and price at [ESI+8]
#define ITEM_INFO_INSTRUCTION 0x4B5A78		// item ID in EAX and on top of stack

#define STATUS_WX86_SINGLE_STEP 0x4000001E
#define STATUS_WX86_BREAKPOINT 0x4000001F

void StartTools();
