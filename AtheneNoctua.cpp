#include <map>
#include <string>
#include <stdio.h>
#include <windows.h>
#include <wininet.h>
#include "AtheneNoctua.h"
#include "AssemblyInjection/AssemblyHook.h"
#include "AssemblyInjection/AssemblyHandlers.h"
#include "ConfigurationConstants.h"

// from AssemblyHandlers
extern playerdata playerData;
extern bool handling;

// Sends a post request to the server endpoint, retrieving a dbID response.
int SendDBUpdate(std::string params_str) {
	HINTERNET hSession = InternetOpen("Mozilla/5.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	HINTERNET hConnect = InternetConnect(hSession, SERVER, INTERNET_DEFAULT_HTTPS_PORT, "", "", INTERNET_SERVICE_HTTP, 0, 0);
	HINTERNET hHttpFile = HttpOpenRequest(hConnect, "POST", ENDPOINT, NULL, NULL, NULL, INTERNET_FLAG_SECURE, 0);

	std::string headers = "Content-Type: application/x-www-form-urlencoded";
	while (!HttpSendRequest(hHttpFile, headers.c_str(), headers.length(), (LPVOID)params_str.c_str(), params_str.length())) {
		printf("HttpSendRequest error : (%lu)\n", GetLastError());

		InternetErrorDlg(GetDesktopWindow(), hHttpFile, ERROR_INTERNET_CLIENT_AUTH_CERT_NEEDED, 
			FLAGS_ERROR_UI_FILTER_FOR_ERRORS | FLAGS_ERROR_UI_FLAGS_GENERATE_DATA | FLAGS_ERROR_UI_FLAGS_CHANGE_OPTIONS, NULL);
	}

	DWORD dwFileSize;
	dwFileSize = BUFSIZ;

	char* buffer;
	buffer = new char[dwFileSize + 1];

	while (true) {
		DWORD dwBytesRead;
		BOOL bRead;

		bRead = InternetReadFile(hHttpFile,buffer,dwFileSize + 1,&dwBytesRead);
		if (dwBytesRead == 0) 
			break;
		if (!bRead)
			printf("InternetReadFile error : <%lu>\n", GetLastError());
		else {
			buffer[dwBytesRead] = 0;
			printf("Retrieved %lu data bytes: %s\n", dwBytesRead, buffer);
		}
	}

	InternetCloseHandle(hHttpFile);
	InternetCloseHandle(hConnect);
	InternetCloseHandle(hSession);
	return atoi(buffer);
}

// Builds the parameters string for a post request.
std::string buildParamsString(std::map<std::string, std::string> params) {
	
	std::string params_str = "";

	if (!params.empty()) {
		std::map<std::string, std::string>::iterator it = params.begin();
		params_str.append(it->first);
		params_str.append("=");
		params_str.append(it->second);
		it++;

		while (it != params.end())
		{
			params_str.append("&");
			params_str.append(it->first);
			params_str.append("=");
			params_str.append(it->second);
			it++;
		}
	}

	return params_str;
}

// Main function of the application.
void StartTools() {
	// setup print outs
#ifdef _DEBUG 
	if (AllocConsole()) {
		freopen("CONOUT$", "w", stdout);
		SetConsoleTitle("Console");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	}
#endif

	printf("DLL loaded\n");

	// create and attach assembly hooks
	AssemblyHook mapChangeHook = AssemblyHook(MapChangeHandler, MAP_CHANGE);
	mapChangeHook.Attach();
	AssemblyHook channelChangeHook = AssemblyHook(ChannelChangeHandler, CHANNEL_CHANGE);
	channelChangeHook.Attach();
	AssemblyHook characterChangeHook = AssemblyHook(CharacterChangeHandler, CHARACTER_CHANGE);
	characterChangeHook.Attach();
	AssemblyHook mapNameChangeHook = AssemblyHook(MapNameChangeHandler, MAP_NAME_CHANGE);
	mapNameChangeHook.Attach();
	AssemblyHook areaNameChangeHook = AssemblyHook(AreaNameChangeHandler, AREA_NAME_CHANGE);
	areaNameChangeHook.Attach();
	AssemblyHook onMapHook = AssemblyHook(OnMapHandler, ON_MAP);
	onMapHook.Attach();
	AssemblyHook offMapHook = AssemblyHook(OffMapHandler, OFF_MAP);
	offMapHook.Attach();

	printf("Hooks attached\n");

	DWORD dbID = 0;

	while (true) {
		Sleep(UPDATE_INTERVAL);

		while (handling) {
			printf("Waiting for handler\n");
			Sleep(500);
		}

		printf("Setting request params\n");

		std::map<std::string, std::string> params;

		if (playerData.changeFlags.areaName) {
			params.emplace("areaName", playerData.areaName);
			params.emplace("lastChange", std::to_string(playerData.lastChangeTime));
			playerData.changeFlags.areaName = false;
		}
		if (playerData.changeFlags.channel) {
			params.emplace("channel", std::to_string(playerData.channel + 1));
			params.emplace("lastChange", std::to_string(playerData.lastChangeTime));
			playerData.changeFlags.channel = false;
		}
		if (playerData.changeFlags.characterName) {
			params.emplace("characterName", std::string((char*)playerData.characterName.bytes, 12));
			params.emplace("lastChange", std::to_string(playerData.lastChangeTime));
			playerData.changeFlags.characterName = false;
		}
		if (playerData.changeFlags.mapID) {
			params.emplace("mapID", std::to_string(playerData.mapID));
			params.emplace("lastChange", std::to_string(playerData.lastChangeTime));
			playerData.changeFlags.mapID = false;
		}
		if (playerData.changeFlags.mapName) {
			params.emplace("mapName", playerData.mapName);
			params.emplace("lastChange", std::to_string(playerData.lastChangeTime));
			playerData.changeFlags.mapName = false;
		}
		if (playerData.changeFlags.onMap) {
			params.emplace("onMap", std::to_string(playerData.onMap));
			params.emplace("lastChange", std::to_string(playerData.lastChangeTime));
			playerData.changeFlags.onMap = false;
		}
		if (dbID) {
			params.emplace("dbID", std::to_string(dbID));
		}
		
		std::string paramsStr = buildParamsString(params);
		printf("Sending request %s\n", paramsStr.c_str());
		dbID = SendDBUpdate(paramsStr);
		while (!dbID) {
			Sleep(5000);
			printf("Resending request %s\n", paramsStr.c_str());
			dbID = SendDBUpdate(paramsStr);
		}
	}

	printf("Exiting\n");
}
