# Athene Noctua
This is a dll based mod for oldschool Maplestory clients which is used to scrape character map location data.

It is meant to be used in conjuction with a server that receives this data then serves it as an index of teleport rock locations.

This mod was built for MapleLegends and while the versions/sources of Maplestory that this can work with are unknown, it could hypothetically work with others.

# Configuring
The ConfigurationConstants.h file contains all constant that must be defined for a given game client and version.

To ensure the mod doesn't run on any client it wasn't built for, a SHA1 hash must be provided.

The hookpoints are the start address, size, end address and op code bytes for client code which executes during specific game tasks.

These hookpoints are the locations where code is replaced to extract relevant game information. 
They have a minimum required size of 6 and must cover operations fully.

* OFF_MAP is on entering the cash shop or exiting to the login screen
* ON_MAP is on exiting the cash shop or loging in to a character
* CHANNEL_CHANGE is on channel change
* CHARACTER_CHANGE is on character login
* MAP_CHANGE is on map ID change
* AREA_NAME_CHANGE is on area name change (ex: maple island, victoria island, dungeon, etc.)
* MAP_NAME_CHANGE is on map name change

These hookpoints won't be hit exclusively when there is a change however so actual changes are manually checked.
For example, on any map change the CHANNEL_CHANGE is actually hit, though this may not be the case for all clients or hookpoint possibilities.

An example ConfigurationConstants.h can be seen [here](https://drive.google.com/file/d/1KEMAgNg8IbhrFRHXzklYVnxt2jsiqcUs/view).

# Compiling
This project's dependencies are exclusively windows libraries and the packaged d3d8 library.

Compile for 32bit with VS2019.

# Installing
Simply drop the created dll file into your client folder next to the exe.

# Server
The server acts as the receiver for character location data and should have a frontend for users to view this location data. Each database entry acts as a seperate client session. Due to this, it is possible that multiple entries with the same character name can exist simultaneously. Expected behavior for the server is as follows.
* It should add new database entries when an ID is not specified in a request. 
* It should edit existing ones when a valid ID is provided. 
* It should return the ID of the updated or new entry on successful processing.
* It should return 0 on unsuccessful processing.
* It should automatically set an attribute to hide entries from the frontend after a few minutes without updates.
* It should automatically remove entries after some number of hours without updates.

An example server using Google Sheets and Apps Script can be found [here](https://docs.google.com/spreadsheets/d/1j0O5MKdWuDezclsS6xD2Soje6PHYGriu9NWVrOV_dFs/edit#gid=584115897).

To get the server script, make a copy of the sheet then navigate to Tools->Script editor.

# LICENSE 
Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
