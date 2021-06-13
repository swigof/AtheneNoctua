#include <windows.h>

//update these are off now prolly
//put in header file
DWORD dwListingInfoBreakpoint = 0x6B95AAAB; // 0x60EBDD; // listing count at [ESI] and price at [ESI+8]
DWORD dwItemInfoBreakpoint = 0x4B5A78; // item ID in EAX and on top of stack
DWORD dwShopOpenStartEndBreakpoint = 0x5C04BF; // executed once at shop open start then once at shop open end
//scan for these instead of static?
// hash check against client exe built for, if different, doesn't work
//.def file?