#ifndef JK2MF
#define JK2MF

#include "../qcommon/qcommon.h"
#include "../game/q_shared.h"
#include "../server/server.h"

#define JK2MFDED_VERSION "1.5"

#define JK2MFDED_API_VERSION 1

extern int(*JK2MF_VM_Call)(vm_t *vm, int callnum, ...);
extern char *(*JK2MF_MSG_ReadStringLine)(msg_t *msg);
extern void(*JK2MF_SV_GameSendServerCommand)(int clientNum, const char *text);
extern void(*JK2MF_SV_LocateGameData)(sharedEntity_t *gEnts, int numGEntities, int sizeofGEntity_t, void *clients, int sizeofGameClient);
extern void(*JK2MF_SV_BeginDownload)(client_t *cl);
extern void(*JK2MF_SVC_RemoteCommand)(netadr_t from, msg_t *msg);
extern void(*JK2MF_SV_UpdateUserinfo)(client_t *cl);
extern void *(*JK2MF_VM_ExplicitArgPtr)(vm_t *vm, int intValue);
extern void(*JK2MF_SV_ExecuteClientCommand)(client_t *cl, const char *s, qboolean clientOK);
extern void(*JK2MF_SV_GetUsercmd)(int clientNum, usercmd_t *cmd);
extern void(*JK2MF_SV_ConnectionlessPacket)(netadr_t from, msg_t *msg);
extern int(*JK2MF_SV_BotAllocateClient)();
extern void(*JK2MF_Com_DPrintf)(const char *fmt, ...);
extern const char *(*JK2MF_FS_ReferencedPakNames)();
extern const char *(*JK2MF_FS_ReferencedPakChecksums)();
extern vm_t *(*JK2MF_VM_Create)(const char *module, int(*systemCalls)(int *), vmInterpret_t interpret);
extern int(*JK2MF_SV_GameSystemCalls)(int *args);

enum {
	VM_CALL, COM_PRINTF, MSG_READSTRINGLINE, MSG_READBYTE,
	SV_GETUSERINFO, INFO_SETVALUEFORKEY, INFO_VALUEFORKEY,
	SV_SETUSERINFO, CVAR_GET, SV_GAMESENDSERVERCOMMAND,
	SVS_TIME, CMD_ARGC, CMD_ARGV, SV_LOCATEGAMEDATA,
	SV_UNLINKENTITY, VM_ARGPTR, SV_BEGINDOWNLOAD,
	SVC_REMOTECOMMAND, LAST_RCONTIME, SV_UPDATEUSERINFO,
	SVS_CLIENTS, SV_USERINFOCHANGED, G_VM, VM_EXPLICITARGPTR,
	SV_GETCONFIGSTRING, SV_EXECUTECLIENTCOMMAND, MSG_READLONG,
	MSG_READSTRING, CMD_TOKENIZESTRING, SV_GETUSERCMD,
	SV_CONNECTIONLESSPACKET, NET_OUTOFBANDPRINT, SV_BOTALLOCATECLIENT,
	FS_GETFILELIST, COM_DPRINTF, FS_REFPAKNAMES, FS_SEARCHPATH, FS_REFPAKCHKSUM,
	VM_RESTART, QVM_SRVNAME, VM_FREE, VM_CREATE, GAME_SYSCALLS, CVAR_SET2
};

typedef enum {
	JK2_102,
	JK2_104,
} jk2version_t;

qboolean JK2MF_Init();
qboolean JK2MF_InUse();
void *GetAddr(int type);
void JK2MF_SetGameVersion(jk2version_t ver);
jk2version_t JK2MF_GetGameVersion();

#endif
