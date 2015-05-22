#ifdef WIN32
	#include <windows.h>
#else
    #include <dlfcn.h>
#endif
#include "../qcommon/qcommon.h"
#include "../server/server.h"
#include "jk2mf.h"

#ifndef WIN32
typedef void* HMODULE;
#endif

HMODULE module;
jk2version_t jk2ver;

int (*JK2MF_VM_Call)(vm_t *vm, int callnum, ...);
char *(*JK2MF_MSG_ReadStringLine)(msg_t *msg);
void (*JK2MF_SV_GameSendServerCommand)(int clientNum, const char *text);
void (*JK2MF_SV_LocateGameData)(sharedEntity_t *gEnts, int numGEntities, int sizeofGEntity_t, void *clients, int sizeofGameClient);
void (*JK2MF_SV_BeginDownload)(client_t *cl);
void (*JK2MF_SVC_RemoteCommand)(netadr_t from, msg_t *msg);
void (*JK2MF_SV_UpdateUserinfo)(client_t *cl);
void *(*JK2MF_VM_ExplicitArgPtr)(vm_t *vm, int intValue);
void (*JK2MF_SV_ExecuteClientCommand)(client_t *cl, const char *s, qboolean clientOK);
void (*JK2MF_SV_GetUsercmd)(int clientNum, usercmd_t *cmd);
void (*JK2MF_SV_ConnectionlessPacket)(netadr_t from, msg_t *msg);
int (*JK2MF_SV_BotAllocateClient)();
void (*JK2MF_Com_DPrintf)(const char *fmt, ...);
const char *(*JK2MF_FS_ReferencedPakNames)();
const char *(*JK2MF_FS_ReferencedPakChecksums)();
vm_t *(*JK2MF_VM_Create)(const char *module, int(*systemCalls)(int *), vmInterpret_t interpret);
int (*JK2MF_SV_GameSystemCalls)(int *args);

#ifndef WIN32
void *GetProcAddress(HMODULE m, const char *name) {
    return dlsym(m, name);
}
#endif

qboolean JK2MF_Init() {
    #ifdef WIN32
	module = LoadLibraryA("jk2mf.dll");
	#else
	module = dlopen(va("%s/jk2mf.so", Cvar_VariableString("fs_basepath")), RTLD_LAZY);
	#endif

	if (!module)
		return qtrue;

	int(*JK2MFDED_GetApiVersion)() = (int(*)())GetProcAddress(module, "JK2MFDED_GetApiVersion");
	void(*JK2MFDED_Init)(void*(*ga)(int)) = (void(*)(void*(*)(int)))GetProcAddress(module, "JK2MFDED_Init");

	if (!JK2MFDED_GetApiVersion || JK2MFDED_GetApiVersion() != JK2MFDED_API_VERSION) {
		Com_Printf("This version of JK2MF is not supported in this version of jk2mfded.\n");
		module = NULL;
		return qtrue;
	}

	JK2MFDED_Init(GetAddr);

	JK2MF_VM_Call = (int(*)(vm_t *, int, ...))GetProcAddress(module, "VM_Call_Hook");
	JK2MF_MSG_ReadStringLine = (char*(*)(msg_t *))GetProcAddress(module, "MSG_ReadStringLine_Hook");
	JK2MF_SV_GameSendServerCommand = (void(*)(int, const char *))GetProcAddress(module, "SV_GameSendServerCommand_Hook");
	JK2MF_SV_LocateGameData = (void(*)(sharedEntity_t *, int, int, void *, int))GetProcAddress(module, "SV_LocateGameData_Hook");
	JK2MF_SV_BeginDownload = (void(*)(client_t *))GetProcAddress(module, "SV_BeginDownload_Hook");
	JK2MF_SVC_RemoteCommand = (void(*)(netadr_t, msg_t *))GetProcAddress(module, "SVC_RemoteCommand_Hook");
	JK2MF_SV_UpdateUserinfo = (void(*)(client_t *))GetProcAddress(module, "SV_UpdateUserinfo_Hook");
	JK2MF_VM_ExplicitArgPtr = (void *(*)(vm_t *, int))GetProcAddress(module, "VM_ExplicitArgPtr_Hook");
	JK2MF_SV_ExecuteClientCommand = (void(*)(client_t *, const char *, qboolean))GetProcAddress(module, "SV_ExecuteClientCommand_Hook");
	JK2MF_SV_GetUsercmd = (void(*)(int, usercmd_t *))GetProcAddress(module, "SV_GetUsercmd_Hook");
	JK2MF_SV_ConnectionlessPacket = (void(*)(netadr_t, msg_t *))GetProcAddress(module, "SV_ConnectionlessPacket_Hook");
	JK2MF_SV_BotAllocateClient = (int(*)())GetProcAddress(module, "SV_BotAllocateClient_Hook");
	JK2MF_Com_DPrintf = (void(*)(const char *, ...))GetProcAddress(module, "Com_DPrintf_Hook");
	JK2MF_FS_ReferencedPakNames = (const char *(*)())GetProcAddress(module, "FS_ReferencedPakNames_Hook");
	JK2MF_FS_ReferencedPakChecksums = (const char *(*)())GetProcAddress(module, "FS_ReferencedPakChecksums_Hook");
	JK2MF_VM_Create = (vm_t *(*)(const char *, int(*)(int *), vmInterpret_t))GetProcAddress(module, "VM_Create_Hook");
	JK2MF_SV_GameSystemCalls = (int(*)(int *))GetProcAddress(module, "SV_GameSystemCalls_Hook");

	return qfalse;
}

qboolean JK2MF_InUse() {
	if (module) return qtrue;
	else return qfalse;
}

void *GetAddr(int type) {
	switch (type) {
		case VM_CALL:
			return (void*)VM_Call_Ori;
		case COM_PRINTF:
			return (void*)Com_Printf;
		case MSG_READSTRINGLINE:
			return (void*)MSG_ReadStringLine_Ori;
		case MSG_READBYTE:
			return (void*)MSG_ReadByte;
		case SV_GETUSERINFO:
			return (void*)SV_GetUserinfo;
		case INFO_SETVALUEFORKEY:
			return (void*)Info_SetValueForKey;
		case INFO_VALUEFORKEY:
			return (void*)Info_ValueForKey;
		case SV_SETUSERINFO:
			return (void*)SV_SetUserinfo;
		case CVAR_GET:
			return (void*)Cvar_Get;
		case SV_GAMESENDSERVERCOMMAND:
			return (void*)SV_GameSendServerCommand_Ori;
		case SVS_TIME:
			return (void*)&svs.time;
		case CMD_ARGC:
			return (void*)Cmd_Argc;
		case CMD_ARGV:
			return (void*)Cmd_Argv;
		case SV_LOCATEGAMEDATA:
			return (void*)SV_LocateGameData_Ori;
		case SV_UNLINKENTITY:
			return (void*)SV_UnlinkEntity;
		case VM_ARGPTR:
			return (void*)VM_ArgPtr;
		case SV_BEGINDOWNLOAD:
			return (void*)SV_BeginDownload_f_Ori;
		case SVC_REMOTECOMMAND:
			return (void*)SVC_RemoteCommand_Ori;
		case LAST_RCONTIME:
			return (void*)&lasttime;
		case SV_UPDATEUSERINFO:
			return (void*)SV_UpdateUserinfo_f_Ori;
		case SVS_CLIENTS:
			return (void*)&svs.clients;
		case SV_USERINFOCHANGED:
			return (void*)SV_UserinfoChanged;
		case G_VM:
			return (void*)&gvm;
		case VM_EXPLICITARGPTR:
			return (void*)VM_ExplicitArgPtr_Ori;
		case SV_GETCONFIGSTRING:
			return (void*)SV_GetConfigstring;
		case SV_EXECUTECLIENTCOMMAND:
			return (void*)SV_ExecuteClientCommand_Ori;
		case MSG_READLONG:
			return (void*)MSG_ReadLong;
		case MSG_READSTRING:
			return (void*)MSG_ReadString;
		case CMD_TOKENIZESTRING:
			return (void*)Cmd_TokenizeString;
		case SV_GETUSERCMD:
			return (void*)SV_GetUsercmd_Ori;
		case SV_CONNECTIONLESSPACKET:
			return (void*)SV_ConnectionlessPacket_Ori;
		case NET_OUTOFBANDPRINT:
			return (void*)NET_OutOfBandPrint;
		case SV_BOTALLOCATECLIENT:
			return (void*)SV_BotAllocateClient_Ori;
		case FS_GETFILELIST:
			return (void*)FS_GetFileList;
		case COM_DPRINTF:
			return (void*)Com_DPrintf_Ori;
		case FS_REFPAKNAMES:
			return (void*)FS_ReferencedPakNames_Ori;
		case FS_SEARCHPATH:
			return (void*)FS_GetSearchPath();
		case FS_REFPAKCHKSUM:
			return (void*)FS_ReferencedPakChecksums_Ori;
		case VM_RESTART:
			return (void*)VM_Restart;
		case QVM_SRVNAME:
			return (void*)NULL; // not needed
		case VM_FREE:
			return (void*)VM_Free;
		case VM_CREATE:
			return (void*)VM_Create_Ori;
		case GAME_SYSCALLS:
			return (void*)SV_GameSystemCalls_Ori;
		case CVAR_SET2:
			return (void*)Cvar_Set2;
		default:
			return (void*)NULL; // bad things will happen for sure
	}
}

void JK2MF_SetGameVersion(jk2version_t ver) {
	jk2ver = ver;
}

jk2version_t JK2MF_GetGameVersion() {
	return jk2ver;
}
