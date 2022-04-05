/**
HookExtension Plugin by Cannon
*/

// includes 

#include <windows.h>
#include <stdio.h>
#include <string>
#include <time.h>
#include <math.h>
#include <list>
#include <map>
#include <algorithm>
#include <FLHook.h>
#include <plugin.h>
#include <math.h>

struct FLHOOK_PLAYER_DATA
{
	string charfilename;
	map<string, string> lines;
};

map<uint, EVENT_PLUGIN_POB_TRANSFER> eventplugindata;

map<uint, string> miningobjdata;

map<uint, FLHOOK_PLAYER_DATA> clients;

/// A return code to indicate to FLHook if we want the hook processing to continue.
PLUGIN_RETURNCODE returncode;

string GetAccountDir(uint client)
{
	static _GetFLName GetFLName = (_GetFLName)((char*)hModServer + 0x66370);
	char dirname[1024];
	GetFLName(dirname, Players[client].Account->wszAccID);
	return dirname;
}

string GetCharfilename(const wstring &charname)
{
	static _GetFLName GetFLName = (_GetFLName)((char*)hModServer + 0x66370);
	char filename[1024];
	GetFLName(filename, charname.c_str());
	return filename;
}

static PlayerData *CurrPlayer;
int __stdcall HkCb_UpdateFile(char *filename, wchar_t *savetime, int b)
{
	// Call the original save charfile function
	int retv;
	__asm
	{
		pushad
		mov ecx, [CurrPlayer]
		push b
		push savetime
		push filename
		mov eax, 0x6d4ccd0
		call eax
		mov retv, eax
		popad
	}

	// Readd the flhook section.
	if (retv)
	{
		uint client = CurrPlayer->iOnlineID;

		string path = scAcctPath + GetAccountDir(client) + "\\" + filename;
		FILE *file = fopen(path.c_str(), "a");
		if (file)
		{
			fprintf(file, "[flhook]\n");
			for (map<string, string>::iterator i = clients[client].lines.begin();
				i != clients[client].lines.end(); ++i)
				fprintf(file, "%s = %s\n", i->first.c_str(), i->second.c_str());
			fclose(file);
		}
	}

	return retv;
}

__declspec(naked)void HkCb_UpdateFileNaked()
{
	__asm mov CurrPlayer, ecx
	__asm jmp HkCb_UpdateFile
}


/// Clear client info when a client connects.
void ClearClientInfo(uint client)
{
	returncode = DEFAULT_RETURNCODE;
	clients.erase(client);
}

/// Load the flhook section from the character file
void __stdcall CharacterSelect(struct CHARACTER_ID const &charid, unsigned int client)
{
	returncode = DEFAULT_RETURNCODE;

	string path = scAcctPath + GetAccountDir(client) + "\\" + charid.szCharFilename;

	//ConPrint(L"CharacterSelect=%s\n", stows(path).c_str());

	clients[client].charfilename = charid.szCharFilename;
	clients[client].lines.clear();

	// Read the flhook section so that we can rewrite after the save so that it isn't lost
	INI_Reader ini;
	if (ini.open(path.c_str(), false))
	{
		while (ini.read_header())
		{
			if (ini.is_header("flhook"))
			{
				wstring tag;
				while (ini.read_value())
				{
					clients[client].lines[ini.get_name_ptr()] = ini.get_value_string();
				}
			}
		}
		ini.close();
	}
}

void __stdcall DisConnect(unsigned int client, enum EFLConnection p2)
{
	returncode = DEFAULT_RETURNCODE;
	clients.erase(client);
}

/// Load the configuration
void LoadSettings()
{
	returncode = DEFAULT_RETURNCODE;

	// The path to the configuration file.
	char szCurDir[MAX_PATH];
	GetCurrentDirectory(sizeof(szCurDir), szCurDir);
	string scPluginCfgFile = string(szCurDir) + "\\flhook_plugins\\hookext.cfg";

	clients.clear();
	struct PlayerData *pPD = 0;
	while (pPD = Players.traverse_active(pPD))
	{
		uint client = HkGetClientIdFromPD(pPD);
		wstring charname = (const wchar_t*)Players.GetActiveCharacterName(client);
		string filename = GetCharfilename(charname) + ".fl";
		CHARACTER_ID charid;
		strcpy(charid.szCharFilename, filename.c_str());
		CharacterSelect(charid, client);
	}
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	static bool patched = false;
	srand((uint)time(0));

	// If we're being loaded from the command line while FLHook is running then
	// set_scCfgFile will not be empty so load the settings as FLHook only
	// calls load settings on FLHook startup and .rehash.
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		if (set_scCfgFile.length() > 0)
			LoadSettings();

		if (!patched)
		{
			patched = true;
			PatchCallAddr((char*)hModServer, 0x6c547, (char*)HkCb_UpdateFileNaked);
			PatchCallAddr((char*)hModServer, 0x6c9cd, (char*)HkCb_UpdateFileNaked);
		}
	}
	else if (fdwReason == DLL_PROCESS_DETACH)
	{
		if (patched)
		{
			patched = false;
			{
				BYTE patch[] = { 0xE8, 0x84, 0x07, 0x00, 0x00 };
				WriteProcMem((char*)hModServer + 0x6c547, patch, 5);
			}

			{
				BYTE patch[] = { 0xE8, 0xFE, 0x2, 0x00, 0x00 };
				WriteProcMem((char*)hModServer + 0x6c9cd, patch, 5);
			}
		}
	}
	return true;
}

namespace HookExt
{
	EXPORT void AddPOBEventData(uint client, string eventid, int count)
	{
		EVENT_PLUGIN_POB_TRANSFER ep;
		ep.eventid = eventid;
		ep.count = count;

		eventplugindata[client] = ep;
	}

	EXPORT void ClearPOBEventData()
	{
		eventplugindata.clear();
	}

	EXPORT map<uint, EVENT_PLUGIN_POB_TRANSFER> RequestPOBEventData()
	{
		//make a local copy of the map
		map<uint, EVENT_PLUGIN_POB_TRANSFER> sentmap = eventplugindata;

		//flush the current map
		eventplugindata.clear();

		//send the data to the event plugin for processing
		return sentmap;
	}

	//mining stuff

	EXPORT void AddMiningObj(uint spaceobj, string basename)
	{
		miningobjdata[spaceobj] = basename;
	}

	EXPORT void ClearMiningObjData()
	{
		miningobjdata.clear();
	}

	EXPORT map<uint, string> GetMiningEventObjs()
	{
		//make a local copy of the map
		map<uint, string> sentmap = miningobjdata;

		//send the data to the event plugin for processing
		return sentmap;
	}


	//end mining stuff

	EXPORT string IniGetS(uint client, const string &name)
	{
		if (clients.find(client) == clients.end())
			return "";

		if (!clients[client].charfilename.length())
			return "";

		if (clients[client].lines.find(name)
			== clients[client].lines.end())
			return "";

		return clients[client].lines[name];
	}

	EXPORT wstring IniGetWS(uint client, const string &name)
	{
		string svalue = HookExt::IniGetS(client, name);

		wstring value;
		long lHiByte;
		long lLoByte;
		while (sscanf(svalue.c_str(), "%02X%02X", &lHiByte, &lLoByte) == 2)
		{
			svalue = svalue.substr(4);
			wchar_t wChar = (wchar_t)((lHiByte << 8) | lLoByte);
			value.append(1, wChar);
		}
		return value;
	}

	EXPORT uint IniGetI(uint client, const string &name)
	{
		string svalue = HookExt::IniGetS(client, name);
		return strtoul(svalue.c_str(), 0, 10);
	}

	EXPORT bool IniGetB(uint client, const string &name)
	{
		string svalue = HookExt::IniGetS(client, name);
		if (svalue == "yes")
			return true;
		return false;
	}

	EXPORT float IniGetF(uint client, const string &name)
	{
		string svalue = HookExt::IniGetS(client, name);
		return (float)atof(svalue.c_str());
	}

	EXPORT void IniSetS(uint client, const string &name, const string &value)
	{
		clients[client].lines[name] = value;
	}

	EXPORT void IniSetWS(uint client, const string &name, const wstring &value)
	{
		string svalue = "";
		for (uint i = 0; (i < value.length()); i++)
		{
			char cHiByte = value[i] >> 8;
			char cLoByte = value[i] & 0xFF;
			char szBuf[8];
			sprintf(szBuf, "%02X%02X", ((uint)cHiByte) & 0xFF, ((uint)cLoByte) & 0xFF);
			svalue += szBuf;
		}

		HookExt::IniSetS(client, name, svalue);
	}

	EXPORT void IniSetI(uint client, const string &name, uint value)
	{
		char svalue[100];
		sprintf(svalue, "%u", value);
		HookExt::IniSetS(client, name, svalue);
	}

	EXPORT void IniSetB(uint client, const string &name, bool value)
	{
		string svalue = value ? "yes" : "no";
		HookExt::IniSetS(client, name, svalue);
	}

	EXPORT void IniSetF(uint client, const string &name, float value)
	{
		char svalue[100];
		sprintf(svalue, "%0.02f", value);
		HookExt::IniSetS(client, name, svalue);
	}

	EXPORT void IniSetS(const wstring &charname, const string &name, const string &value)
	{
		// If the player is online then update the in memory cache.
		string charfilename = GetCharfilename(charname) + ".fl";
		for (map<uint, FLHOOK_PLAYER_DATA>::iterator i = clients.begin(); i != clients.end(); ++i)
		{
			if (i->second.charfilename == charfilename)
			{
				HookExt::IniSetS(i->first, name, value);
				return;
			}
		}

		// Otherwise write directly to the character file if it exists.
		CAccount *acc = HkGetAccountByCharname(charname);
		if (acc)
		{
			string charpath = scAcctPath + GetCharfilename(acc->wszAccID) + "\\" + charfilename;
			WritePrivateProfileString("flhook", name.c_str(), value.c_str(), charpath.c_str());
		}
	}

	EXPORT void IniSetWS(const wstring &charname, const string &name, const wstring &value)
	{
		string svalue = "";
		for (uint i = 0; (i < value.length()); i++)
		{
			char cHiByte = value[i] >> 8;
			char cLoByte = value[i] & 0xFF;
			char szBuf[8];
			sprintf(szBuf, "%02X%02X", ((uint)cHiByte) & 0xFF, ((uint)cLoByte) & 0xFF);
			svalue += szBuf;
		}

		HookExt::IniSetS(charname, name, svalue);
	}

	EXPORT void IniSetI(const wstring &charname, const string &name, uint value)
	{
		char svalue[100];
		sprintf(svalue, "%u", value);
		HookExt::IniSetS(charname, name, svalue);
	}

	EXPORT void IniSetB(const wstring &charname, const string &name, bool value)
	{
		string svalue = value ? "yes" : "no";
		HookExt::IniSetS(charname, name, svalue);
	}

	EXPORT void IniSetF(const wstring &charname, const string &name, float value)
	{
		char svalue[100];
		sprintf(svalue, "%0.02f", value);
		HookExt::IniSetS(charname, name, svalue);
	}
}

/** Functions to hook */
EXPORT PLUGIN_INFO* Get_PluginInfo()
{
	PLUGIN_INFO* p_PI = new PLUGIN_INFO();
	p_PI->sName = "HookExt Plugin by Cannon";
	p_PI->sShortName = "hookext";
	p_PI->bMayPause = true;
	p_PI->bMayUnload = true;
	p_PI->ePluginReturnCode = &returncode;
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&LoadSettings, PLUGIN_LoadSettings, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&ClearClientInfo, PLUGIN_ClearClientInfo, 0));
	p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&CharacterSelect, PLUGIN_HkIServerImpl_CharacterSelect, 0));
	//p_PI->lstHooks.push_back(PLUGIN_HOOKINFO((FARPROC*)&DisConnect, PLUGIN_HkIServerImpl_DisConnect,0));

	return p_PI;
}