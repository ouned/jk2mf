/***********************************
		   JK2MF Project

		 made for the lulz
***********************************/

#ifndef MAIN_H
#define MAIN_H

#ifdef WIN32
	#include <windows.h>
#else
    #include <netinet/in.h>
    #include <unistd.h>
#endif
#include <stdio.h>
#include <time.h>
#include "common.h"
#include "detours.h"
#include "mongoose.h"
#include "api.h"

#define VERSION "3.0.5"
#define CLIENTVERSION "3.0.5"

#define JK2MFDED_API_VERSION 1

#define HTTPSRV_STDPORT 18200

// Allowed characters in Names
#define VALIDNAMECHARS " aäbcdefghijklmnoöpqrstuüvwxyzßAÄBCDEFGHIJKLMNOÖPQRSTUÜVWXYZ1234567890^$&/()=?!´`'+#*.,:<>|{[]}-_~@%§"

// JK2MP104EXE valid checksum
#define JK2MP104EXECHKSUM "3981898944"

// JK2MP102EXE valid checksum
#define JK2MP102EXECHKSUM "2335185060"

// ASPYRJK valid checksum
#define ASPYRCHKSUM "2549259741"

enum {
	GOODCLIENT,
	BOT,
	SCRIPTER,
	/* ---- Reasons to block cmd's ---- */
	NOPLUGIN,
	CODEMODIFICATION,
	BROKENCOMMUNICATION,
	INVALIDMOD,
};

struct client {
	char IP[20];
	char IPSave[20];
	char OS[100];
	int nextUInfTime; // next svstime userinfo changing is allowed
	char nextUInfStr[MAX_INFO_STRING];
	int allowedUCh; // number of changes on map starts
	uint thinks; // number of ClientThink() calls this client had
	bool sendPlgMsgOnCmd;
	byte clientState; // plugin & cheat status
	int lastCheckCmd; // svstime the last checksums have been received
	int checkScriptTime;
	int lastClientThink;
	int chkSumTimeout;
	bool sentCorrputMsg;
	int lastScriptTime;
	int nextCPtime;
};

struct conless_t {
	netadr_t adr;
	int thisSecondNum;
};

/* Function Prototypes */
void Init();
void SetGameBinaryVersion();
char *CheckPerson(int clientNum, qboolean firstTime, qboolean isBot);
void ResetSVSTime(qboolean restart, int *time);
void Init_Cvars(vm_t *vm);
void PrintHugeBuffer(int clientNum, char *text);
bool ClientCommand(int clientNum);
void VerifyUserinfo(client_t *cl);
void SetModFilteredName(int clientNum);
void ClientThink(int clientNum);
void MFClCommand(int clientNum);
void PrintPlayersClient(int clientNum);
void FreezePlayer(int clientNum);
void ClientBegin(int clientNum);
void SendBadClientNotification(int clientNum, byte msgType);
void SendCenterPrintNotifications(int clientNum);
void GenerateModChecksums();
void SetUserInfo(client_t *cl, char *userinfo);
void WebServerStart();
void WebServerEnd();
void WebServerFileRequestMainThread();
bool WebServerFileRequest(const char *path, sockaddr_in *addr);
bool ReferencePack(pack_t *pack);
bool ReferencePack(const char *gameBaseName, bool fullPath);
void ReadLists();
void UnloadLists();
bool RconCommand();
void GameInit(vm_t *vm, int restart, int *time);

typedef int SV_GameSystemCalls_Def(int*);

#ifdef MAIN
	/* Binary imports */
	int (*Imp_VM_Call)(vm_t *vm, int callnum, ...);
	void (*Imp_Com_Printf)(const char *fmt, ...);
	char (*Imp_MSG_ReadStringLine)(msg_t *msg);
	int (*Imp_MSG_ReadByte)(msg_t *msg);
	void (*Imp_SV_GetUserinfo)(int index, char *buffer, int bufferSize);
	void (*Imp_Info_SetValueForKey)(char *s, const char *key, const char *value);
	void (*Imp_Info_RemoveKey)(char *s, const char *key);
	char *(*Imp_Info_ValueForKey)(const char *s, const char *key);
	void (*Imp_SV_SetUserinfo)(int index, const char *val);
	cvar_t *(*Imp_Cvar_Get)(const char *var_name, const char *var_value, int flags);
	void (*Imp_SV_GameSendServerCommand)(int clientNum, const char *text);
	int (*Imp_Cmd_Argc)();
	char *(*Imp_Cmd_Argv)(int arg);
	void (*Imp_SV_LocateGameData)(gentity_s *gEnts, int numGEntities, int sizeofGEntity_t, void *clients, int sizeofGameClient);
	void (*Imp_SV_UnlinkEntity)(gentity_s *gEnt);
	void *(*Imp_VM_ArgPtr)(int intValue);
	void (*Imp_SV_BeginDownload)(client_t *cl);
	void (*Imp_SVC_RemoteCommand)(netadr_t from, msg_t *msg);
	void (*Imp_SV_UpdateUserinfo)(client_t *cl);
	void (*Imp_SV_UserinfoChanged)(client_t *cl);
	void *(*Imp_VM_ExplicitArgPtr)(vm_t *vm, int intValue);
	void (*Imp_SV_GetConfigstring)(int index, char *buffer, int bufferSize);
	void (*Imp_SV_ExecuteClientCommand)(client_t *cl, const char *s, qboolean clientOK);
	int (*Imp_MSG_ReadLong)(msg_t *msg);
	char *(*Imp_MSG_ReadString)(msg_t *msg);
	void (*Imp_Cmd_TokenizeString)(const char *text_in);
	void (*Imp_SV_GetUsercmd)(int clientNum, usercmd_t *cmd);
	void (*Imp_SV_ConnectionlessPacket)(netadr_t from, msg_t *msg);
	void (*Imp_NET_OutOfBandPrint)(netsrc_t sock, netadr_t adr, const char *format, ...);
	int (*Imp_SV_BotAllocateClient)();
	int	(*Imp_FS_GetFileList)(const char *path, const char *extension, char *listbuf, int bufsize);
	void (*Imp_Com_DPrintf)(const char *fmt, ...);
	const char *(*Imp_FS_ReferencedPakNames)();
	const char *(*Imp_FS_ReferencedPakChecksums)();
	vm_t *(*Imp_VM_Restart)(vm_t *vm);
	void (*Imp_VM_Free)(vm_t *vm);
	vm_t *(*Imp_VM_Create)(const char *module, int (*systemCalls)(int *), vmInterpret_t interpret);
	int (*Imp_SV_GameSystemCalls)(int *args);
	cvar_t *(*Imp_Cvar_Set2)(const char *var_name, const char *value, qboolean force);

	int *svtime; // svs.time
	unsigned int* lastRconTime;
	client_t **svclients; // svs.clients
	vm_t **gvm; // current jk2mpgame
	searchpath_t **fs_searchpaths;

	/* Cvars */
	cvar_t *jk2mf_fixq3infoboom;
	cvar_t *jk2mf_fixforcecrash;
	cvar_t *jk2mf_fixnamecrash;
	cvar_t *jk2mf_fixq3filluic;
	cvar_t *jk2mf_fixq3fillmcc;
	cvar_t *jk2mf_fixflooding;
	cvar_t *jk2mf_fixq3msgboom;
	cvar_t *jk2mf_fixturretcrash;
	cvar_t *jk2mf_fixq3cbufexec;
	cvar_t *jk2mf_fixq3dirtrav;
	cvar_t *jk2mf_fixrconblocking;
	cvar_t *jk2mf_fixwpglowing;
	cvar_t *jk2mf_fixnamediff;
	cvar_t *jk2mf_fixrocketbug;
	cvar_t *jk2mf_fixteamcrash;
	cvar_t *jk2mf_fixsaberstealing;
	cvar_t *jk2mf_fixudpflooding;
	cvar_t *jk2mf_fixgalaking;
	cvar_t *jk2mf_fixbrokenmodels;

	cvar_t *jk2mf_forceplugin;
	cvar_t *jk2mf_anticheat;

	cvar_t *jk2mf_httpdownloads;
	cvar_t *jk2mf_httpserverport;

	cvar_t *jk2mf_vmgame;
	cvar_t *jk2mf_apilevel;

	cvar_t *sv_maxclients;
	cvar_t *fs_homepath;
	cvar_t *fs_basepath;
	cvar_t *com_developer;
	cvar_t *jk2mf_loadmethod;
	cvar_t *mapname;
	cvar_t *fs_game;
	cvar_t *vm_game;

	/* Hooks */
	EXPORT int VM_Call_Hook(vm_t *vm, int callnum, ...);
	typedef int VM_Call_Def(vm_t *, int, ...);
	VM_Call_Def *Ori_VM_Call;

	EXPORT char *MSG_ReadStringLine_Hook(msg_t *msg);
	typedef char* MSG_ReadStringLine_Def(msg_t *);
	MSG_ReadStringLine_Def *Ori_MSG_ReadStringLine;

	EXPORT void SV_GameSendServerCommand_Hook(int clientNum, const char *text);
	typedef void SV_GameSendServerCommand_Def(int, const char*);
	SV_GameSendServerCommand_Def *Ori_SV_GameSendServerCommand;

	EXPORT void SV_LocateGameData_Hook(gentity_s *gEnts, int numGEntities, int sizeofGEntity_t, void *clients, int sizeofGameClient);
	typedef void SV_LocateGameData_Def(void*, int, int, void*, int);
	SV_LocateGameData_Def *Ori_SV_LocateGameData;

	EXPORT void SV_BeginDownload_Hook(client_t *cl);
	typedef void SV_BeginDownload_Def(client_t*);
	SV_BeginDownload_Def *Ori_SV_BeginDownload;

	EXPORT void SVC_RemoteCommand_Hook(netadr_t from, msg_t *msg);
	typedef void SVC_RemoteCommand_Def(netadr_t, msg_t*);
	SVC_RemoteCommand_Def *Ori_SVC_RemoteCommand;

	EXPORT void SV_UpdateUserinfo_Hook(client_t *cl);
	typedef void SV_UpdateUserinfo_Def(client_t*);
	SV_UpdateUserinfo_Def *Ori_SV_UpdateUserinfo;

	EXPORT void *VM_ExplicitArgPtr_Hook(vm_t *vm, int intValue);
	typedef void* VM_ExplicitArgPtr_Def(vm_t*, int);
	VM_ExplicitArgPtr_Def *Ori_VM_ExplicitArgPtr;

	EXPORT void SV_ExecuteClientCommand_Hook(client_t *cl, const char *s, qboolean clientOK);
	typedef void SV_ExecuteClientCommand_Def(client_t*, const char*, qboolean);
	SV_ExecuteClientCommand_Def *Ori_SV_ExecuteClientCommand;

	EXPORT void SV_GetUsercmd_Hook(int clientNum, usercmd_t *cmd);
	typedef void SV_GetUsercmd_Def(int, usercmd_t*);
	SV_GetUsercmd_Def *Ori_SV_GetUsercmd;

	EXPORT void SV_ConnectionlessPacket_Hook(netadr_t from, msg_t *msg);
	typedef void SV_ConnectionlessPacket_Def(netadr_t, msg_t*);
	SV_ConnectionlessPacket_Def *Ori_SV_ConnectionlessPacket;

	EXPORT int SV_BotAllocateClient_Hook();
	typedef int SV_BotAllocateClient_Def();
	SV_BotAllocateClient_Def *Ori_SV_BotAllocateClient;

	EXPORT void Com_DPrintf_Hook(const char *fmt, ...);
	typedef void Com_DPrintf_Def(const char*, ...);
	Com_DPrintf_Def *Ori_Com_DPrintf;

	EXPORT const char *FS_ReferencedPakNames_Hook();
	typedef const char* FS_ReferencedPakNames_Def();
	FS_ReferencedPakNames_Def *Ori_FS_ReferencedPakNames;

	EXPORT const char *FS_ReferencedPakChecksums_Hook();
	typedef const char* FS_ReferencedPakChecksums_Def();
	FS_ReferencedPakChecksums_Def *Ori_FS_ReferencedPakChecksums;

	EXPORT vm_t *VM_Create_Hook(const char *module, int(*systemCalls)(int *), vmInterpret_t interpret);
	typedef vm_t* VM_Create_Def(const char*, int(*)(int*), vmInterpret_t);
	VM_Create_Def *Ori_VM_Create;

	EXPORT int SV_GameSystemCalls_Hook(int *args);
	SV_GameSystemCalls_Def *Ori_SV_GameSystemCalls;

	typedef cvar_t* Cvar_Set2_Def(const char*, const char*, qboolean);
#else
	extern client clients[32];
	extern client_t **svclients;
	extern cvar_t *sv_maxclients;
	extern cvar_t *jk2mf_anticheat;
	extern time_t startTime;
	extern void *(*Imp_VM_ArgPtr)(int intValue);
	extern SV_GameSystemCalls_Def *Ori_SV_GameSystemCalls;
	extern bool blockCMDmod;
	extern void (*Imp_NET_OutOfBandPrint)(netsrc_t sock, netadr_t adr, const char *format, ...);
	extern netadr_t lastAdr;
	extern searchpath_t **fs_searchpaths;
	extern mfDownloadInfo lastReq;
	extern byte blockedIPs[500][4];
	extern int numblockedIPs;
	extern void*(*gae)(int type);
#endif

#ifdef _DEBUG
	#define TESTMSG(TEXT) MessageBoxA(NULL, TEXT, "", NULL);
#endif

#endif /* MAIN_H */
