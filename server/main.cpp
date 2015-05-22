/***********************************
		   JK2MF Project

		 made for the lulz
***********************************/

#define MAIN

#include "main.h"
#include "api.h"
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>

/* global vars */
client clients[32];
time_t startTime;
GVER gVer; // binary instance
bool argIsHook;
gentity_s *gEntsp; // gentity_t in the mod
int sizeofEntsp;
uint cgameValids[30];
mg_context *ctx; // WebServer instance
char *whitelist;
char *blacklist;
char *reflist;
bool pseudoMod; // method 1 & 2
bool modProxy;
char jk2mfpath[2048]; // full path to jk2mf
char jk2mfpathrel[512];
bool blockCMDmod;
netadr_t lastAdr;
void *webMutex;
bool webReq;
const char *webPakStr;
int webReturn;
byte webIP[4];
mfDownloadInfo lastReq;
conless_t conlessIPs[500];
int conlessIPsClear;
byte blockedIPs[500][4];
int numblockedIPs;
bool jk2mfded;
void*(*gae)(int type);

// ==============
// Init Imports and Detoures
// ==============
void Init()
{
	SetGameBinaryVersion();

	if ( gVer == NULLBIN ) // This is not a supported jk2 version
		return;

	// Init function addresses
	Imp_VM_Call = (int(*)(vm_t *, int, ...))(GetAddr(gVer, VM_CALL));
	Imp_Com_Printf = (void(*)(const char *, ...))(GetAddr(gVer, COM_PRINTF));
	Imp_MSG_ReadStringLine = (char(*)(msg_t *))(GetAddr(gVer, MSG_READSTRINGLINE));
	Imp_MSG_ReadByte = (int(*)(msg_t *))(GetAddr(gVer, MSG_READBYTE));
	Imp_SV_GetUserinfo = (void(*)(int, char *, int))(GetAddr(gVer, SV_GETUSERINFO));
	Imp_Info_SetValueForKey = (void(*)(char *, const char *, const char*))(GetAddr(gVer, INFO_SETVALUEFORKEY));
	Imp_Info_ValueForKey = (char*(*)(const char*, const char*))(GetAddr(gVer, INFO_VALUEFORKEY));
	Imp_SV_SetUserinfo = (void(*)(int, const char *))(GetAddr(gVer, SV_SETUSERINFO));
	Imp_Cvar_Get = (cvar_t*(*)(const char *, const char *, int))(GetAddr(gVer, CVAR_GET));
	Imp_SV_GameSendServerCommand = (void(*)(int, const char*))(GetAddr(gVer, SV_GAMESENDSERVERCOMMAND));
	Imp_Cmd_Argc = (int(*)())(GetAddr(gVer, CMD_ARGC));
	Imp_Cmd_Argv = (char*(*)(int))(GetAddr(gVer, CMD_ARGV));
	Imp_SV_LocateGameData = (void(*)(gentity_s*, int, int, void*, int))(GetAddr(gVer, SV_LOCATEGAMEDATA));
	Imp_SV_UnlinkEntity = (void(*)(gentity_s*))(GetAddr(gVer, SV_UNLINKENTITY));
	Imp_VM_ArgPtr = (void*(*)(int))(GetAddr(gVer, VM_ARGPTR));
	Imp_SV_BeginDownload = (void(*)(client_t*))(GetAddr(gVer, SV_BEGINDOWNLOAD));
	Imp_SVC_RemoteCommand = (void(*)(netadr_t, msg_t*))(GetAddr(gVer, SVC_REMOTECOMMAND));
	Imp_SV_UpdateUserinfo = (void(*)(client_t*))(GetAddr(gVer, SV_UPDATEUSERINFO));
	Imp_SV_UserinfoChanged = (void(*)(client_t*))(GetAddr(gVer, SV_USERINFOCHANGED));
	Imp_VM_ExplicitArgPtr = (void*(*)(vm_t*, int))(GetAddr(gVer, VM_EXPLICITARGPTR));
	Imp_SV_GetConfigstring = (void(*)(int, char*,int))(GetAddr(gVer, SV_GETCONFIGSTRING));
	Imp_SV_ExecuteClientCommand = (void(*)(client_t*, const char*, qboolean))(GetAddr(gVer, SV_EXECUTECLIENTCOMMAND));
	Imp_MSG_ReadLong = (int(*)(msg_t*))(GetAddr(gVer, MSG_READLONG));
	Imp_MSG_ReadString = (char*(*)(msg_t*))(GetAddr(gVer, MSG_READSTRING));
	Imp_Cmd_TokenizeString = (void(*)(const char*))(GetAddr(gVer, CMD_TOKENIZESTRING));
	Imp_SV_GetUsercmd = (void(*)(int, usercmd_t*))(GetAddr(gVer, SV_GETUSERCMD));
	Imp_SV_ConnectionlessPacket = (void(*)(netadr_t, msg_t*))(GetAddr(gVer, SV_CONNECTIONLESSPACKET));
	Imp_NET_OutOfBandPrint = (void(*)(netsrc_t, netadr_t, const char*, ...))(GetAddr(gVer, NET_OUTOFBANDPRINT));
	Imp_SV_BotAllocateClient = (int(*)())(GetAddr(gVer, SV_BOTALLOCATECLIENT));
	Imp_FS_GetFileList = (int(*)(const char*, const char*, char*, int))(GetAddr(gVer, FS_GETFILELIST));
	Imp_Com_DPrintf = (void(*)(const char*, ...))(GetAddr(gVer, COM_DPRINTF));
	Imp_FS_ReferencedPakNames = (const char*(*)())(GetAddr(gVer, FS_REFPAKNAMES));
	Imp_FS_ReferencedPakChecksums = (const char*(*)())(GetAddr(gVer, FS_REFPAKCHKSUM));
	Imp_VM_Restart = (vm_t*(*)(vm_t*))(GetAddr(gVer, VM_RESTART));
	Imp_VM_Free = (void(*)(vm_t*))(GetAddr(gVer, VM_FREE));
	Imp_VM_Create = (vm_t*(*)(const char*, int(*)(int*), vmInterpret_t))(GetAddr(gVer, VM_CREATE));
	Imp_SV_GameSystemCalls = (int(*)(int*))(GetAddr(gVer, GAME_SYSCALLS));
	Imp_Cvar_Set2 = (cvar_t*(*)(const char*, const char*, qboolean))(GetAddr(gVer, CVAR_SET2));

	// Init variable addresses
	svtime = (int*)(GetAddr(gVer, SVS_TIME));
	lastRconTime = (unsigned int*)(GetAddr(gVer, LAST_RCONTIME));
	svclients = (client_t**)(GetAddr(gVer, SVS_CLIENTS));
	gvm = (vm_t**)(GetAddr(gVer, G_VM));
	fs_searchpaths = (searchpath_t**)(GetAddr(gVer, FS_SEARCHPATH));

	// Init Detours
	if (gVer != JK2MFDED)
	{
		Ori_VM_Call = (VM_Call_Def*)Detour((void*)Imp_VM_Call, (void*)VM_Call_Hook, Gethl(gVer, VM_CALL));
		Ori_MSG_ReadStringLine = (MSG_ReadStringLine_Def*)Detour((void*)Imp_MSG_ReadStringLine, (void*)MSG_ReadStringLine_Hook, Gethl(gVer, MSG_READSTRINGLINE));
		Ori_SV_GameSendServerCommand = (SV_GameSendServerCommand_Def*)Detour((void*)Imp_SV_GameSendServerCommand, (void*)SV_GameSendServerCommand_Hook, Gethl(gVer, SV_GAMESENDSERVERCOMMAND));
		Ori_SV_LocateGameData = (SV_LocateGameData_Def*)Detour((void*)Imp_SV_LocateGameData, (void*)SV_LocateGameData_Hook, Gethl(gVer, SV_LOCATEGAMEDATA));
		Ori_SV_BeginDownload = (SV_BeginDownload_Def*)Detour((void*)Imp_SV_BeginDownload, (void*)SV_BeginDownload_Hook, Gethl(gVer, SV_BEGINDOWNLOAD));
		Ori_SVC_RemoteCommand = (SVC_RemoteCommand_Def*)Detour((void*)Imp_SVC_RemoteCommand, (void*)SVC_RemoteCommand_Hook, Gethl(gVer, SVC_REMOTECOMMAND));
		Ori_SV_UpdateUserinfo = (SV_UpdateUserinfo_Def*)Detour((void*)Imp_SV_UpdateUserinfo, (void*)SV_UpdateUserinfo_Hook, Gethl(gVer, SV_UPDATEUSERINFO));
		Ori_VM_ExplicitArgPtr = (VM_ExplicitArgPtr_Def*)Detour((void*)Imp_VM_ExplicitArgPtr, (void*)VM_ExplicitArgPtr_Hook, Gethl(gVer, VM_EXPLICITARGPTR));
		Ori_SV_ExecuteClientCommand = (SV_ExecuteClientCommand_Def*)Detour((void*)Imp_SV_ExecuteClientCommand, (void*)SV_ExecuteClientCommand_Hook, Gethl(gVer, SV_EXECUTECLIENTCOMMAND));
		Ori_SV_GetUsercmd = (SV_GetUsercmd_Def*)Detour((void*)Imp_SV_GetUsercmd, (void*)SV_GetUsercmd_Hook, Gethl(gVer, SV_GETUSERCMD));
		Ori_SV_ConnectionlessPacket = (SV_ConnectionlessPacket_Def*)Detour((void*)Imp_SV_ConnectionlessPacket, (void*)SV_ConnectionlessPacket_Hook, Gethl(gVer, SV_CONNECTIONLESSPACKET));
		Ori_SV_BotAllocateClient = (SV_BotAllocateClient_Def*)Detour((void*)Imp_SV_BotAllocateClient, (void*)SV_BotAllocateClient_Hook, Gethl(gVer, SV_BOTALLOCATECLIENT));
		Ori_Com_DPrintf = (Com_DPrintf_Def*)Detour((void*)Imp_Com_DPrintf, (void*)Com_DPrintf_Hook, Gethl(gVer, COM_DPRINTF));
		Ori_FS_ReferencedPakNames = (FS_ReferencedPakNames_Def*)Detour((void*)Imp_FS_ReferencedPakNames, (void*)FS_ReferencedPakNames_Hook, Gethl(gVer, FS_REFPAKNAMES));
		Ori_FS_ReferencedPakChecksums = (FS_ReferencedPakChecksums_Def*)Detour((void*)Imp_FS_ReferencedPakChecksums, (void*)FS_ReferencedPakChecksums_Hook, Gethl(gVer, FS_REFPAKCHKSUM));
		Ori_VM_Create = (VM_Create_Def*)Detour((void*)Imp_VM_Create, (void*)VM_Create_Hook, Gethl(gVer, VM_CREATE));
		Ori_SV_GameSystemCalls = (SV_GameSystemCalls_Def*)Detour((void*)Imp_SV_GameSystemCalls, (void*)SV_GameSystemCalls_Hook, Gethl(gVer, GAME_SYSCALLS));
	}
	else
	{
		Ori_VM_Call = Imp_VM_Call;
		Ori_MSG_ReadStringLine = (MSG_ReadStringLine_Def*)Imp_MSG_ReadStringLine;
		Ori_SV_GameSendServerCommand = Imp_SV_GameSendServerCommand;
		Ori_SV_LocateGameData = (SV_LocateGameData_Def*)Imp_SV_LocateGameData;
		Ori_SV_BeginDownload = Imp_SV_BeginDownload;
		Ori_SVC_RemoteCommand = Imp_SVC_RemoteCommand;
		Ori_SV_UpdateUserinfo = Imp_SV_UpdateUserinfo;
		Ori_VM_ExplicitArgPtr = Imp_VM_ExplicitArgPtr;
		Ori_SV_ExecuteClientCommand = Imp_SV_ExecuteClientCommand;
		Ori_SV_GetUsercmd = Imp_SV_GetUsercmd;
		Ori_SV_ConnectionlessPacket = Imp_SV_ConnectionlessPacket;
		Ori_SV_BotAllocateClient = Imp_SV_BotAllocateClient;
		Ori_Com_DPrintf = Imp_Com_DPrintf;
		Ori_FS_ReferencedPakNames = Imp_FS_ReferencedPakNames;
		Ori_FS_ReferencedPakChecksums = Imp_FS_ReferencedPakChecksums;
		Ori_VM_Create = Imp_VM_Create;
		Ori_SV_GameSystemCalls = Imp_SV_GameSystemCalls;
	}

	// Increase the priority for the main thread so the webserver causes less laggs
	SetCurrentThreadPriorityHigh();

	// Checking if it has been loaded through the proxy
	if ( *gvm )
	{
		Init_Cvars(*gvm);
		pseudoMod = true;
		modProxy = true;

		// Set paths to the jk2mf directory (Mods Directory)
		if ( strlen(fs_game->string) )
		{
			Q_strncpyz(jk2mfpath, va("%s/%s/jk2mf", fs_basepath->string, fs_game->string), sizeof(jk2mfpath));
			Q_strncpyz(jk2mfpathrel, va("%s/jk2mf", fs_game->string), sizeof(jk2mfpathrel));
		}
		else
		{
			Q_strncpyz(jk2mfpath, va("%s/base/jk2mf", fs_basepath->string), sizeof(jk2mfpath));
			Q_strncpyz(jk2mfpathrel, "base/jk2mf", sizeof(jk2mfpathrel));
		}
	}

	// uptime calculation
	time(&startTime);

	webMutex = InitMutex();
	webReq = false;
	webReturn = -1;
}

EXPORT void JK2MFDED_Init(void*(*ga)(int))
{
	jk2mfded = true;
	gae = ga;

	Init();
}

EXPORT int JK2MFDED_GetApiVersion()
{
	return JK2MFDED_API_VERSION;
}

// We need an extra place for Cvars because in Init() the Cvar system is not initialized
void Init_Cvars(vm_t *vm)
{
	#ifdef WINAPIPRX
		if ( strcmp(vm->name, "jk2mpgame") )
			MessageBoxA(NULL, "JK2MF serverside detected that you are running it in normal gameing mode.\n"
							  "This may lead to heavy problems and your PC may jump out of the window.\n"
							  "Please put the winmm.dll out of your GameData folder and restart JK2.\n\n"
							  "If you have your server in the same directory which is also used by your normal game it's now time to seperate them. ;)", "JK2MF Error", MB_OK|MB_ICONERROR);
	#endif

	#ifdef WIN32
		Imp_Cvar_Get("JK2MF", va("%s win-x86", VERSION), CVAR_SERVERINFO|CVAR_ROM);
	#else
		Imp_Cvar_Get("JK2MF", va("%s linux-i386", VERSION), CVAR_SERVERINFO|CVAR_ROM);
	#endif

	// imports
	sv_maxclients = Imp_Cvar_Get("sv_maxclients", "8", 0);
	fs_homepath = Imp_Cvar_Get("fs_homepath", "", 0);
	fs_basepath = Imp_Cvar_Get("fs_basepath", "", 0);
	fs_game = Imp_Cvar_Get("fs_game", "", 0);
	com_developer = Imp_Cvar_Get("developer", "0", CVAR_TEMP);
	mapname = Imp_Cvar_Get("mapname", "", 0);
	vm_game = Imp_Cvar_Get("vm_game", "", 0);

	// fixes
	jk2mf_fixq3infoboom =		Imp_Cvar_Get("jk2mf_fixq3infoboom",    "1",    CVAR_ARCHIVE);
	jk2mf_fixforcecrash =		Imp_Cvar_Get("jk2mf_fixforcecrash",    "1",    CVAR_ARCHIVE);
	jk2mf_fixnamecrash =		Imp_Cvar_Get("jk2mf_fixnamecrash",	   "1",    CVAR_ARCHIVE);
	jk2mf_fixq3filluic =		Imp_Cvar_Get("jk2mf_fixq3filluic",     "1",    CVAR_ARCHIVE);
	jk2mf_fixq3fillmcc =		Imp_Cvar_Get("jk2mf_fixq3fillmcc",     "2",    CVAR_ARCHIVE);
	jk2mf_fixflooding =			Imp_Cvar_Get("jk2mf_fixflooding",	   "800",  CVAR_ARCHIVE);
	jk2mf_fixq3msgboom =		Imp_Cvar_Get("jk2mf_fixq3msgboom",     "1",    CVAR_ARCHIVE);
	jk2mf_fixturretcrash =		Imp_Cvar_Get("jk2mf_fixturretcrash",   "1",    CVAR_ARCHIVE);
	jk2mf_fixq3cbufexec =		Imp_Cvar_Get("jk2mf_fixq3cbufexec",    "1",    CVAR_ARCHIVE);
	jk2mf_fixq3dirtrav =		Imp_Cvar_Get("jk2mf_fixq3dirtrav",     "1",    CVAR_ARCHIVE);
	jk2mf_fixrconblocking =		Imp_Cvar_Get("jk2mf_fixrconblocking",  "1",    CVAR_ARCHIVE);
	jk2mf_fixwpglowing =		Imp_Cvar_Get("jk2mf_fixwpglowing",     "0",    CVAR_ARCHIVE);
	jk2mf_fixnamediff =			Imp_Cvar_Get("jk2mf_fixnamediff",      "1",    CVAR_ARCHIVE);
	jk2mf_fixrocketbug =		Imp_Cvar_Get("jk2mf_fixrocketbug",     "1",    CVAR_ARCHIVE);
	jk2mf_fixteamcrash =		Imp_Cvar_Get("jk2mf_fixteamcrash",     "1",    CVAR_ARCHIVE);
	jk2mf_fixsaberstealing =	Imp_Cvar_Get("jk2mf_fixsaberstealing", "1",    CVAR_ARCHIVE);
	jk2mf_fixudpflooding =      Imp_Cvar_Get("jk2mf_fixudpflooding",   "15",   CVAR_ARCHIVE);
	jk2mf_fixgalaking =         Imp_Cvar_Get("jk2mf_fixgalaking",      "1",    CVAR_ARCHIVE);
	jk2mf_fixbrokenmodels =     Imp_Cvar_Get("jk2mf_fixbrokenmodels",  "1",    CVAR_ARCHIVE);

	// anti cheat
	jk2mf_forceplugin =			Imp_Cvar_Get("jk2mf_forceplugin",	   "1",	   CVAR_ARCHIVE|CVAR_SERVERINFO|CVAR_LATCH);
	jk2mf_anticheat =			Imp_Cvar_Get("jk2mf_anticheat",        "1",    CVAR_ARCHIVE|CVAR_SERVERINFO|CVAR_LATCH);

	// http downloads
	jk2mf_httpdownloads =		Imp_Cvar_Get("jk2mf_httpdownloads",	   "0",    CVAR_ARCHIVE|CVAR_SERVERINFO|CVAR_LATCH);
	jk2mf_httpserverport =		Imp_Cvar_Get("jk2mf_httpserverport",   "0",    CVAR_ARCHIVE|CVAR_LATCH);

	// others
	jk2mf_vmgame =				Imp_Cvar_Get("jk2mf_vmgame",           "2",	   CVAR_ARCHIVE);
	jk2mf_apilevel =			Imp_Cvar_Get("jk2mf_apilevel",         va("%i", JK2MF_APILEVEL), 0);
}

// ==============
// Get the Binary version this instance is running with so we can get the correct addresses
// ==============
void SetGameBinaryVersion()
{
	if (jk2mfded) {
		gVer = JK2MFDED;
		return;
	}

    char buff[14];
#ifdef WIN32
	char *jkDedExe = (char*)0x046f5d0;	// 0x046f5d0 jk2ded.exe -> JK2MP: v1.04
	char *jkMPExe = (char*)0x04f66a0;	// 0x04f66a0 jk2mp.exe  -> JK2MP: v1.04
	SIZE_T len;

	ReadProcessMemory(GetCurrentProcess(), jkDedExe, buff, sizeof(buff), &len);
	buff[13] = 0;
	if (strstr(buff, "JK2MP: v1.04")) {
		gVer = JK2DEDEXE;
		return;
	}

	ReadProcessMemory(GetCurrentProcess(), jkMPExe, buff, sizeof(buff), &len);
	buff[13] = 0;
	if (strstr(buff, "JK2MP: v1.04")) {
		gVer = JK2MPEXE;
		return;
	}
#else
	char *linuxded = (char*)0x08103900;
	if (strstr(linuxded, "JK2MP: v1.04")) {
		gVer = JK2DEDLINUX;
		return;
	}
#endif

	gVer = NULLBIN;
}

EXPORT vm_t *VM_Create_Hook(const char *module, int(*systemCalls)(int *), vmInterpret_t interpret)
{
	// If this module is called modgame the interpreter should not be specific by vm_game
	// because it's set to 0 in any case
	if ( !strcmp(module, "modgame") )
	{
		return Ori_VM_Create(module, systemCalls, (vmInterpret_t)jk2mf_vmgame->integer);
	}

	return Ori_VM_Create(module, systemCalls, interpret);
}

EXPORT int VM_Call_Hook(vm_t *vm, int callnum, ...)
{
	int args[16], retn;
	va_list ap;

	if ( pseudoMod )
	{
		// Init the real mod
		memcpy((char*)GetAddr(gVer, QVM_SRVNAME), "modgame", 8);

		Imp_VM_Free(*gvm);
		Imp_VM_Create("modgame", (int(*)(int*))(GetAddr(gVer, GAME_SYSCALLS)), (vmInterpret_t)jk2mf_vmgame->integer);
		Ori_VM_Call(*gvm, GAME_INIT, *svtime, rand(), qfalse);

		// call GAME_INIT
		GameInit(*gvm, qfalse, NULL);

		pseudoMod = false;
		return 0;
	}

	va_start(ap, callnum);
	for (int i = 0; (unsigned int)i < sizeof (args) / sizeof (args[i]); i++)
		args[i] = va_arg(ap, int);
	va_end(ap);

	switch ( callnum )
	{
		case GAME_RUN_FRAME:
			LockMutex(webMutex);
			if (webReq && webReturn == -1)
				WebServerFileRequestMainThread();
			UnlockMutex(webMutex);

			if ( conlessIPsClear <= *svtime )
			{
				memset(conlessIPs, 0, sizeof(conlessIPs));
				conlessIPsClear = *svtime + 1000;
			}
			break;
		case GAME_CLIENT_THINK:
			ClientThink(args[0]);
			break;
		case GAME_INIT:
			GameInit(vm, args[2], &args[0]);
			break;
		case GAME_SHUTDOWN:
			WebServerEnd();
			UnloadLists();
			break;
		case GAME_CLIENT_CONNECT:
			retn = (int)CheckPerson(args[0], (qboolean)args[1], (qboolean)args[2]);
			if ( retn != 0 )
				return retn;
			break;
		case GAME_CLIENT_DISCONNECT:
			memset(&clients[args[0]], 0, sizeof(clients[0]));
			break;
		case GAME_CLIENT_BEGIN:
			ClientBegin(args[0]);
			break;
		case GAME_CLIENT_COMMAND:
			if ( ClientCommand(args[0]) )
				return 0;
			break;
		case GAME_CONSOLE_COMMAND:
			if ( RconCommand() )
				return qtrue;
			break;
	}

	retn = Ori_VM_Call(vm, callnum, args[0],  args[1],  args[2],  args[3],
									args[4],  args[5],  args[6],  args[7],
									args[8],  args[9],  args[10], args[11],
									args[12], args[13], args[14], args[15]);

	// Get Name after the mod filtered it. This is a basejk bug and fixed here.
	if ( callnum == GAME_CLIENT_USERINFO_CHANGED || callnum == GAME_CLIENT_CONNECT )
		SetModFilteredName(args[0]);

	return retn;
}

void GameInit(vm_t *vm, int restart, int *time)
{
	if ( !pseudoMod )
	{
		Init_Cvars(vm);
		ResetSVSTime((qboolean)restart, time);
	}

	if (!modProxy)
	{
		// Injected
		// Set paths to the jk2mf directory (GameData)
		Q_strncpyz(jk2mfpath, va("%s/jk2mf", fs_basepath->string), sizeof(jk2mfpath));
		Q_strncpyz(jk2mfpathrel, "jk2mf", sizeof(jk2mfpathrel));
	}

	Imp_Com_Printf("-------------- JK2MF --------------\n");
	Imp_Com_Printf("Author:  ouned (Twitch, Twilight)\n");
	Imp_Com_Printf("Website: http://jk2.ouned.de/jk2mf\n");
	#ifdef WIN32
		Imp_Com_Printf("Version: %s win-x86\n", VERSION);
	#else
		Imp_Com_Printf("Version: %s linux-i386\n", VERSION);
	#endif
	Imp_Com_Printf("-----------------------------------\n");

	GenerateModChecksums();
	WebServerStart();
	ReadLists();

	// enable CMD blocking
	blockCMDmod = true;

	// reset conlessIPsClear
	conlessIPsClear = 0;

	memset(blockedIPs, 0, sizeof(blockedIPs));
	numblockedIPs = 0;
}

// ==============
// MSG_ReadStringLine
//
// Used to fix q3infoboom
// ==============
EXPORT char *MSG_ReadStringLine_Hook(msg_t *msg)
{
	if ( jk2mf_fixq3infoboom->integer )
	{
		static char	string[320]; // Now 320 and not 1024 (jk2 crashes at ~850)
		int		l,c;

		l = 0;
		do {
			c = Imp_MSG_ReadByte(msg);
			if (c == -1 || c == 0 || c == '\n') {
				break;
			}
			if ( c == '%' ) {
				c = '.';
			}
			string[l] = c;
			l++;
		} while (l < (int)sizeof(string)-1);

		string[l] = 0;

		return string;
	}
	else
	{
		// If fix is turned off just call the original one
		return Ori_MSG_ReadStringLine(msg);
	}
}

// ==============
// SV_GameSendServerCommand | ClientCommand
//
// Used to fix the q3msgboom lagging and to disable vsay commands
// Also, some informational commands and the callvote fix
// ==============
EXPORT void SV_GameSendServerCommand_Hook(int clientNum, const char *text)
{
	if ( jk2mf_fixq3msgboom->integer && strlen(text) > 1022 ) // Everything above 1022 would lagg the clients
	{
		return;
	}
	else
	{
		Ori_SV_GameSendServerCommand(clientNum, text);
	}
}

// NOTE: returning true will NOT call ClientCommand() in the mod
bool ClientCommand(int clientNum)
{
	char cmd[MAX_TOKEN_CHARS];
	char arg1[MAX_TOKEN_CHARS];
	char arg2[MAX_TOKEN_CHARS];

	Q_strncpyz(cmd, Imp_Cmd_Argv(0), sizeof(cmd));
	Q_strncpyz(arg1, Imp_Cmd_Argv(1), sizeof(arg1));
	Q_strncpyz(arg2, Imp_Cmd_Argv(2), sizeof(arg2));

	// say exploit fix
	if ( jk2mf_fixq3cbufexec->integer )
	{
		if ( !Q_stricmp(cmd, "say") || !Q_stricmp(cmd, "say_team") || !Q_stricmp(cmd, "tell") )
		{
			char args[2048] = "\0";
			for ( int i = 1; i < Imp_Cmd_Argc(); i++ )
				Q_strcat(args, sizeof(args), Imp_Cmd_Argv(i));

			if ( strchr(args, '\n') || strchr(args, '\r') )
			{
				Imp_SV_GameSendServerCommand(clientNum, "print \"^7JK2MF^1: ^7Command blocked^1.^7\n\"");
				return true;
			}
		}
	}

	// disable useless vsay commands
	if ( jk2mf_fixq3msgboom->integer )
	{
		if ( !Q_stricmp(cmd, "vsay") || !Q_stricmp(cmd, "vsay_team") || !Q_stricmp(cmd, "vtell") || !Q_stricmp(cmd, "vosay") ||
			 !Q_stricmp(cmd, "vosay_team") || !Q_stricmp(cmd, "votell") || !Q_stricmp(cmd, "vtaunt") )
		{
			Imp_SV_GameSendServerCommand(clientNum, "print \"^7JK2MF^1: ^7Command blocked^1.^7\n\"");
			return true;
		}
	}

	// q3cbufexec fix
	if ( jk2mf_fixq3cbufexec->integer && ( !Q_stricmp(cmd, "callvote") || !Q_stricmp(cmd, "callteamvote") ) )
	{
		if ( strchr(arg1, ';') || strchr(arg2, ';') || strchr(arg1, '\n') || strchr(arg2, '\n') || strchr(arg1, '\r') || strchr(arg2, '\r') )
		{
			Imp_SV_GameSendServerCommand(clientNum, "print \"^7JK2MF^1: ^7Illegal votestring^1.^7\n\"");
			return true;
		}
	}

	// teamcrash fix
	if ( jk2mf_fixteamcrash->integer && !Q_stricmp(cmd, "team") && ( !Q_stricmp(arg1, "follow1") || !Q_stricmp(arg1, "follow2") ) )
	{
		Imp_SV_GameSendServerCommand(clientNum, "print \"^7JK2MF^1: ^7Command blocked^1.^7\n\"");
		return true;
	}

	// force clientside & anticheat
	if ( ( !Q_stricmp(cmd, "forcechanged") && Imp_Cmd_Argc() > 1 ) || !Q_stricmp(cmd, "team") || !Q_stricmp(cmd, "follow") )
	{
		if ( jk2mf_forceplugin->integer && clients[clientNum].clientState == NOPLUGIN && blockCMDmod )
		{
			// Do not send this notification on the first team-change cmd
			if ( clients[clientNum].sendPlgMsgOnCmd )
				SendBadClientNotification(clientNum, NOPLUGIN);

			clients[clientNum].sendPlgMsgOnCmd = true;
			return true;
		}
		if ( jk2mf_anticheat->integer && jk2mf_forceplugin->integer )
		{
			if ( clients[clientNum].clientState == CODEMODIFICATION )
			{
				SendBadClientNotification(clientNum, CODEMODIFICATION);
				return true;
			}
			else if ( clients[clientNum].clientState == INVALIDMOD )
			{
				SendBadClientNotification(clientNum, INVALIDMOD);
				return true;
			}
			else if ( clients[clientNum].clientState == BROKENCOMMUNICATION )
			{
				SendBadClientNotification(clientNum, BROKENCOMMUNICATION);
				return true;
			}
		}
	}

	// information cmd's
	if ( !Q_stricmp(cmd, "jk2mfinf") || !Q_stricmp(cmd, "jk2mfinfo") )
	{
		if ( !Q_stricmp(arg1, "players") )
		{
			PrintPlayersClient(clientNum);
		}
		else if ( !Q_stricmp(arg1, "about") )
		{
			Imp_SV_GameSendServerCommand(clientNum, "print \"JK2MF is a plugin for Jedi Knight II that implements an anticheat system, HTTP-Downloads\n"
															 "and a lot of basejk fixes in a mod independent way. You can use JK2MF together with\n"
															 "DS-Online, DCMod, Twimod and most other mods.\n\"");
		}
		else if ( !Q_stricmp(arg1, "stats") )
		{
			char printBuff[1000] = "";

			// uptime
			time_t now;
			time(&now);
			char pTime[100];
			PrintTime(pTime, sizeof(pTime), (uint)now - (uint)startTime);

			// anticheat
			char cheatStr[20];
			if ( jk2mf_anticheat->integer && jk2mf_forceplugin->integer )
				Q_strncpyz(cheatStr, "^2on", sizeof(cheatStr));
			else if ( jk2mf_anticheat->integer && !jk2mf_forceplugin->integer )
				Q_strncpyz(cheatStr, "^3optional", sizeof(cheatStr));
			else
				Q_strncpyz(cheatStr, "^1off", sizeof(cheatStr));

			// HTTP-Downloads
			char httpStr[60];
			if ( jk2mf_httpdownloads->integer )
			{
				if ( jk2mf_httpserverport->integer != 0 )
					Q_strncpyz(httpStr, va("^2on ^7( TCP-Port: %i )", jk2mf_httpserverport->integer), sizeof(httpStr));
				else
					Q_strncpyz(httpStr, "^2on ^7( external )", sizeof(httpStr));
			}
			else
				Q_strncpyz(httpStr, "^1Off", sizeof(httpStr));

			Q_strncpyz(printBuff, va("print \"^1[ ^7JK2MF Server Stats ^1]^7\n"
											 "Server Uptime^1: ^7%s\n"
											 "Anticheat^1: %s^7\n"
											 "HTTP-Downloads^1: %s^7"
											 "\n\"", pTime, cheatStr, httpStr), sizeof(printBuff));

			Imp_SV_GameSendServerCommand(clientNum, printBuff);
		}
		else
		{
			Imp_SV_GameSendServerCommand(clientNum, "print \"^1[ ^7JK2MF Information ^1]\n"
															"^7Topics^1: ^7about^1, ^7stats^1, ^7players\n\"");
		}

		return true;
	}

	if ( !Q_stricmp(cmd, "cheaters") || !Q_stricmp(cmd, "amcheaters") )
	{
		PrintPlayersClient(clientNum);
		return true;
	}

	return false;
}

void PrintPlayersClient(int clientNum)
{
	if ( !jk2mf_anticheat->integer )
	{
		Imp_SV_GameSendServerCommand(clientNum, "print \"^7JK2MF^1: ^7Anticheat is disabled on this server^1.^7\n\"");
		return;
	}

	char pBuff[8192] = "";
	Q_strcat(pBuff, sizeof(pBuff), "^1##  Name                               Status^7");
	int plCount = 1;
	for ( int i = 0; i < sv_maxclients->integer; i++ )
	{
		client_t *pl = (client_t*)((uint)212204 * (uint)i + (uint)*svclients);

		if ( ( pl->state == CS_ACTIVE || pl->state == CS_CONNECTED ) && clients[i].clientState != BOT )
		{
			if ( plCount < 10 )
				Q_strcat(pBuff, sizeof(pBuff), va("\n^7%i^1)  ^7", plCount));
			else
				Q_strcat(pBuff, sizeof(pBuff), va("\n^7%i^1) ^7", plCount));

			char namebuff[256], filtered[256];
			int filteredlen = 0, seeable = 0;
			char info[MAX_INFO_STRING];

			Imp_SV_GetConfigstring(CS_PLAYERS + i, info, sizeof(info));
			Q_strncpyz(namebuff, Imp_Info_ValueForKey(info, "n"), sizeof(namebuff));
			
			// filter all ^ chars without following number 0 - 8 because it fucks up the whole list in 1.02
			// also filter bad chars out which can not be displayed, mostly used by players in 1.02
			for (int j = 0; j < strlen(namebuff); j++) {
				if (seeable >= 32)
					break;

				if (namebuff[j] == '^') {
					if (j + 1 < strlen(namebuff)) {
						if (namebuff[j + 1] < '0' || namebuff[j + 1] > '7') {
							// not okey (^a)
							// fix the colorcode
							namebuff[j+1] = '0' + (namebuff[j + 1] % 8); // 8 colorcodes = %8 (9 == red) see ascii table
						}

						filtered[filteredlen++] = namebuff[j];
						filtered[filteredlen++] = namebuff[j+1];
						j++; // skip the fixed char
					}
				}
				else {
					if (!strchr(VALIDNAMECHARS, namebuff[j]))
						continue; // skip this char

					seeable++;
					filtered[filteredlen++] = namebuff[j];
				}
			}
			filtered[filteredlen] = 0;

			Q_strfit(filtered, sizeof(filtered), 35);
			Q_strcat(pBuff, sizeof(pBuff), filtered);

			if ( pl->state == CS_CONNECTED )
				Q_strcat(pBuff, sizeof(pBuff), "^3connecting");
			else if ( clients[i].clientState == GOODCLIENT )
				Q_strcat(pBuff, sizeof(pBuff), "^2cheatfree");
			else if ( clients[i].clientState == NOPLUGIN )
				Q_strcat(pBuff, sizeof(pBuff), "^3unknown");
			else if ( clients[i].clientState == CODEMODIFICATION )
				Q_strcat(pBuff, sizeof(pBuff), "^1code modification");
			else if ( clients[i].clientState == SCRIPTER )
				Q_strcat(pBuff, sizeof(pBuff), "^1scripter");
			else if ( clients[i].clientState == INVALIDMOD )
				Q_strcat(pBuff, sizeof(pBuff), "^1unallowed clientside");
			else if ( clients[i].clientState == BROKENCOMMUNICATION )
				Q_strcat(pBuff, sizeof(pBuff), "^1communication failure");

			plCount++;
		}
	}

	Q_strcat(pBuff, sizeof(pBuff), "\n");
	PrintHugeBuffer(clientNum, pBuff);
}

// Print a very long text to a client
void PrintHugeBuffer(int clientNum, char *text)
{
	char *currPos = text;
	text = currPos + strlen(text);
	char colorCode = -1;

	while ( currPos < text )
	{
		int plen = text - currPos;

		// Send a maximum of 1000 chars per message
		if ( plen > 1000 )
			plen = 1000;

		// Do not cut strings inside a colorcode
		if ( currPos[plen-1] == '^' && currPos[plen] >= '0' && currPos[plen] <= '7' )
			plen++;

		char pBuff[1003];
		memcpy(pBuff, currPos, plen);
		pBuff[plen] = 0;

		if ( colorCode == -1 )
			Imp_SV_GameSendServerCommand(clientNum, va("print \"%s\"", pBuff));
		else
			Imp_SV_GameSendServerCommand(clientNum, va("print \"^%c%s\"", colorCode, pBuff));

		currPos += plen;

		// Save current colorcode for the next message
		char *i = currPos;
		bool found = false;
		while ( i != text - 1 )
		{
			if ( i[0] == '^' && i[1] >= '0' && i[1] <= '6'  )
			{
				colorCode = i[1];
				found = true;
				break;
			}

			i--;
		}
		if ( found == false )
			colorCode = -1;
	}
}

// ==============
// RconCommands
//
// Additional rcon commands
// NOTE: returning true will NOT call ConsoleCommand() in the mod
// ==============
bool RconCommand()
{
	char cmd[MAX_TOKEN_CHARS];
	Q_strncpyz(cmd, Imp_Cmd_Argv(0), sizeof(cmd));

	if ( !Q_stricmp(cmd, "jk2mfstats") )
	{
		char pBuff[MAX_TOKEN_CHARS] = "";

		for ( int i = 0; i < sv_maxclients->integer; i++ )
		{
			client_t *pl = (client_t*)((uint)212204 * (uint)i + (uint)*svclients);

			if ( ( pl->state == CS_ACTIVE || pl->state == CS_CONNECTED ) && clients[i].clientState != BOT )
			{
				if ( i < 10 )
					Q_strcat(pBuff, sizeof(pBuff), va("%i)  ", i));
				else
					Q_strcat(pBuff, sizeof(pBuff), va("%i) ", i));

				Q_strcat(pBuff, sizeof(pBuff), va("%s ", Imp_Info_ValueForKey(pl->userinfo, "name")));

				if ( pl->state == CS_CONNECTED )
					Q_strcat(pBuff, sizeof(pBuff), "connecting");
				else if ( clients[i].clientState == GOODCLIENT )
					Q_strcat(pBuff, sizeof(pBuff), "cheatfree");
				else if ( clients[i].clientState == NOPLUGIN )
					Q_strcat(pBuff, sizeof(pBuff), "unknown");
				else if ( clients[i].clientState == CODEMODIFICATION )
					Q_strcat(pBuff, sizeof(pBuff), "codeModification");
				else if ( clients[i].clientState == SCRIPTER )
					Q_strcat(pBuff, sizeof(pBuff), "scripter");
				else if ( clients[i].clientState == INVALIDMOD )
					Q_strcat(pBuff, sizeof(pBuff), "unallowedClientside");
				else if ( clients[i].clientState == BROKENCOMMUNICATION )
					Q_strcat(pBuff, sizeof(pBuff), "communicationFailure");

				Q_strcat(pBuff, sizeof(pBuff), "\n");
			}
		}

		Imp_Com_Printf(pBuff);
		return true;
	}

	return false;
}

// ==============
// SV_UpdateUserinfo | SetUserInfo | VerifyUserinfo | SetModFilteredName
//
// Used to fix forcecrash, namecrash and floodprotection
// ==============
EXPORT void SV_UpdateUserinfo_Hook(client_t *cl)
{
	int clientNum = ((uint)cl - (uint)*svclients) / (uint)212204;

	// Floodprotection
	if ( jk2mf_fixflooding->integer )
	{
		if ( clients[clientNum].nextUInfTime > *svtime && !clients[clientNum].allowedUCh )
		{
			Q_strncpyz(clients[clientNum].nextUInfStr, Imp_Cmd_Argv(1), sizeof(clients[0].nextUInfStr));
			return;
		}
	}

	if ( clients[clientNum].allowedUCh )
		clients[clientNum].allowedUCh--;

	SetUserInfo(cl, Imp_Cmd_Argv(1));
}

void SetUserInfo(client_t *cl, char *userinfo)
{
	int clientNum = ((uint)cl - (uint)*svclients) / (uint)212204;

	// Backward compatibility: If the value of jk2mf_fixflooding is lower then 10 it's still in seconds
	if ( jk2mf_fixflooding->integer >= 10 )
		clients[clientNum].nextUInfTime = *svtime + jk2mf_fixflooding->integer;
	else
		clients[clientNum].nextUInfTime = *svtime + (jk2mf_fixflooding->integer * 1000);

	Q_strncpyz(cl->userinfo, userinfo, sizeof(cl->userinfo));
	VerifyUserinfo(cl);
	Imp_SV_UserinfoChanged(cl);
	Imp_VM_Call(*gvm, GAME_CLIENT_USERINFO_CHANGED, clientNum);
}

void VerifyUserinfo(client_t *cl)
{
	// Namecrash fix
	if ( jk2mf_fixnamecrash->integer )
	{
		char name[61], cleanedName[61] = "\0"; // 60 because some mods are increasing this
		Q_strncpyz(name, Imp_Info_ValueForKey(cl->userinfo, "name"), sizeof(name));
		int count = 0;

		for ( int i = 0; i < (int)strlen(name); i++ )
		{
			char ch = name[i];
			if ( strchr(VALIDNAMECHARS, ch) )
			{
				cleanedName[count] = ch;
				count++;
			}
		}
		Imp_Info_SetValueForKey(cl->userinfo, "name", cleanedName);
	}

	// Forcecrash fix
	if ( jk2mf_fixforcecrash->integer )
	{
		char forcePowers[30];
		Q_strncpyz(forcePowers, Imp_Info_ValueForKey(cl->userinfo, "forcepowers"), sizeof(forcePowers));

		int len = strlen(forcePowers);
		bool badForce = false;
		if ( len >= 22 && len <= 24 )
		{
			byte seps = 0;

			for ( int i = 0; i < len; i++ )
			{
				if ( forcePowers[i] != '-' && ( forcePowers[i] < '0' || forcePowers[i] > '9' ) )
				{
					badForce = true;
					break;
				}

				if ( forcePowers[i] == '-' && ( i < 1 || i > 5 ) )
				{
					badForce = true;
					break;
				}

				if ( i && forcePowers[i-1] == '-' && forcePowers[i] == '-' )
				{
					badForce = true;
					break;
				}

				if ( forcePowers[i] == '-' )
					seps++;
			}

			if ( seps != 2 )
				badForce = true;
		}
		else
			badForce = true;

		if ( badForce )
			Q_strncpyz(forcePowers, "7-1-030000000000003332", sizeof(forcePowers));

		Imp_Info_SetValueForKey(cl->userinfo, "forcepowers", forcePowers);
	}

	if (jk2mf_fixgalaking->integer)
	{
		char model[80];

		Q_strncpyz(model, Imp_Info_ValueForKey(cl->userinfo, "model"), sizeof(model));
		if (stristr(model, "galak_mech"))
			Imp_Info_SetValueForKey(cl->userinfo, "model", "galak/default");

		Q_strncpyz(model, Imp_Info_ValueForKey(cl->userinfo, "team_model"), sizeof(model));
		if (stristr(model, "galak_mech"))
			Imp_Info_SetValueForKey(cl->userinfo, "team_model", "galak/default");
	}

	if (jk2mf_fixbrokenmodels->integer)
	{
		char model[80];

		Q_strncpyz(model, Imp_Info_ValueForKey(cl->userinfo, "model"), sizeof(model));
		if (stristr(model, "kyle/fpls") || (stristr(model, "morgan") && (stricmp(model, "morgan/default_mp") && stricmp(model, "morgan/red") && stricmp(model, "morgan/blue"))))
			Imp_Info_SetValueForKey(cl->userinfo, "model", "galak/default");

		Q_strncpyz(model, Imp_Info_ValueForKey(cl->userinfo, "team_model"), sizeof(model));
		if (stristr(model, "kyle/fpls") || (stristr(model, "morgan") && (stricmp(model, "morgan/default_mp") && stricmp(model, "morgan/red") && stricmp(model, "morgan/blue"))))
			Imp_Info_SetValueForKey(cl->userinfo, "team_model", "galak/default");
	}
}

// Because jk2 is to dump to call trap_SetUserInfo and only calls trap_SetConfigString we are doing it for the mod.
void SetModFilteredName(int clientNum)
{
	if ( jk2mf_fixnamediff->integer )
	{
		char info[MAX_INFO_STRING];
		Imp_SV_GetConfigstring(CS_PLAYERS+clientNum, info, sizeof(info));

		client_t *cl = (client_t*)((uint)212204 * (uint)clientNum + (uint)*svclients);
		char *clName = (char*)((uint)cl + (uint)133188); // can't use cl->name because ravensoft modded this struct and the pointer is wrong

		Imp_Info_SetValueForKey(cl->userinfo, "name", Imp_Info_ValueForKey(info, "n"));
		Q_strncpyz(clName, Imp_Info_ValueForKey(info, "n"), 32); // lenght 32 see quake3 source or disassembly
	}
}

// ==============
// VM_ExplicitArgPtr | CheckPerson | RemoveIp | RemoveAllIps
//
// q3fill protection
// ==============

EXPORT void *VM_ExplicitArgPtr_Hook(vm_t *vm, int intValue)
{
	// return just the intValue if it's inside jk2mf
	if ( argIsHook )
	{
		argIsHook = false;
		return (void*)intValue;
	}
	else // else, get the data from the original gamecode
		return Ori_VM_ExplicitArgPtr(vm, intValue);
}

char *CheckPerson(int clientNum, qboolean firstTime, qboolean isBot)
{
	// Call VerifyUserinfo to make sure this is no crasher
	VerifyUserinfo((client_t*)((uint)212204 * (uint)clientNum + (uint)*svclients));

	char userinfo[MAX_INFO_STRING];
	Imp_SV_GetUserinfo(clientNum, userinfo, sizeof(userinfo));

	if ( !isBot && firstTime )
	{
		// Simple userinfo protection
		// this will only work for noobs
		if ( jk2mf_fixq3filluic->integer )
		{
			// If the name is too long some of this values may not be in the string, this is why && is used
			if ( !strlen(Imp_Info_ValueForKey(userinfo, "color2")) && !strlen(Imp_Info_ValueForKey (userinfo, "sex")) &&
				 !strlen(Imp_Info_ValueForKey (userinfo, "handicap")) )
			{
				argIsHook = true;
				return "Invalid Userinfo";
			}
		}

		// Limiting connecting clients per IP
		// more pro friendly
		char buff[30], ip[20];
		Q_strncpyz(buff, Imp_Info_ValueForKey(userinfo, "ip"), sizeof(buff));

		if ( strchr(buff, ':') )
			Q_strncpyz(ip, buff, strchr(buff, ':') - buff + 1);

		int connClients = 0;
		int maxConnClients = jk2mf_fixq3fillmcc->integer;
		if ( maxConnClients <= 0 ) // If cvars value is <= 0 this check is disabled
			maxConnClients = 32;

		for ( int i = 0; i < 32; i++ )
		{
			if ( !strcmp(clients[i].IP, ip) )
				connClients++;
		}

		// Local clients can bypass this
		if ( connClients >= maxConnClients && strlen(ip) && strcmp(ip, "127.0.0.1") && strcmp(ip, "localhost") )
		{
			argIsHook = true;
			return "Please wait..";
		}

		Q_strncpyz(clients[clientNum].IP, ip, sizeof(clients[0].IP));
		Q_strncpyz(clients[clientNum].IPSave, ip, sizeof(clients[0].IPSave));

		// reset client struct vars
		clients[clientNum].clientState = NOPLUGIN;
	}

	// reset client struct vars
	clients[clientNum].allowedUCh = 4;
	clients[clientNum].lastCheckCmd = -7000;
	clients[clientNum].checkScriptTime = 0;
	clients[clientNum].nextCPtime = 0;
	clients[clientNum].thinks = 0;

	// Bots are ignored by the anticheat system
	if ( isBot )
		clients[clientNum].clientState = BOT;

	return NULL;
}

// Called when a bot connects
EXPORT int SV_BotAllocateClient_Hook()
{
	int res = Ori_SV_BotAllocateClient();

	if ( res != -1 )
		CheckPerson(res, qtrue, qtrue);

	return res;
}

void ResetSVSTime(qboolean restart, int *time)
{
	// reset svs.time on new maps to fix the weapon glowing bug
	if ( jk2mf_fixwpglowing->integer && !restart )
	{
		*svtime = 0;
		*time = 0;
	}
}

// ==============
// SV_LocateGameData
//
// Fixes turret & seeker crash
// ==============
EXPORT void SV_LocateGameData_Hook(gentity_s *gEnts, int numGEntities, int sizeofGEntity_t, void *clients, int sizeofGameClient)
{
	// save the gEnts pointer - we need this also for the rocket bugfix
	gEntsp = gEnts;
	sizeofEntsp = sizeofGEntity_t;

	if ( jk2mf_fixturretcrash->integer )
	{
		int remEnts = 0;
		if ( numGEntities > 1020 )
		{
			for ( int i = 32; i < numGEntities; i++ )
			{
				// Can't just use gEnts[] because the size of gentitiy_t is different in each mod.
				gentity_s *ent = (gentity_s*)((uint)gEnts + (uint)sizeofGEntity_t * (uint)i);

				if ( ent->s.eType == ET_EVENTS + EV_SABER_BLOCK || ( ent->s.weapon == WP_TURRET && ent->s.eType == ET_MISSILE ) )
				{
					// Too many tempEntitys and missiles. Remove them all.
					Imp_SV_UnlinkEntity(ent);
					memset(ent, 0, sizeofGEntity_t);
					remEnts++;
				}
			}
		}
		Ori_SV_LocateGameData(gEnts, numGEntities - remEnts, sizeofGEntity_t, clients, sizeofGameClient);
	}
	else
		Ori_SV_LocateGameData(gEnts, numGEntities, sizeofGEntity_t, clients, sizeofGameClient);
}

// ==============
// SVC_RemoteCommand
//
// Fixing rconblocking
// ==============
EXPORT void SVC_RemoteCommand_Hook(netadr_t from, msg_t *msg)
{
	if ( jk2mf_fixrconblocking->integer )
		*lastRconTime = 0; // set this to 0 so lastTime + 500 will always be < svs.time

	Ori_SVC_RemoteCommand(from, msg);
}

// ==============
// ClientThink
//
// Used to fix the rocket and saberstealing bug
// ==============
void ClientThink(int clientNum)
{
	if ( jk2mf_fixrocketbug->integer )
	{
		// Get current clients usercmd - hardcoded
		usercmd_t *cmd = (usercmd_t*)((uint)212204 * (uint)clientNum + (uint)*svclients + 132124);
		gentity_s *ent = (gentity_s*)((uint)gEntsp + (uint)sizeofEntsp * (uint)clientNum);
		int *weaponStatePtr = (int*)Imp_VM_ArgPtr((int)&ent->client->ps.weaponstate); // Don't allow targeting while changing weapon
		int *forceHandExtendPtr = (int*)Imp_VM_ArgPtr((int)&ent->client->ps.forceHandExtend); // Don't allow targeting while using the force

		// Reset rocketLockTime on use of specific forcepowers, animations and if weapon is changed
		if ( cmd->generic_cmd == GENCMD_SABERSWITCH || cmd->generic_cmd == GENCMD_ENGAGE_DUEL || cmd->generic_cmd == GENCMD_FORCE_THROW ||
			 cmd->generic_cmd == GENCMD_FORCE_PULL || cmd->generic_cmd == GENCMD_FORCE_DISTRACT || cmd->generic_cmd == GENCMD_ZOOM ||
			 cmd->buttons & BUTTON_GESTURE || cmd->buttons & BUTTON_FORCEGRIP || cmd->buttons & BUTTON_FORCE_LIGHTNING || cmd->buttons & BUTTON_FORCE_DRAIN ||
			 *weaponStatePtr == WEAPON_DROPPING || *forceHandExtendPtr )
		{
			float *rockLockTimePtr = (float*)Imp_VM_ArgPtr((int)&ent->client->ps.rocketLockTime);
			int *rocketLockIndexPtr = (int*)Imp_VM_ArgPtr((int)&ent->client->ps.rocketLockIndex);
			float *rocketTargetTimePtr = (float*)Imp_VM_ArgPtr((int)&ent->client->ps.rocketTargetTime);

			*rockLockTimePtr = 0;
			*rocketLockIndexPtr = MAX_CLIENTS;
			*rocketTargetTimePtr = 0;
		}
	}

	if ( jk2mf_fixsaberstealing->integer )
	{
		gentity_s *ent = (gentity_s*)((uint)gEntsp + (uint)sizeofEntsp * (uint)clientNum);
		playerState_s *clState = (playerState_s*)Imp_VM_ArgPtr((int)&ent->client->ps);

		// Spectators don't own sabers
		if ( clState->persistant[PERS_TEAM] == TEAM_SPECTATOR )
		{
			// -1 seems to work without any problems
			clState->saberEntityNum = -1;
		}
	}

	if ( jk2mf_fixflooding->integer )
	{
		if ( strlen(clients[clientNum].nextUInfStr) )
		{
			if ( clients[clientNum].nextUInfTime <= *svtime )
			{
				client_t *cl = (client_t*)((uint)212204 * (uint)clientNum + (uint)*svclients);
				SetUserInfo(cl, clients[clientNum].nextUInfStr);
				memset(clients[clientNum].nextUInfStr, 0, sizeof(clients[0].nextUInfStr));
			}
		}
	}

	// Do not count timeouts to the communication counter
	if ( *svtime - clients[clientNum].lastClientThink > 1500 && clients[clientNum].lastClientThink )
		clients[clientNum].chkSumTimeout += *svtime - clients[clientNum].lastClientThink;

	// Send information to the client if the plugin wasn't found
	// This is done here because on ClientBegin the Init-CMD isn't received yet
	if ( clients[clientNum].thinks == 20 && clients[clientNum].clientState == NOPLUGIN )
	{
		SendBadClientNotification(clientNum, NOPLUGIN);
		Ori_VM_Call(*gvm, JK2MF_CLIENT_CHEATSTATUS_CHANGED, clientNum);
	}

	// Send the cmd to create checksums to the client
	if ( jk2mf_anticheat->integer && clients[clientNum].thinks >= 25 && clients[clientNum].clientState == GOODCLIENT && *svtime - clients[clientNum].lastCheckCmd >= 5000 )
	{
		Imp_SV_GameSendServerCommand(clientNum, "mfclcmd chksum");
		clients[clientNum].lastCheckCmd = *svtime;
	}

	// If we didn't receive checksums from a client for more than 12 seconds we can't allow the client to play
	if ( jk2mf_anticheat->integer && clients[clientNum].thinks >= 30 && clients[clientNum].chkSumTimeout <= *svtime && !clients[clientNum].sentCorrputMsg && clients[clientNum].clientState == GOODCLIENT )
	{
		SendBadClientNotification(clientNum, BROKENCOMMUNICATION);
		clients[clientNum].sentCorrputMsg = true;
		clients[clientNum].clientState = BROKENCOMMUNICATION;
	}

	SendCenterPrintNotifications(clientNum);

	clients[clientNum].thinks++;
	clients[clientNum].lastClientThink = *svtime;
}

// ==============
// SV_ExecuteClientCommand
//
// Receiving clients checksums and pluginversion.
// Bypasses sv_floodprotect and doesn't print it to
// the console even if the developer cvar is set.
// you mad hax0rs?
// ==============
EXPORT void SV_ExecuteClientCommand_Hook(client_t *cl, const char *s, qboolean clientOK)
{
	// is this something jk2mf related?
	if ( !Q_strncmp(s, "mfsrvcmd ", 9) )
	{
		Imp_Cmd_TokenizeString(s);

		int clientNum = ((uint)cl - (uint)*svclients) / (uint)212204;
		MFClCommand(clientNum);

		// Decrease nextReliableTime so sv_floodprotect will not block chat messages etc.
		int *nextReliableTime = (int*)((uint)cl + (uint)133384);
		*nextReliableTime = *nextReliableTime - 1000;
	}
	else
		Ori_SV_ExecuteClientCommand(cl, s, clientOK);
}

void MFClCommand(int clientNum)
{
	char arg1[MAX_TOKEN_CHARS];
	char arg2[MAX_TOKEN_CHARS];
	char arg3[MAX_TOKEN_CHARS];
	Q_strncpyz(arg1, Imp_Cmd_Argv(1), sizeof(arg1));
	Q_strncpyz(arg2, Imp_Cmd_Argv(2), sizeof(arg2));
	Q_strncpyz(arg3, Imp_Cmd_Argv(3), sizeof(arg3));

	if ( !strcmp(arg1, "version") )
	{
		// The client sent it's version to the server, check it
		if ( !strcmp(arg2, CLIENTVERSION) )
		{
			if (clients[clientNum].clientState == NOPLUGIN) {
				clients[clientNum].clientState = GOODCLIENT;
				Ori_VM_Call(*gvm, JK2MF_CLIENT_CHEATSTATUS_CHANGED, clientNum);
			}

			// Set OS string
			Q_strncpyz(clients[clientNum].OS, arg3, sizeof(clients[0].OS));

			// Send anticheat state now
			Imp_SV_GameSendServerCommand(clientNum, va("mfclcmd state %i", jk2mf_anticheat->integer));
		}

		return;
	}

	if ( !jk2mf_anticheat->integer )
		return;

	if ( !strcmp(arg1, "chksum") )
	{
		if ( clients[clientNum].clientState >= NOPLUGIN )
			return;

		// Decrypt messages
		if ( !DecryptMessage(arg2) || !DecryptMessage(arg3) )
			return;

		// Checksums received
		clients[clientNum].chkSumTimeout = *svtime + 16000;

		// Check if this is the correct checksum of the binary
		if (strcmp(arg2, JK2MP104EXECHKSUM) && strcmp(arg2, JK2MP102EXECHKSUM) && strcmp(arg2, ASPYRCHKSUM))
		{
			if ( clients[clientNum].clientState != CODEMODIFICATION )
				SendBadClientNotification(clientNum, CODEMODIFICATION);

			clients[clientNum].clientState = CODEMODIFICATION;
			Ori_VM_Call(*gvm, JK2MF_CLIENT_CHEATSTATUS_CHANGED, clientNum);
			return;
		}

		// Check if this is the correct checksum (cgame.qvm)
		uint chkSum = 0;
		sscanf(arg3, "%u", &chkSum);
		for ( int i = 0; i < 20; i++ )
		{
			if ( chkSum == cgameValids[i] )
				return;
		}

		// Client plays with an unallowed cgame.
		clients[clientNum].clientState = INVALIDMOD;
		SendBadClientNotification(clientNum, INVALIDMOD);
		Ori_VM_Call(*gvm, JK2MF_CLIENT_CHEATSTATUS_CHANGED, clientNum);
	}
	else if ( !strcmp(arg1, "scripter") )
	{
		// Client tried to use a script
		if ( *svtime - clients[clientNum].lastScriptTime > 10000 && clients[clientNum].checkScriptTime < *svtime && clients[clientNum].checkScriptTime  )
		{
			if ( clients[clientNum].clientState != GOODCLIENT && clients[clientNum].clientState != SCRIPTER )
				return;

			SendBadClientNotification(clientNum, SCRIPTER);
			clients[clientNum].lastScriptTime = *svtime;

			if ( clients[clientNum].clientState != GOODCLIENT )
				return;

			clients[clientNum].clientState = SCRIPTER;
			Ori_VM_Call(*gvm, JK2MF_CLIENT_CHEATSTATUS_CHANGED, clientNum);
		}
	}
}

void ClientBegin(int clientNum)
{
	memset(&clients[clientNum].IP, 0, sizeof(clients[0].IP));

	clients[clientNum].checkScriptTime = *svtime + 4000;
	clients[clientNum].chkSumTimeout = *svtime + 16000;

	#ifdef WIN32
		Imp_SV_GameSendServerCommand(clientNum, va("print \"^1[ ^7JK2MF ^1v^7%s win-x86 ^1| ^7http://jk2.ouned.de/jk2mf/ ^1]^7\n\"", VERSION));
	#else
		Imp_SV_GameSendServerCommand(clientNum, va("print \"^1[ ^7JK2MF ^1v^7%s linux-i386 ^1| ^7http://jk2.ouned.de/jk2mf/ ^1]^7\n\"", VERSION));
	#endif

	if ( jk2mf_anticheat->integer && jk2mf_forceplugin->integer )
	{
		Imp_SV_GameSendServerCommand(clientNum, "print \"^1[ ^7Anticheat is activated ^1]^7\n\"");
	}
	else if ( jk2mf_anticheat->integer && !jk2mf_forceplugin->integer )
	{
		Imp_SV_GameSendServerCommand(clientNum, "print \"^1[ ^7Anticheat is optional ^1]^7\n\"");
		Imp_SV_GameSendServerCommand(clientNum, "print \"^1[ ^7Use /cheaters to see a list of players ^1]^7\n\"");
	}
}

// ==============
// SV_GetUsercmd
//
// Blocking bad clients by ignoring the userinput
// ==============
EXPORT void SV_GetUsercmd_Hook(int clientNum, usercmd_t *cmd)
{
	Ori_SV_GetUsercmd(clientNum, cmd);
	if ( jk2mf_forceplugin->integer && clients[clientNum].clientState >= NOPLUGIN && blockCMDmod )
	{
		// Don't allow any client input
		memset(&cmd->angles, 0, sizeof(usercmd_t) - sizeof(int));
	}
}

void SendBadClientNotification(int clientNum, byte msgType)
{
	if ( msgType == NOPLUGIN )
	{
		Imp_SV_GameSendServerCommand(clientNum, "print \"^1==========\n"
												"^7You don't have the JK2MF clientside installed^1.\n"
												"^7Download at http^1://^7jk2^1.^7ouned^1.^7de^1/^7jk2mf^1/^7\n"
												"^1==========^7\n\"");

		if ( jk2mf_forceplugin->integer )
			clients[clientNum].nextCPtime = *svtime;

		return;
	}

	if ( !jk2mf_forceplugin->integer )
		return;

	client_t *cl = (client_t*)((uint)212204 * (uint)clientNum + (uint)*svclients);
	char *clName = (char*)((uint)cl + (uint)133188);

	switch ( msgType )
	{
		case CODEMODIFICATION:
			Imp_SV_GameSendServerCommand(-1, va("print \"^7JK2MF^1: ^7%s ^7code modification detected^1.^7\n\"", clName));
			break;
		case BROKENCOMMUNICATION:
			Imp_SV_GameSendServerCommand(-1, va("print \"^7JK2MF^1: ^7%s ^7communication failure^1.^7\n\"", clName));
			break;
		case SCRIPTER:
			Imp_SV_GameSendServerCommand(-1, va("print \"^7JK2MF^1: ^7%s ^7tried to use a script^1.^7\n\"", clName));
			break;
		case INVALIDMOD:
			Imp_SV_GameSendServerCommand(-1, va("print \"^7JK2MF^1: ^7%s ^7Is using an unallowed clientside (cgame.qvm)^1.^7\n\"", clName));
			break;
	}
}

void SendCenterPrintNotifications(int clientNum)
{
	if ( clients[clientNum].nextCPtime > *svtime || !clients[clientNum].nextCPtime )
		return;

	switch ( clients[clientNum].clientState )
	{
		case NOPLUGIN:
			Imp_SV_GameSendServerCommand(clientNum, va("cp \"^1[ ^7JK2MF %s^1 ]\n\n^7"
															"You need the JK2MF clientside to\n"
															"play on this server.\n\n"
															"Download at:\n^1http://jk2.ouned.de/jk2mf/\"", VERSION));
	}

	clients[clientNum].nextCPtime = *svtime + 2000;
}

EXPORT void SV_ConnectionlessPacket_Hook(netadr_t from, msg_t *msg)
{
	// Blocked?
	for ( int i = 0; i < numblockedIPs; i++ )
		if ( blockedIPs[i][0] == from.ip[0] && blockedIPs[i][1] == from.ip[1] && ( blockedIPs[i][2] == from.ip[2] || blockedIPs[i][2] == 0 ) && ( blockedIPs[i][3] == from.ip[3] || blockedIPs[i][3] == 0 ) )
			return;

	// Per IP Flood protection
	bool found = false;

	for ( int i = 0; i < sizeof(conlessIPs) / sizeof(conless_t); i++ )
	{
		if ( conlessIPs[i].adr.ip[0] == from.ip[0] && conlessIPs[i].adr.ip[1] == from.ip[1] && conlessIPs[i].adr.ip[2] == from.ip[2] && conlessIPs[i].adr.ip[3] == from.ip[3] )
		{
			if (jk2mf_fixudpflooding->integer && conlessIPs[i].thisSecondNum >= jk2mf_fixudpflooding->integer)
				return;

			conlessIPs[i].thisSecondNum++;
			found = true;
			break;
		}
	}

	if ( !found )
	{
		for ( int i = 0; i < sizeof(conlessIPs) / sizeof(conless_t); i++ )
		{
			if ( conlessIPs[i].adr.ip[0] == 0 )
			{
				memcpy(&conlessIPs[i].adr, &from, sizeof(netadr_t));
				conlessIPs[i].thisSecondNum++;
				break;
			}
		}
	}

	Ori_SV_ConnectionlessPacket(from, msg);

	char msgStr[500];
	Q_strncpyz(msgStr, Imp_Cmd_Argv(0), sizeof(msgStr));

	// This can be used to make serverside only patches by sending a different version then VERSION
	if ( !Q_stricmp(msgStr, "jk2mfversion") )
	{
		Imp_NET_OutOfBandPrint(NS_SERVER, from, "%s", CLIENTVERSION);
	}
	else if ( !Q_stricmp(msgStr, "jk2mfport") )
	{
		if ( jk2mf_httpdownloads->integer )
			Imp_NET_OutOfBandPrint(NS_SERVER, from, jk2mf_httpserverport->string);
		else
			Imp_NET_OutOfBandPrint(NS_SERVER, from, "disabled");
	}
	else
	{
		if ( !Q_stricmp(msgStr, "getstatus") || !Q_stricmp(msgStr, "getinfo") || !Q_stricmp(msgStr, "getchallenge") || !Q_stricmp(msgStr, "connect") ||
			 !Q_stricmp(msgStr, "ipAuthorize") || !Q_stricmp(msgStr, "rcon") || !Q_stricmp(msgStr, "disconnect") )
			 return;

		// Call the API
		memcpy(&lastAdr, &from, sizeof(netadr_t));
		Ori_VM_Call(*gvm, JK2MF_CONNECTIONLESS_PACKET, from.ip[0], from.ip[1], from.ip[2], from.ip[3]);
	}
}

// ==============
// GenerateModChecksums
//
// Get checksums of all allowed clientsides.
// Could still be manipulated in the memory but never saw someone doing this so far.
// ==============
void GenerateModChecksums()
{
	char list[2048];
	int files = Imp_FS_GetFileList(va("../%s/clplugins/", jk2mfpathrel), ".qvm", list, sizeof(list));

	// clear old checksums
	memset(cgameValids, 0, sizeof(cgameValids));

	char *thisFileName = list;
	int thisFileLen = 0;
	for ( int i = 0; i < files; i++, thisFileName += thisFileLen+1 )
	{
		thisFileLen = strlen(thisFileName);

		FILE *handle;
		handle = fopen(va("%s/clplugins/%s", jk2mfpath, thisFileName), "rb");

		fseek(handle, 0, SEEK_END);
		uint fileLen = ftell(handle);
		fseek(handle, 0, SEEK_SET);

		char *fBuff = (char*)malloc(fileLen+1);

		fread(fBuff, fileLen, 1, handle);
		fclose(handle);

		cgameValids[i] = hash(fBuff, fileLen, 25790);

		free(fBuff);
	}
}

EXPORT void Com_DPrintf_Hook(const char *fmt, ...) {
	va_list		argptr;
	char		msg[4096];

	if ( !com_developer || !com_developer->integer ) {
		return;
	}

	va_start (argptr,fmt);
	vsnprintf(msg, sizeof(msg), fmt, argptr);
	va_end (argptr);

	// Don't print secret anticheat cmd's to the console
	if ( strstr(msg, "jk2mfversion") || strstr(msg, "mfsrvcmd") )
		return;

	// Don't print stupid "bad connectionless packet" shit
	if ( strstr(msg, "bad connectionless packet from ") )
		return;

	Imp_Com_Printf("%s", msg);
}

// ==============
// Download system
//
// Fixing q3dirtrav by checking the filepath
// ==============
EXPORT const char *FS_ReferencedPakNames_Hook()
{
	static char	info[BIG_INFO_STRING];
	searchpath_t *search;

	info[0] = 0;
	for ( search = *fs_searchpaths; search; search = search->next )
	{
		if ( search->pack )
		{
			if ( ReferencePack(search->pack) )
			{
				Q_strcat(info, sizeof(info), search->pack->pakGamename);
				Q_strcat(info, sizeof(info), "/" );
				Q_strcat(info, sizeof(info), search->pack->pakBasename);
				Q_strcat(info, sizeof(info), " ");
			}
		}
	}

	return info;
}

EXPORT const char *FS_ReferencedPakChecksums_Hook()
{
	static char	info[BIG_INFO_STRING];
	searchpath_t *search;

	info[0] = 0;
	for ( search = *fs_searchpaths; search; search = search->next )
	{
		if ( search->pack )
		{
			if ( ReferencePack(search->pack) )
			{
				Q_strcat( info, sizeof( info ), va("%i ", search->pack->checksum ) );
			}
		}
	}

	return info;
}

void ReadLists()
{
	whitelist = ReadFileHeap(va("%s/whitelist.txt", jk2mfpath));
	blacklist = ReadFileHeap(va("%s/blacklist.txt", jk2mfpath));
	reflist = ReadFileHeap(va("%s/reflist.txt", jk2mfpath));
}

void UnloadLists()
{
	if ( whitelist )
	{
		free(whitelist);
		whitelist = 0;
	}

	if ( blacklist )
	{
		free(blacklist);
		blacklist = 0;
	}

	if ( reflist )
	{
		free(reflist);
		reflist = 0;
	}
}

bool ReferencePack(pack_t *pack)
{
	// do not allow referencing of clientsides in the base folder
	// even ignore the reflist in this case
	if ((pack->referenced & (FS_CGAME_REF | FS_UI_REF)) && !Q_stricmpn(pack->pakGamename, "base", 4)) {
		return false;
	}

	if ( reflist )
	{
		if ( stristr(reflist, va("\n%s/%s.pk3", pack->pakGamename, pack->pakBasename)) )
		{
			return true;
		}
	}

	if ( whitelist )
	{
		if ( !stristr(whitelist, va("\n%s/%s.pk3", pack->pakGamename, pack->pakBasename)) )
		{
			return false;
		}
	}
	else if ( blacklist )
	{
		if ( stristr(blacklist, va("\n%s/%s.pk3", pack->pakGamename, pack->pakBasename)) )
		{
			return false;
		}
	}

	if ( pack->referenced )
		return true;

	if ( Q_stricmpn(pack->pakGamename, "base", 4) )
		return true;

	return false;
}

bool ReferencePack(const char *gameBaseName, bool fullPath)
{
	pack_t *pack;
	pack = PackForPath(gameBaseName, fullPath);

	if ( pack )
		return ReferencePack(pack);
	else
		return false;
}

EXPORT void SV_BeginDownload_Hook(client_t *cl)
{
	char downName[MAX_QPATH];
	Q_strncpyz(downName, Imp_Cmd_Argv(1), sizeof(downName));
	int len = strlen(downName);

	// Do not send this file if it's path contains either ".." or it's extension is not .pk3
	if ( ( jk2mf_fixq3dirtrav->integer && ( len <= 4 || stristr(downName, "..") || downName[len-4] != '.' || downName[len-3] != 'p' || downName[len-2] != 'k' || downName[len-1] != '3' ) ) || !ReferencePack(downName, false) )
	{
		// SV_WriteDownloadToClient will stop the download if downloadName is empty
		char *downloadName = (char*)((uint)cl + (uint)133220);
		memset(downloadName, 0, MAX_QPATH);
	}
	else
	{
		int clientNum = ((uint)cl - (uint)*svclients) / (uint)212204;

		pack_t *reqFile = PackForPath(downName, false);
		Q_strncpyz(lastReq.fileName, va("%s/%s.pk3", reqFile->pakGamename, reqFile->pakBasename), sizeof(lastReq.fileName));
		// set IP
		uint tmpIP[4];
		sscanf(clients[clientNum].IPSave, "%u.%u.%u.%u", &tmpIP[0], &tmpIP[1], &tmpIP[2], &tmpIP[3]);
		lastReq.IP[0] = tmpIP[0];
		lastReq.IP[1] = tmpIP[1];
		lastReq.IP[2] = tmpIP[2];
		lastReq.IP[3] = tmpIP[3];
		lastReq.protocol = REQUEST_UDP;
		lastReq.clientNum = clientNum;

		if ( Ori_VM_Call(*gvm, JK2MF_DOWNLOAD_REQUEST) == DOWNLOAD_REJECT )
		{
			// SV_WriteDownloadToClient will stop the download if downloadName is empty
			char *downloadName = (char*)((uint)cl + (uint)133220);
			memset(downloadName, 0, MAX_QPATH);
			return;
		}

		Ori_SV_BeginDownload(cl);
	}
}

// ==============
// WebServer
//
// Fast autodownloading of files using a modified version of the mongoose http server
// ==============
void WebServerStart()
{
	if ( jk2mf_httpdownloads->integer && !strstr(jk2mf_httpserverport->string, "http://") && !ctx )
	{
		if (jk2mf_httpserverport->integer) {
			const char *options[] = { "listening_ports", jk2mf_httpserverport->string,
				"document_root", fs_basepath->string,
				"num_threads", "5", NULL };

			ctx = mg_start(NULL, NULL, options);
		} else {
			for (int port = HTTPSRV_STDPORT; port < HTTPSRV_STDPORT + 10; port++) {
				char portstr[10];
				sprintf(portstr, "%i", port);

				const char *options[] = { "listening_ports", portstr,
					"document_root", fs_basepath->string,
					"num_threads", "5", NULL };

				ctx = mg_start(NULL, NULL, options);

				if (ctx) {
					Imp_Cvar_Set2("jk2mf_httpserverport", portstr, qtrue);
					break;
				}
			}
		}

		if (ctx) {
			Imp_Com_Printf("JK2MF: webserver running on port %s\n", jk2mf_httpserverport->string);
		} else {
			Imp_Com_Printf("JK2MF: webserver startup failed\n");
		}
	}
}

void WebServerEnd()
{
	if ( ( ( jk2mf_httpdownloads->latchedString && !atoi(jk2mf_httpdownloads->latchedString) ) || jk2mf_httpserverport->latchedString ) && ctx )
	{
		mg_stop(ctx);
		Imp_Com_Printf("JK2MF: webserver shutting down...\n");
		ctx = NULL;
	}
}

void WebServerFileRequestMainThread()
{
	bool res = ReferencePack(webPakStr, true);

	if ( res )
	{
		// Download will continue if the API allows it
		pack_t *reqFile = PackForPath(webPakStr, true);
		Q_strncpyz(lastReq.fileName, va("%s/%s.pk3", reqFile->pakGamename, reqFile->pakBasename), sizeof(lastReq.fileName));
		memcpy(lastReq.IP, webIP, sizeof(lastReq.IP));
		lastReq.protocol = REQUEST_HTTP;
		lastReq.clientNum = CLIENTNUM_UNKNOWN;

		if ( Ori_VM_Call(*gvm, JK2MF_DOWNLOAD_REQUEST) == DOWNLOAD_REJECT )
			webReturn = 0;
		else
			webReturn = 1;
	}
	else
		webReturn = 0;

	webReq = false;
}

// WARNING: Running in different Threads!
bool WebServerFileRequest(const char *path, sockaddr_in *addr)
{
	int len = strlen(path);

	if ( len <= 4 || stristr((char*)path, "..") || path[len-4] != '.' || path[len-3] != 'p' || path[len-2] != 'k' || path[len-1] != '3' )
		return false;
	else if ( stristr((char*)path, "assets0.pk3") || stristr((char*)path, "assets1.pk3") || stristr((char*)path, "assets2.pk3") || stristr((char*)path, "assets5.pk3") )
		return false;

	// wait till the main thread is ready to take the request
	while ( true )
	{
		LockMutex(webMutex);

		if ( !webReq && webReturn == -1 )
			break;

		UnlockMutex(webMutex);
		MilliSleep(10);
	}

	memcpy(webIP, &addr->sin_addr.s_addr, 4);
	webPakStr = path;
	webReq = true;
	UnlockMutex(webMutex);

	// wait for the result
	while ( true )
	{
		LockMutex(webMutex);

		if ( webReturn != -1 )
			break;

		UnlockMutex(webMutex);
		MilliSleep(10);
	}

	bool res;
	if ( webReturn == 0 )
		res = false;
	else
		res = true;

	webReturn = -1;
	UnlockMutex(webMutex);
	return res;
}
