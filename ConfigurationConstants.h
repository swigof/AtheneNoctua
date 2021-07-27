#pragma once
#include "AssemblyInjection/AssemblyHook.h"

// memory hookpoints with start address, size, end address and bytes
constexpr hookpoint OFF_MAP = {};
constexpr hookpoint ON_MAP = {};
constexpr hookpoint CHANNEL_CHANGE = {};
constexpr hookpoint CHARACTER_CHANGE = {};
constexpr hookpoint MAP_CHANGE = {};
constexpr hookpoint AREA_NAME_CHANGE = {};
constexpr hookpoint MAP_NAME_CHANGE = {};

// server and endpoint to send data to
constexpr char* SERVER = "";
constexpr char* ENDPOINT = "";

// SHA1 hash of client .exe in lowercase
constexpr char* GAME_VERSION_HASH = "";

// interval to send client data updates in milliseconds
constexpr int UPDATE_INTERVAL = 60000;

// time after which the server removes inactive entries in seconds
// adjusted to 80% of the value for client side purging
constexpr int SERVER_PURGE_INTERVAL = 3600;
