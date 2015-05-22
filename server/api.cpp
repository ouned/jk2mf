/***********************************
		   JK2MF Project

		 made for the lulz
***********************************/

#include "main.h"
#include "api.h"
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>

#define	VMA(x) Imp_VM_ArgPtr(args[x])

qboolean GetPlayerStats(int clientNum, mfPlayer *ply);
uint GetUptimeSeconds();
void GetUptimeString(char *buffer, int bufferSize);
void SwitchBlocking(qboolean status);
void SendConnectionlessPacket(char *msg);
void SendConnectionlessPacketIP(byte *IP, int port, char *msg);
qboolean AddIPBlock(byte *IP);
qboolean RemIPBlock(byte *IP);

EXPORT int SV_GameSystemCalls_Hook(int *args)
{
	switch ( args[0] )
	{
		case JK2MF_GET_PLAYERSTATS:
			return (int)GetPlayerStats(args[1], (mfPlayer*)VMA(2));
		case JK2MF_GET_UPTIMESECS:
			return (int)GetUptimeSeconds();
		case JK2MF_GET_UPTIMESTR:
			GetUptimeString((char*)VMA(1), args[2]);
			return 0;
		case JK2MF_SWITCH_CMD_BLOCKING:
			SwitchBlocking((qboolean)args[1]);
			return 0;
		case JK2MF_SEND_CONNECTIONLESSPACKET:
			SendConnectionlessPacket((char*)VMA(1));
			return 0;
		case JK2MF_GET_DOWNLOADREQUEST_INFO:
			memcpy((mfDownloadInfo*)VMA(1), &lastReq, sizeof(mfDownloadInfo));
			return 0;
		case JK2MF_BLOCK_IP:
			return (int)AddIPBlock((byte*)VMA(1));
		case JK2MF_UNBLOCK_IP:
			return (int)RemIPBlock((byte*)VMA(1));
		case JK2MF_SEND_CONNECTIONLESSPACKET_IP:
			SendConnectionlessPacketIP((byte*)VMA(1), args[2], (char*)VMA(3));
			return 0;
	}

	return Ori_SV_GameSystemCalls(args);
}

qboolean AddIPBlock(byte *IP)
{
	if ( numblockedIPs >= sizeof(blockedIPs) / 4 )
		return qfalse;

	blockedIPs[numblockedIPs][0] = IP[0];
	blockedIPs[numblockedIPs][1] = IP[1];
	blockedIPs[numblockedIPs][2] = IP[2];
	blockedIPs[numblockedIPs][3] = IP[3];
	numblockedIPs++;

	return qtrue;
}

qboolean RemIPBlock(byte *IP)
{
	for ( int i = 0; i < numblockedIPs; i++ )
	{
		if ( blockedIPs[i][0] == IP[0] && blockedIPs[i][1] == IP[1] &&
			 blockedIPs[i][2] == IP[2] && blockedIPs[i][3] == IP[3] )
		{
			blockedIPs[i][0] = 0;
			blockedIPs[i][1] = 0;
			blockedIPs[i][2] = 0;
			blockedIPs[i][3] = 0;

			return qtrue;
		}
	}

	return qfalse;
}

qboolean GetPlayerStats(int clientNum, mfPlayer *reqPly)
{
	if ( clientNum < 0 || clientNum > sv_maxclients->integer )
		return qfalse;

	if ( clients[clientNum].clientState == BOT )
		return qfalse;

	// set mfCheatStatus
	if ( !jk2mf_anticheat->integer )
	{
		reqPly->cheatStatus = CHEAT_DISABLED;
	}
	else
	{
		if ( clients[clientNum].clientState == GOODCLIENT )
			reqPly->cheatStatus = CHEAT_FREE;
		else if ( clients[clientNum].clientState == SCRIPTER )
			reqPly->cheatStatus = CHEAT_SCRIPTER;
		else if ( clients[clientNum].clientState == NOPLUGIN )
			reqPly->cheatStatus = CHEAT_NOPLUGIN;
		else if ( clients[clientNum].clientState == CODEMODIFICATION )
			reqPly->cheatStatus = CHEAT_CODEMODIFICATION;
		else if ( clients[clientNum].clientState == BROKENCOMMUNICATION )
			reqPly->cheatStatus = CHEAT_BROKENCOMMUNICATION;
		else if ( clients[clientNum].clientState == INVALIDMOD )
			reqPly->cheatStatus = CHEAT_UNKNOWNMOD;
	}

	// set IP
	uint tmpIP[4];
	sscanf(clients[clientNum].IPSave, "%u.%u.%u.%u", &tmpIP[0], &tmpIP[1], &tmpIP[2], &tmpIP[3]);
	reqPly->IP[0] = (byte)tmpIP[0];
	reqPly->IP[1] = (byte)tmpIP[1];
	reqPly->IP[2] = (byte)tmpIP[2];
	reqPly->IP[3] = (byte)tmpIP[3];

	// set OS
	if ( !strcmp(clients[clientNum].OS, "win") )
		reqPly->clientOS = OS_WINDOWS;
	else if ( !strcmp(clients[clientNum].OS, "mac") )
		reqPly->clientOS = OS_MAC;
	else
		reqPly->clientOS = OS_UNKNOWN;

	return qtrue;
}

uint GetUptimeSeconds()
{
	time_t now;
	time(&now);

	return (uint)now - (uint)startTime;
}

void GetUptimeString(char *buffer, int bufferSize)
{
	PrintTime(buffer, bufferSize, GetUptimeSeconds());
}

void SwitchBlocking(qboolean status)
{
	if ( status )
		blockCMDmod = true;
	else
		blockCMDmod = false;
}

void SendConnectionlessPacket(char *msg)
{
	Imp_NET_OutOfBandPrint(NS_SERVER, lastAdr, msg);
}

void SendConnectionlessPacketIP(byte *IP, int port, char *msg)
{
	netadr_t adr;
	adr.ip[0] = IP[0]; adr.ip[1] = IP[1]; adr.ip[2] = IP[2]; adr.ip[3] = IP[3];
	adr.port = port;
	adr.type = NA_IP;

	Imp_NET_OutOfBandPrint(NS_SERVER, adr, msg);
}
