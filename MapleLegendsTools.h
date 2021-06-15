#pragma once

// .def?

// hash check for client version?
// update?
// scan for these instead of static?
#define LISTING_INFO_INSTRUCTION 0x60EBDD	// listing count at [ESI] and price at [ESI+8]
#define ITEM_INFO_INSTRUCTION 0x4B5A78		// item ID in EAX and on top of stack
#define SHOP_OPEN_INSTRUCTION 0x5C04BF		// executed once on initial shop open and once when the shop has loaded

#define STATUS_WX86_SINGLE_STEP 0x4000001E
#define STATUS_WX86_BREAKPOINT 0x4000001F

void StartTools();
