#include <stdio.h>
#include <windows.h>
#include <wininet.h>
#include <time.h>
#include "AtheneNoctua.h"
#include "AssemblyInjection/AssemblyHook.h"
#include "AssemblyInjection/AssemblyHandlers.h"
#include "ConfigurationConstants.h"

// from AssemblyHandlers
extern playerdata playerData;
extern bool handling;

// Main function of the thread.
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

	if (!CheckSHA1()) {
		printf("SHA1 bad\nThread exiting\n");
		return;
	}
	printf("SHA1 good\n");

	// create and attach assembly hooks
	AssemblyHook mapChangeHook = AssemblyHook(MapChangeHandler, MAP_CHANGE);
	AssemblyHook channelChangeHook = AssemblyHook(ChannelChangeHandler, CHANNEL_CHANGE);
	AssemblyHook characterChangeHook = AssemblyHook(CharacterChangeHandler, CHARACTER_CHANGE);
	AssemblyHook mapNameChangeHook = AssemblyHook(MapNameChangeHandler, MAP_NAME_CHANGE);
	AssemblyHook areaNameChangeHook = AssemblyHook(AreaNameChangeHandler, AREA_NAME_CHANGE);
	AssemblyHook onMapHook = AssemblyHook(OnMapHandler, ON_MAP);
	AssemblyHook offMapHook = AssemblyHook(OffMapHandler, OFF_MAP);
	if (!mapChangeHook.Attach() || !channelChangeHook.Attach() || !characterChangeHook.Attach() ||
		!mapNameChangeHook.Attach() || !areaNameChangeHook.Attach() || !onMapHook.Attach() || !offMapHook.Attach()) {
		printf("Hook attaches failed\nThread exiting\n");
		return;
	}

	printf("Hooks attached\n");

	DWORD dbID = 0;
	time_t lastSuccessfulRequestTime = 0;

	while (true) {
		Sleep(UPDATE_INTERVAL);

		// reset dbID if the purge time has passed
		if ((time(NULL) - lastSuccessfulRequestTime) >= (SERVER_PURGE_INTERVAL * 0.8)) {
			dbID = 0;
			printf("Purge time passed, dbID reset\n");
		}

		// wait for game data to be read
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
		int attempts = 1;
		printf("Sending update request %s\n", paramsStr.c_str());
		dbID = SendDBUpdate(paramsStr);
		while (!dbID && attempts < 5) {
			Sleep(5000);
			printf("Resending update request %s\n", paramsStr.c_str());
			dbID = SendDBUpdate(paramsStr);
			attempts++;
		}
		if (dbID) {
			time(&lastSuccessfulRequestTime);
			printf("Update request successful, dbID = %u\n", dbID);
		}
		else {
			printf("Update request unsuccessful\n");
			if(params.find("dbID") != params.end())
				dbID = stoi(params["dbID"]);
		}
	}

	printf("Thread exiting\n");
}

// Check hash of hooked game against GAME_VERSION_HASH
bool CheckSHA1() {
	TCHAR fileName[MAX_PATH];
	GetModuleFileName(NULL, fileName, MAX_PATH);
	HANDLE hFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		printf("Error opening %s: %d\n", fileName, GetLastError());

	HCRYPTPROV hCryptProv;
	if (!CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
		printf("CryptAcquireContext failed: %d\n", GetLastError());
	HCRYPTHASH hCryptHash;
	if (!CryptCreateHash(hCryptProv, CALG_SHA1, 0, 0, &hCryptHash))
		printf("CryptCreateHash failed: %d\n", GetLastError());

	bool result = false;
	BYTE rgbFile[1024];
	DWORD cbRead = 0;
	while (result = ReadFile(hFile, rgbFile, 1024, &cbRead, NULL)) {
		if (0 == cbRead)
			break;
		if (!CryptHashData(hCryptHash, rgbFile, cbRead, 0))
			printf("CryptHashData failed: %d\n", GetLastError());
	}
	if (!result)
		printf("ReadFile failed: %d\n", GetLastError());

	DWORD cbHash = 20;
	BYTE rgbHash[20];
	CHAR hash[40];
	CHAR rgbDigits[] = "0123456789abcdef";
	if (!CryptGetHashParam(hCryptHash, HP_HASHVAL, rgbHash, &cbHash, 0))
		printf("CryptGetHashParam failed: %d\n", GetLastError());
	for (DWORD i = 0; i < cbHash; i++) {
		hash[i*2] = rgbDigits[rgbHash[i] >> 4];
		hash[i*2+1] = rgbDigits[rgbHash[i] & 0xf];
	}

	CryptDestroyHash(hCryptHash);
	CryptReleaseContext(hCryptProv, 0);
	CloseHandle(hFile);

	if (!memcmp(GAME_VERSION_HASH, hash, cbHash * 2))
		return true;
	else
		return false;
}

// Sends a post request to the server endpoint, retrieving a dbID response.
int SendDBUpdate(std::string params_str) {
	HINTERNET hSession = InternetOpen("Mozilla/5.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	HINTERNET hConnect = InternetConnect(hSession, SERVER, INTERNET_DEFAULT_HTTPS_PORT, "", "", INTERNET_SERVICE_HTTP, 0, 0);
	HINTERNET hHttpFile = HttpOpenRequest(hConnect, "POST", ENDPOINT, NULL, NULL, NULL, INTERNET_FLAG_SECURE, 0);

	int attempts = 0;
	std::string headers = "Content-Type: application/x-www-form-urlencoded";
	while (!HttpSendRequest(hHttpFile, headers.c_str(), headers.length(), (LPVOID)params_str.c_str(), params_str.length()) && attempts < 5) {
		printf("HttpSendRequest error : (%lu)\n", GetLastError());
		attempts++;
	}
	if (attempts >= 5) {
		printf("Failed to send request\n");
		return 0;
	}

	DWORD fileSize;
	fileSize = BUFSIZ;

	char* buffer;
	buffer = new char[fileSize + 1];
	DWORD bytesRead = 0;

	while (true) {
		BOOL read;

		read = InternetReadFile(hHttpFile, buffer, fileSize + 1, &bytesRead);
		if (bytesRead == 0)
			break;
		if (!read) {
			printf("InternetReadFile error : <%lu>\n", GetLastError());
			return 0;
		}
		else
			buffer[bytesRead] = 0;
	}

	InternetCloseHandle(hHttpFile);
	InternetCloseHandle(hConnect);
	InternetCloseHandle(hSession);

	for (int i = 0; i < bytesRead; i++) {
		if (!isdigit(buffer[i])) {
			printf("Non-numeric response retrieved\n");
			return 0;
		}
	}

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
