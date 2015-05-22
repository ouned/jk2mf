/***********************************
         JK2MF API Header

Author:  ouned
Website: http://jk2.ouned.de/
API-Level: 1
***********************************/

// NOTE:
// Include this file in "g_local.h" after "g_public.h" is included.

#ifndef JK2MFAPI_H
#define JK2MFAPI_H

#define JK2MF_APILEVEL 1
#define JK2MFApiAvailable() (trap_Cvar_VariableIntegerValue("jk2mf_apilevel") >= JK2MF_APILEVEL)
#define JK2MFMINVERSION "2.0"

/* API functions */
#define JK2MF_GET_PLAYERSTATS 700
#define JK2MF_GET_UPTIMESECS 701
#define JK2MF_GET_UPTIMESTR 702
#define JK2MF_SWITCH_CMD_BLOCKING 703
#define JK2MF_SEND_CONNECTIONLESSPACKET 704
#define JK2MF_GET_DOWNLOADREQUEST_INFO 705

/* VM Calls */
#define JK2MF_CLIENT_CHEATSTATUS_CHANGED 20
#define JK2MF_CONNECTIONLESS_PACKET 21
#define JK2MF_DOWNLOAD_REQUEST 22

/* Definitions */
typedef unsigned int uint;

typedef enum {
    CHEAT_DISABLED,						// anticheat is disabled
    CHEAT_FREE,							// client is cheatfree
    CHEAT_NOPLUGIN,						// client doesn't run the plugin (can still play if jk2mf_forceplugin is disabled)
    CHEAT_SCRIPTER,						// client used at least once a movement script
    CHEAT_CODEMODIFICATION,				// client uses a malicious executable
    CHEAT_BROKENCOMMUNICATION,			// clients anticheat communication failed
    CHEAT_UNKNOWNMOD,					// client uses an unknown cgame.qvm (not necessarily a cheater)
} mfCheatStatus;

typedef enum {
	OS_WINDOWS,
	OS_MAC,
	OS_UNKNOWN,
} mfClientOS;

typedef struct {
    byte IP[4];							// clients IP
    mfCheatStatus cheatStatus;			// clients cheat status
	mfClientOS clientOS;				// clients operating system
} mfPlayer;

typedef enum {
	REQUEST_UDP,						// client is requesting a pk3 through the quake3 autodownloads (sv_allowdownload)
	REQUEST_HTTP,						// client is requesting a pk3 from the JK2MF HTTP server
} mfDownloadProtocol;

typedef struct {
	int clientNum;						// requesters clientNum (only in REQUEST_UDP)
	byte IP[4];							// requesters IP
	mfDownloadProtocol protocol;		// requesters protocol
	char fileName[500];					// file beeing requested (e.g. "base/map.pk3")
} mfDownloadInfo;

#define CLIENTNUM_UNKNOWN -2

#define DOWNLOAD_REJECT 0
#define DOWNLOAD_PROCEED 1

#ifndef JK2MFINTERNAL
    qboolean trap_JK2MF_GetPlayerStats(int clientNum, mfPlayer *reqPly);
    uint trap_JK2MF_GetUptimeSeconds(void);
    void trap_JK2MF_GetUptimeString(char *buffer, int bufferSize);
	void trap_JK2MF_SwitchCmdBlocking(qboolean enableBlocking);
	void trap_JK2MF_SendConnectionlessPacket(const char *message);
	void trap_JK2MF_GetDownloadRequestInfo(mfDownloadInfo *downInf);
#endif

#endif /* JK2MFAPI_H */
