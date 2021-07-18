#pragma once
#include "AssemblyHook.h"

constexpr hookpoint OFF_MAP = { 0x00739183, 5, 0x00739188, "\xF6\x44\x24\x08\x01" };
constexpr hookpoint ON_MAP = { 0x0073A073, 7, 0x0073A07A, "\x83\x4D\xFC\xFF\x8D\x45\xD4" };
constexpr hookpoint CHANNEL_CHANGE = { 0x0065BC25, 5, 0x0065BC2A, "\x39\x45\xF0\x89\x03" };
constexpr hookpoint CHARACTER_CHANGE = { 0x004B7636, 6, 0x004B763C, "\x8B\x4D\xC8\x0F\xB6\xC0" };
constexpr hookpoint MAP_CHANGE = { 0x006DBC40, 10, 0x006DBC4A, "\xC7\x87\x38\x06\x00\x00\x01\x00\x00\x00" };
constexpr hookpoint AREA_NAME_CHANGE = { 0x006D94C6, 6, 0x006D94CC, "\x3B\xC3\xC6\x45\xFC\x0A" };
constexpr hookpoint MAP_NAME_CHANGE = { 0x006D9529, 6, 0x006D952F, "\x3B\xC3\xC6\x45\xFC\x0F" };

constexpr char* SERVER = "script.google.com";
constexpr char* ENDPOINT = "";

constexpr char* GAME_VERSION_HASH = "";

constexpr int UPDATE_INTERVAL = 60000;