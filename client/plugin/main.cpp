/***********************************
		   JK2MF Project

		 made for the lulz
***********************************/

#define MAIN
#ifdef WIN32
	#include <windows.h>
	#include <dbghelp.h>
	#include <psapi.h>
	#include "libcurl\curl.h"
#else
    #include <stdlib.h>
    #include <dlfcn.h>
    #include <sys/stat.h>
    #include <sys/select.h>
    #include <string.h>
	#include <curl/curl.h>
#endif
#include <stdio.h>
#include <time.h>
#include "main.h"
#include "common.h"
#include "detours.h"
#include "strings.h"

/* global vars */
static GVER gVer; // binary instance
static bool imBad; // cheater?
static bool systemActive; // don't block scripts if anticheat is disabled on the server
static uint cgameChksum;
static uint currModuleAddr; // only relevant on win32
static uint currModuleSize;

static void *httpMutex;
static char httpMainUrl[512];
static char httpCurrFileUrl[800];
static char httpToFilePath[800];
static char httpToFileName[100];
static char toFileTmp[800];
static FILE *httpFile;
static int	httpResult;
static double httpDownSize;
static double httpDownPos;
static int httpPosLastSec;
static int httpDownSpeed;
static int httpDownNextSet;
static char servInf[1024];

void Init()
{
	SetGameBinaryVersion();

	if ( gVer == NULLBIN ) // This is not a supported jk2 version
		return;

	// Init function addresses
	Imp_CL_GetServerCommand = (qboolean(*)(int))(GetAddr(gVer, CL_GETSERVERCOMMAND));
	Imp_Cmd_TokenizeString = (void(*)(const char*))(GetAddr(gVer, CMD_TOKENIZESTRING));
	Imp_Cmd_Argv = (char*(*)(int))(GetAddr(gVer, CMD_ARGV));
	Imp_CL_AddReliableCommand = (void(*)(const char*))(GetAddr(gVer, CL_ADDRELIABLECOMMAND));
	Imp_CL_SendPureChecksums = (void(*)())(GetAddr(gVer, CL_SENDPURECHECKSUMS));
	Imp_Cvar_Get = (cvar_t*(*)(const char *, const char *, int))(GetAddr(gVer, CVAR_GET));
	Imp_Cvar_Set2 = (cvar_t*(*)(const char*, const char*, qboolean))(GetAddr(gVer, CVAR_SET2));
	Imp_VM_Create = (vm_t*(*)(const char*, int(*)(int*), vmInterpret_t))(GetAddr(gVer, VM_CREATE));
	Imp_FS_ReadFile = (int(*)(const char*, void**))(GetAddr(gVer, FS_READFILE));
	Imp_SCR_DrawStringExt = (void(*)(int, int, float, const char*, float*, qboolean))(GetAddr(gVer, SCR_DRAWSTRINGEXT));
	Imp_CL_InitDownloads = (void(*)())(GetAddr(gVer, CL_INITDOWNLOADS));
	Imp_CL_BeginDownload = (void(*)(const char*, const char*))(GetAddr(gVer, CL_BEGINDOWNLOAD));
	Imp_Con_CheckResize = (void(*)())(GetAddr(gVer, CON_CHECKRESIZE));
	Imp_CL_NextDownload = (void(*)())(GetAddr(gVer, CL_NEXTDOWNLOAD));
	Imp_CL_KeyEvent = (void(*)(int, qboolean, uint))(GetAddr(gVer, CL_KEYEVENT));
	Imp_Cbuf_InsertText = (void(*)(const char*))(GetAddr(gVer, CBUF_INSERTTEXT));
	Imp_CL_ServerStatus = (int(*)(char*, char*, int))(GetAddr(gVer, CL_SERVERSTATUS));
	Imp_Com_Printf = (void(*)(const char *, ...))(GetAddr(gVer, COM_PRINTF));

	// Init Detours
	Ori_CL_GetServerCommand = (CL_GetServerCommand_Def*)Detour((void*)Imp_CL_GetServerCommand, (void*)CL_GetServerCommand_Hook, Gethl(gVer, CL_GETSERVERCOMMAND));
	Ori_CL_SendPureChecksums = (CL_SendPureChecksums_Def*)Detour((void*)Imp_CL_SendPureChecksums, (void*)CL_SendPureChecksums_Hook, Gethl(gVer, CL_SENDPURECHECKSUMS));
	Ori_Cvar_Set2 = (Cvar_Set2_Def*)Detour((void*)Imp_Cvar_Set2, (void*)Cvar_Set2_Hook, Gethl(gVer, CVAR_SET2));
	Ori_VM_Create = (VM_Create_Def*)Detour((void*)Imp_VM_Create, (void*)VM_Create_Hook, Gethl(gVer, VM_CREATE));
	Ori_FS_ReadFile = (FS_ReadFile_Def*)Detour((void*)Imp_FS_ReadFile, (void*)FS_ReadFile_Hook, Gethl(gVer, FS_READFILE));
	Ori_CL_InitDownloads = (CL_InitDownloads_Def*)Detour((void*)Imp_CL_InitDownloads, (void*)CL_InitDownloads_Hook, Gethl(gVer, CL_INITDOWNLOADS));
	Ori_CL_BeginDownload = (CL_BeginDownload_Def*)Detour((void*)Imp_CL_BeginDownload, (void*)CL_BeginDownload_Hook, Gethl(gVer, CL_BEGINDOWNLOAD));
	Ori_Con_CheckResize = (Con_CheckResize_Def*)Detour((void*)Imp_Con_CheckResize, (void*)Con_CheckResize_Hook, Gethl(gVer, CON_CHECKRESIZE));
	Ori_CL_KeyEvent = (CL_KeyEvent_Def*)Detour((void*)Imp_CL_KeyEvent, (void*)CL_KeyEvent_Hook, Gethl(gVer, CL_KEYEVENT));
	Ori_Cbuf_InsertText = (Cbuf_InsertText_Def*)Detour((void*)Imp_Cbuf_InsertText, (void*)Cbuf_InsertText_Hook, Gethl(gVer, CBUF_INSERTTEXT));

	// Init strings
	InitStrings();

	// Check Modules
	CheckModules();

	// Init vars
	if ( gVer == ASPYRMAC )
	{
		clcLastExecutedServerCommand = (int*)(*(uint*)(GetAddr(gVer, CLC_LASTEXECUTEDSVCMD)) + 0x20440);
		clcServerCommands = (char*)(*(uint*)(GetAddr(gVer, CLC_SERVERCOMMANDS)) + 0x20444);
		keyCatchup = (int*)(*(uint*)(GetAddr(gVer, KEY_CATCHUP)) + 4);
	}
	else
	{
		clcLastExecutedServerCommand = (int*)(GetAddr(gVer, CLC_LASTEXECUTEDSVCMD));
		clcServerCommands = (char*)(GetAddr(gVer, CLC_SERVERCOMMANDS));
		keyCatchup = (int*)(GetAddr(gVer, KEY_CATCHUP));
	}
	servAddr = (netadr_t*)(GetAddr(gVer, CURR_SERVERADDR));
	clsState = (int*)(GetAddr(gVer, CLS_STATE));
	clsRealtime = (int*)(GetAddr(gVer, CLS_REALTIME));
	drw = (refexport_t*)(GetAddr(gVer, PTR_REFEXPORT));
	clsWhiteShader = (qhandle_t*)(GetAddr(gVer, CLS_WHITESHADER));
	keys = (qkey_t*)(GetAddr(gVer, Q_KEYS));

	// To be absolutely sure.
	systemActive = true;

	// Init Cvars
	cl_maxpackets = Imp_Cvar_Get(GetStr(STR_CL_MAXPACKETS), GetStr(STR_50), CVAR_ARCHIVE);
	com_maxfps = Imp_Cvar_Get(GetStr(STR_COM_MAXFPS), GetStr(STR_85), CVAR_ARCHIVE);
	snaps = Imp_Cvar_Get(GetStr(STR_SNAPS), GetStr(STR_30), CVAR_ARCHIVE);
	rate = Imp_Cvar_Get(GetStr(STR_RATE), GetStr(STR_5000), CVAR_ARCHIVE);
	com_hunkmegs = Imp_Cvar_Get(GetStr(STR_HUNKMEGS), GetStr(STR_128), CVAR_ARCHIVE);

	fs_homepath = Imp_Cvar_Get("fs_homepath", "", 0);

	cl_allowhttpdownload = Imp_Cvar_Get("cl_allowhttpdownload", "1", CVAR_ARCHIVE);
	cl_allowdownload = Imp_Cvar_Get("cl_allowdownload", "0", 0);


	if ( com_hunkmegs->integer < 128 || com_hunkmegs->integer > 256 )
		Ori_Cvar_Set2(GetStr(STR_HUNKMEGS), GetStr(STR_128), qtrue);
}

// ==============
// Copy original memory back
// ==============
void Shutdown()
{
	if ( gVer == NULLBIN )
		return;

	// End all HTTP Downloads
	if ( httpMutex )
	{
		LockMutex(httpMutex);
		httpResult = DOWNENDNOW;
		UnlockMutex(httpMutex);

		while (1)
		{
			LockMutex(httpMutex);
			if ( httpResult == DOWNFAIL )
			{
				UnlockMutex(httpMutex);
				DeleteMutex(&httpMutex);
				break;
			}
			UnlockMutex(httpMutex);

            #ifdef WIN32
                Sleep(10);
            #else
                usleep(10000);
            #endif
		}
	}

	FreeStrings();

	// Undetour everything
	memcpy((void*)Imp_CL_GetServerCommand, (void*)Ori_CL_GetServerCommand, Gethl(gVer, CL_GETSERVERCOMMAND));
	free((void*)Ori_CL_GetServerCommand);
	memcpy((void*)Imp_CL_SendPureChecksums, (void*)Ori_CL_SendPureChecksums, Gethl(gVer, CL_SENDPURECHECKSUMS));
	free((void*)Ori_CL_SendPureChecksums);
	memcpy((void*)Imp_Cvar_Set2, (void*)Ori_Cvar_Set2, Gethl(gVer, CVAR_SET2));
	free((void*)Ori_Cvar_Set2);
	memcpy((void*)Imp_VM_Create, (void*)Ori_VM_Create, Gethl(gVer, VM_CREATE));
	free((void*)Ori_VM_Create);
	memcpy((void*)Imp_FS_ReadFile, (void*)Ori_FS_ReadFile, Gethl(gVer, FS_READFILE));
	free((void*)Ori_FS_ReadFile);
	memcpy((void*)Imp_CL_InitDownloads, (void*)Ori_CL_InitDownloads, Gethl(gVer, CL_INITDOWNLOADS));
	free((void*)Ori_CL_InitDownloads);
	memcpy((void*)Imp_Con_CheckResize, (void*)Ori_Con_CheckResize, Gethl(gVer, CON_CHECKRESIZE));
	free((void*)Ori_Con_CheckResize);
	memcpy((void*)Imp_CL_BeginDownload, (void*)Ori_CL_BeginDownload, Gethl(gVer, CL_BEGINDOWNLOAD));
	free((void*)Ori_CL_BeginDownload);
	memcpy((void*)Imp_CL_KeyEvent, (void*)Ori_CL_KeyEvent, Gethl(gVer, CL_KEYEVENT));
	free((void*)Ori_CL_KeyEvent);
	memcpy((void*)Imp_Cbuf_InsertText, (void*)Ori_Cbuf_InsertText, Gethl(gVer, CBUF_INSERTTEXT));
	free((void*)Ori_Cbuf_InsertText);

	// Set current Binary version to NULLBIN
	gVer = NULLBIN;
}

// ==============
// Get the Binary version this instance is running with so we can get the correct addresses
// ==============
void SetGameBinaryVersion()
{
	gVer = NULLBIN;

	#ifdef WIN32
		char *jkMPExe = (char*)(0x04f66a0); // 0x04f66a0 jk2mp.exe -> JK2MP: v1.04
		char *jkMPExe_1_02 = (char*)(0x04EF690); // 0x04EF690 jk2mp.exe -> JK2MP: v1.02a

		if (strstr(jkMPExe, "JK2MP: v1.04"))
			gVer = JK2MP104EXE;
		else if ( strstr(jkMPExe_1_02, "JK2MP: v1.02") )
			gVer = JK2MP102EXE;
	#else
		char *jkAspyrMac = (char*)(0xfe7e8); // 0xfe7e8 Jedi Knight II -> JK2MP: v1.04
        if ( strstr(jkAspyrMac, "JK2MP: v1.5") )
            gVer = ASPYRMAC;
    #endif
}

// ==============
// CL_SendPureChecksums
//
// Send pluginversion to the server
// ==============
void CL_SendPureChecksums_Hook()
{
	Ori_CL_SendPureChecksums();
	Imp_CL_AddReliableCommand(va(GetStr(STR_MFSRVCMD), GetStr(STR_VERSION), GetStr(STR_OS)));
}

// ==============
// CL_GetServerCommand
//
// Receiving commands from the server
// The Server will freeze us if we don't answere for a long time
// ==============
qboolean CL_GetServerCommand_Hook(int serverCommandNumber)
{
	char *s = (char*)((uint)clcServerCommands + (uint)1024 * (uint)(serverCommandNumber & 127));

	// is this something jk2mf related?
	if ( !Q_strncmp(s, GetStr(STR_MFCLCMD), 8) )
	{
		*clcLastExecutedServerCommand = serverCommandNumber;
		Imp_Cmd_TokenizeString(s);

		MFSrvCommand();
		return qfalse;
	}
	
	return Ori_CL_GetServerCommand(serverCommandNumber);
}

void MFSrvCommand()
{
	char arg1[MAX_TOKEN_CHARS];
	char arg2[MAX_TOKEN_CHARS];
	Q_strncpyz(arg1, Imp_Cmd_Argv(1), sizeof(arg1));
	Q_strncpyz(arg2, Imp_Cmd_Argv(2), sizeof(arg2));

	if ( !strcmp(arg1, GetStr(STR_CHKSUM)) )
	{
		// Check Modules
		CheckModules();

		// Send Checksum
		uint BinChkSum = GetClientsideChecksum() + GetBinaryChecksum();

		srand((uint)time(NULL));
		char BinChkSumStr[200], cgameChksumStr[200];
		EncryptMessage(va("%u", BinChkSum), BinChkSumStr, sizeof(BinChkSumStr), rand() % 0xFFFFFFF + 0x01);
		EncryptMessage(va("%u", cgameChksum), cgameChksumStr, sizeof(cgameChksumStr), rand() % 0xFFFFFFF + 0x01);

		Imp_CL_AddReliableCommand(va(GetStr(STR_CLSENDCHK), BinChkSumStr, cgameChksumStr));
		
		// Check Cvars
		if ( cl_maxpackets->integer < 50 || cl_maxpackets->integer > 150 )
			Ori_Cvar_Set2(GetStr(STR_CL_MAXPACKETS), GetStr(STR_50), qtrue);
		if ( com_maxfps->integer < 85 )
			Ori_Cvar_Set2(GetStr(STR_COM_MAXFPS), GetStr(STR_85), qtrue);
		if ( snaps->integer < 30 || snaps->integer > 100 )
			Ori_Cvar_Set2(GetStr(STR_SNAPS), GetStr(STR_30), qtrue);
		if ( rate->integer < 5000 || rate->integer > 110000 )
			Ori_Cvar_Set2(GetStr(STR_RATE), GetStr(STR_5000), qtrue);
	}
	else if ( !strcmp(arg1, GetStr(STR_STATE)) )
	{
		if ( atoi(arg2) )
			systemActive = true;
		else
			systemActive = false;
	}
}

/* ==================================== WIN32 JK2MP.EXE 1.04 ==================================== */
uint Win104InfoTab[] = {
    /* FROM      TO */
    0x401000, 0x4E9BC2
};

/* Exclude Table (Code Section) */
HashTable Win104ExclTab[] = {
	/* EXCLADDR  LEN  */
	{  0x401470,  6   }, // CL_GETSERVERCOMMAND HOOK
	{  0x405E40,  5   }, // CON_CHECKRESIZE HOOK
	{  0x409C50,  6   }, // CL_KEYEVENT HOOK
	{  0x40A178,  1   }, // STEAM VERSION
	{  0x40AC10,  5   }, // CL_DISCONNECT HOOK & STEAM VERSION
	{  0x40B184,  1   }, // NOCD CRACK JK2MP.TK
	{  0x40B4D0,  6   }, // CL_SENDPURECHECKSUMS HOOK
	{  0x40B790,  5   }, // CL_BEGINDOWNLOADS HOOK
	{  0x40B8D0,  5   }, // CL_INITDOWNLOADS HOOK
	{  0x40FCF0,  5   }, // SCR_DRAWSCREENFIELD HOOK & STEAM VERSION
	{  0x427CF0,  6   }, // CBUF_INSERTTEXT HOOK
	{  0x42B3D0,  5	  }, // CVAR_SET2 HOOK
	{  0x42D660,  5	  }, // FS_READFILE HOOK
	{  0x43EBA0,  8	  }, // VM_CREATE HOOK
	{  0x4427E8,  1   }, // NOCD CRACK EVC
	{  0x442E00,  143 }, // NOCD START-MP-104 & STEAM VERSION
	{  0x444F40,  5   }, // NET_INIT HOOK
	{  0x44D85D,  1   }, // NOCD CRACK JK2MP.TK
	{  0x488F32,  6   }, // RE_REGISTERMODEL SPAMFIX
};

/* Include Table (Section Independent) */
HashTable Win104InclTab[] = {
	/* INCLADDR   LEN       */
	{  0x4F65AC,  0x618     }, // STRINGS
	{  0x4F83D8,  0x137E    }, // STRINGS
	{  0x4F9C78,  0xD9C     }, // STRINGS
	{  0x4FAA28,  0x818     }, // STRINGS
	{  0x4FB37C,  0x1D8     }, // STRINGS
	{  0x4FB564,  0xE93     }, // STRINGS
	{  0x4FC4A0,  0x8E8     }, // STRINGS
	{  0x4FDAC8,  0xD54     }, // STRINGS
	{  0x4FF13C,  0x344     }, // STRINGS
	{  0x4FF530,  0x310     }, // STRINGS
	{  0x500990,  0x9B8     }, // STRINGS
	{  0x501418,  0x518     }, // STRINGS
	{  0x501B0C,  0xFC1     }, // STRINGS
	{  0x502BC8,  0xA14     }, // STRINGS
	{  0x5035E8,  0x1340    }, // STRINGS
	{  0x50493C,  0x244     }, // STRINGS
	{  0x504BD8,  0x1F36    }, // STRINGS
	{  0x506D64,  0x2A9     }, // STRINGS
	{  0x507EDC,  0xC3F     }, // STRINGS
	{  0x508B34,  0x190     }, // STRINGS
	{  0x50B91F,  0x3E8D    }, // STRINGS
};
/* ============================================================================================== */

/* ==================================== WIN32 JK2MP.EXE 1.02 ==================================== */
uint Win102InfoTab[] = {
	/* FROM      TO */
	0x401000, 0x4E3C22
};

/* Exclude Table (Code Section) */
HashTable Win102ExclTab[] = {
	/* EXCLADDR  LEN  */
	{ 0x401470,   6 }, // CL_GETSERVERCOMMAND HOOK
	{ 0x405D70,   5 }, // CON_CHECKRESIZE HOOK
	{ 0x409690,   6 }, // CL_KEYEVENT HOOK
	{ 0x40A660,   5 }, // CL_DISCONNECT HOOK
	{ 0x40AF10,   6 }, // CL_SENDPURECHECKSUMS HOOK
	{ 0x40B1D0,   5 }, // CL_BEGINDOWNLOADS HOOK
	{ 0x40B310,   5 }, // CL_INITDOWNLOADS HOOK
	{ 0x40F5B0,   5 }, // SCR_DRAWSCREENFIELD HOOK
	{ 0x4273F0,   6 }, // CBUF_INSERTTEXT HOOK
	{ 0x42AAB0,   5 }, // CVAR_SET2 HOOK
	{ 0x42CCF0,   5 }, // FS_READFILE HOOK
	{ 0x43E270,   8 }, // VM_CREATE HOOK
	{ 0x441E18,   1 }, // NOCD CRACK EVC
	{ 0x442420,   4 }, // NOCD CRACK EVC
	{ 0x4445B0,   5 }, // NET_INIT HOOK
	{ 0x444C2E,   2 }, // EVC CPU CRASHFIX
	{ 0x444CA0,   2 }, // EVC CPU CRASHFIX
	{ 0x444CAC,   2 }, // EVC CPU CRASHFIX
	{ 0x444CBF,   2 }, // EVC CPU CRASHFIX
};

/* Include Table (Section Independent) */
HashTable Win102InclTab[] = {
	/* INCLADDR   LEN       */
	{ 0x4EF5AC, 0x5FC  }, // STRINGS
	{ 0x4F1004, 0x13BE }, // STRINGS
	{ 0x4F28E0, 0xD8C  }, // STRINGS
	{ 0x4F3680, 0x818  }, // STRINGS
	{ 0x4F3FD4, 0x1D8  }, // STRINGS
	{ 0x4F41BC, 0xE93  }, // STRINGS
	{ 0x4F50F8, 0x8F8  }, // STRINGS
	{ 0x4F6710, 0xD48  }, // STRINGS
	{ 0x4F7D7C, 0x344  }, // STRINGS
	{ 0x4F8170, 0x2D8  }, // STRINGS
	{ 0x4F9598, 0x9B8  }, // STRINGS
	{ 0x4FA008, 0x530  }, // STRINGS
	{ 0x4FA714, 0x110C }, // STRINGS
	{ 0x4FBAA0, 0x1B18 }, // STRINGS
	{ 0x4FD5CC, 0x244  }, // STRINGS
	{ 0x4FD868, 0x1E6E }, // STRINGS
	{ 0x4FF92C, 0x2A9  }, // STRINGS
	{ 0x500A84, 0xC18  }, // STRINGS
	{ 0x5016CC, 0x190  }, // STRINGS
	{ 0x5044B7, 0x4056 }, // STRINGS
};
/* ============================================================================================== */

/* =========================================== ASPYR MAC ======================================== */
uint AspyrInfoTab[] = {
	/* FROM      TO */
	0x002678, 0x0FA88C
};

/* Exclude Table (Code Section) */
HashTable AspyrExclTab[] = {
	/* EXCLADDR LEN */
	{ 0x004762, 5  }, // CL_GETSERVERCOMMAND HOOK
	{ 0x00A39D, 5  }, // CON_CHECKRESIZE HOOK
	{ 0x00E1BC, 5  }, // CL_KEYEVENT HOOK
	{ 0x0107EA, 5  }, // CL_BEGINDOWNLOAD HOOK
	{ 0x010996, 9  }, // CL_SENDPURECHECKSUMS HOOK
	{ 0x01158C, 10 }, // CL_INITDOWNLOADS HOOK
	{ 0x01345A, 6  }, // CL_DISCONNECT HOOK
	{ 0x014035, 7  }, // CL_CONNECT HOOK
	{ 0x015E9B, 6  }, // SCR_DRAWSCREENFIELD HOOK
	{ 0x027F87, 7  }, // COM_ERROR HOOK
	{ 0x029381, 7  }, // COM_INIT HOOK
	{ 0x02A107, 5  }, // CVAR_SET2 HOOK
	{ 0x0D068F, 5  }, // VM_CREATE HOOK
	{ 0x0D834E, 5  }, // FS_READFILE HOOK
	{ 0x0DA62C, 5  }, // CBUF_EXECUTE HOOK
	{ 0x0DA728, 5  }, // CBUF_INSERTTEXT HOOK
};

/* Include Table (Section Independent) */
HashTable AspyrInclTab[] = {
	/* INCLADDR  LEN  */
	{ 0x0FADC0, 0x485 }, // STRINGS
	{ 0x0FE832, 0x2D  }, // STRINGS
	{ 0x0FC276, 0x29  }, // STRINGS
	{ 0x1096D3, 0x3E  }, // STRINGS
	{ 0x10C694, 0x3B  }, // STRINGS
	{ 0x0FA91F, 0x2D  }, // STRINGS
	{ 0x10B580, 0x30  }, // STRINGS
	{ 0x0FEC58, 0x30  }, // STRINGS
};
/* ============================================================================================== */

// ==============
// GetBinaryChecksum
//
// Get the hash of jk2mp.exe's memory.
// Excluding all sections expect .text
// Also, some are needed for nocd cracks and hooks -> allow them
// ==============
uint GetBinaryChecksum()
{
	char hashes[4096];
	memset(hashes, 0, sizeof(hashes));

	if ( imBad )
		return -1;

	uint currPos = 0;
	int lenghtOfStruct = 0;
	if (gVer == JK2MP104EXE)
	{
		currPos = Win104InfoTab[0];
		lenghtOfStruct = sizeof(Win104ExclTab) / sizeof(HashTable);
	}
	else if (gVer == JK2MP102EXE)
	{
		currPos = Win102InfoTab[0];
		lenghtOfStruct = sizeof(Win102ExclTab) / sizeof(HashTable);
	}
	else if ( gVer == ASPYRMAC )
	{
		currPos = AspyrInfoTab[0];
		lenghtOfStruct = sizeof(AspyrExclTab) / sizeof(HashTable);
	}

	for ( int i = 0; i < lenghtOfStruct + 1; i++ )
	{
		uint h = 0;

		if ( i == lenghtOfStruct )
		{
			if ( gVer == JK2MP104EXE )
				h = hash((void*)currPos, Win104InfoTab[1] - currPos, 3422);
			else if (gVer == JK2MP102EXE)
				h = hash((void*)currPos, Win102InfoTab[1] - currPos, 3422);
			else if ( gVer == ASPYRMAC )
				h = hash((void*)currPos, AspyrInfoTab[1] - currPos, 3422);
		}
		else
		{
			if ( gVer == JK2MP104EXE )
			{
				h = hash((void*)currPos, Win104ExclTab[i].addr - currPos, 43259);
				currPos = Win104ExclTab[i].addr + Win104ExclTab[i].lenght;
			}
			if (gVer == JK2MP102EXE)
			{
				h = hash((void*)currPos, Win102ExclTab[i].addr - currPos, 43259);
				currPos = Win102ExclTab[i].addr + Win102ExclTab[i].lenght;
			}
			else if ( gVer == ASPYRMAC )
			{
				h = hash((void*)currPos, AspyrExclTab[i].addr - currPos, 43259);
				currPos = AspyrExclTab[i].addr + AspyrExclTab[i].lenght;
			}
		}

		Q_strcat(hashes, sizeof(hashes), va(GetStr(STR_INSUINT), h));
	}

	if ( gVer == JK2MP104EXE )
		lenghtOfStruct = sizeof(Win104InclTab) / sizeof(HashTable);
	else if (gVer == JK2MP102EXE)
		lenghtOfStruct = sizeof(Win102InclTab) / sizeof(HashTable);
	else if ( gVer == ASPYRMAC )
		lenghtOfStruct = sizeof(AspyrInclTab) / sizeof(HashTable);

	for ( int i = 0; i < lenghtOfStruct; i++ )
	{
		uint h = 0;

		if ( gVer == JK2MP104EXE )
			h = hash((void*)Win104InclTab[i].addr, Win104InclTab[i].lenght, 145892);
		if (gVer == JK2MP102EXE)
			h = hash((void*)Win102InclTab[i].addr, Win102InclTab[i].lenght, 145892);
		else if ( gVer == ASPYRMAC )
			h = hash((void*)AspyrInclTab[i].addr, AspyrInclTab[i].lenght, 145892);

		Q_strcat(hashes, sizeof(hashes), va(GetStr(STR_INSUINT), h));
	}

	uint finHash = hash(hashes, strlen(hashes), 1238);
	return finHash;
}

char scriptIdents[][50] = {"+forward", "-forward", "+moveup", "-moveup", "+movedown", "-movedown", "+moveleft", "-moveleft", "+moveright", "-moveright", "+attack", "-attack", "+lookdown", "-lookdown", "+left", "-left", "+right", "-right"};


uint GetClientsideChecksum()
{
	/*static uint codeSectionLenght = 0x34C00; // DON'T FORGET TO CHANGE ON RELEASE

	uint codeHash = 0;
	if ( gVer == JK2MP104EXE )
		codeHash = hash((void*)(currModuleAddr+0x1000), codeSectionLenght, 114584);
	*/

	uint str1Hash = hash(scriptIdents, sizeof(scriptIdents), 93175);
	uint str2Hash = GetStrChksum();

	return /*codeHash + */str1Hash + str2Hash;
}

void CL_KeyEvent_Hook(int key, qboolean down, unsigned time)
{
	if ( IsIllegalScript(keys[key].binding, key) && systemActive )
	{
		if ( *keyCatchup == 0 )
			Imp_CL_AddReliableCommand(GetStr(STR_SCRIPTER));

		return;
	}

	Ori_CL_KeyEvent(key, down, time);
}

void Cbuf_InsertText_Hook(const char *text)
{
	if ( IsIllegalScript((char*)text, 0) && systemActive )
	{
		Imp_CL_AddReliableCommand(GetStr(STR_SCRIPTER));
		return;
	}

	Ori_Cbuf_InsertText(text);
}

bool IsIllegalScript(char *cmdLine, int key)
{
	static char realLine[100000];
	int quotes = 0, curSize = 0;
	int cmds = 0;
	int waits = 0;

	if ( !cmdLine )
		return false;

	// filter out all text in quotes
	for ( int i = 0; i < (int)strlen(cmdLine); i++ )
	{
		if (cmdLine[i] == '"')
			quotes++;

		if ( !(quotes&1) && cmdLine[i] != '"' )
		{
			realLine[curSize] = cmdLine[i];
			curSize++;
		}
	}
	realLine[curSize] = 0;

	// count movement commands
	char *end = (char*)realLine + strlen(realLine);
	char *currPos = realLine;
	for ( int i = 0; i < sizeof(scriptIdents) / 50; i++ )
	{
		while ( currPos < realLine + strlen(realLine) )
		{
			char *foundpos = strstr(currPos, scriptIdents[i]);
			if ( foundpos )
			{
				char *searchPos = foundpos;
				while ( *searchPos != '\n' && *searchPos != ';' && *searchPos != '\r' && searchPos != realLine - 1 )
					searchPos--;

				searchPos++;

				while ( ( *searchPos == ' ' || *searchPos == '\t' ) && searchPos <= end )
					searchPos++;

				if ( Q_strncmp(searchPos, GetStr(STR_BIND), 5) && Q_strncmp(searchPos, GetStr(STR_SET), 4) && Q_strncmp(searchPos, GetStr(STR_SETA), 5) && Q_strncmp(searchPos, GetStr(STR_SETU), 4) )
					cmds++;

				currPos = foundpos + 1;
			}
			else
				break;
		}
	}

	// count waits
	currPos = realLine;
	while ( currPos < realLine + strlen(realLine) )
	{
		char *foundpos = strstr(currPos, GetStr(STR_WAIT));
		if ( foundpos )
		{
			currPos = foundpos + 1;
			waits++;
		}
		else
			break;
	}
    
	// Mousewheel & cmd is a script
	if (gVer != JK2MP102EXE && (key == KEY_MWHEELUP || (key == KEY_MWHEELDOWN && (gVer == JK2MP104EXE || gVer == JK2MP102EXE))) && cmds >= 1)
		return true;
	
	// A "wait" and one or more cmds is a script for sure
	if ( waits >= 1 && cmds >= 1 )
  		return true;

	// One or more cmds and one of these two special cases is a script
	if ( cmds >= 1 && ( strstr(cmdLine, GetStr(STR_VSTR)) || strstr(cmdLine, GetStr(STR_EXEC)) ) )
		return true;

	return false;
}

cvar_t *Cvar_Set2_Hook(const char *var_name, const char *value, qboolean force)
{
	CheckHooks(gVer, currModuleAddr, currModuleSize, &imBad);

	if ( !Q_stricmp(var_name, GetStr(STR_CL_MAXPACKETS)) )
	{
		int intVal = atoi(value);

		if ( intVal < 50 || intVal > 150 )
			return Ori_Cvar_Set2(var_name, GetStr(STR_50), qtrue);
	}
	else if ( !Q_stricmp(var_name, GetStr(STR_COM_MAXFPS)) )
	{
		int intVal = atoi(value);

		if ( intVal < 85 )
			return Ori_Cvar_Set2(var_name, GetStr(STR_85), qtrue);
	}
	else if ( !Q_stricmp(var_name, GetStr(STR_SNAPS)) )
	{
		int intVal = atoi(value);

		if ( intVal < 30 || intVal > 100 )
			return Ori_Cvar_Set2(var_name, GetStr(STR_30), qtrue);
	}
	else if ( !Q_stricmp(var_name, GetStr(STR_RATE)) )
	{
		int intVal = atoi(value);

		if ( intVal < 5000 || intVal > 110000 )
			return Ori_Cvar_Set2(var_name, GetStr(STR_5000), qtrue);
	}
	
	return Ori_Cvar_Set2(var_name, value, force);
}

vm_t *VM_Create_Hook(const char *module, int (*systemCalls)(int *), vmInterpret_t interpret)
{
	vm_t *res = Ori_VM_Create(module, systemCalls, interpret);

	/*if ( res != NULL )
	{
		if ( interpret == VMI_COMPILED )
		{
			char filename[MAX_QPATH];
			char *buffer;

			//sprintf(filename, "vm/%s.qvm", vm->name);
			//int length = Imp_FS_ReadFile(filename, (void **)&buffer);
		}
	}*/

	return res;
}

int FS_ReadFile_Hook(const char *qpath, void **buffer)
{
	int len = Ori_FS_ReadFile(qpath, buffer);

	if ( !strcmp(qpath, GetStr(STR_CGAMEQVM)) && len > 0 )
	{
		cgameChksum = hash(*buffer, len, 25790);
	}

	return len;
}

void CheckModules()
{
    #ifdef WIN32
		HMODULE hMods[512];
		DWORD cbNeeded;
		int opglLibs = 0;

		if( EnumProcessModules(GetCurrentProcess(), hMods, sizeof(hMods), &cbNeeded))
		{
			for ( uint i = 0; i < (cbNeeded / sizeof(HMODULE)); i++ )
			{
				char szModName[MAX_PATH];
				if ( GetModuleFileNameExA(GetCurrentProcess(), hMods[i], szModName, sizeof(szModName)))
				{
					MODULEINFO modInf;
					GetModuleInformation(GetCurrentProcess(), hMods[i], &modInf, sizeof(modInf));

					// Get name only
					const char *nameOnly;
					for ( int j = strlen(szModName); j >= 0; j-- )
					{
						if ( szModName[j] == '\\' )
						{
							nameOnly = &szModName[j+1];
							break;
						}
						else
							if ( j == 0 )
								nameOnly = szModName;
					}

					// Check for duplicate opengl32.dll's
					if ( !Q_stricmp(nameOnly, GetStr(STR_OPENGL32)))
					{
						opglLibs++;

						if ( opglLibs >= 2 )
							imBad = true;
					}

					// Validate hooks
					if ( strstr(nameOnly, GetStr(STR_JK2MFCL)) )
					{
						MODULEINFO modInf;
						GetModuleInformation(GetCurrentProcess(), hMods[i], &modInf, sizeof(modInf));

						currModuleAddr = (uint)modInf.lpBaseOfDll;
						currModuleSize = modInf.SizeOfImage;
						CheckHooks(gVer, currModuleAddr, currModuleSize, &imBad);
					}

					// KPT Hack detection
					if (gVer == JK2MP104EXE) {
						byte tempcpy[5];
						byte *ptr = (byte*)((uint)modInf.lpBaseOfDll + (uint)0x10C5);
						ReadProcessMemory(GetCurrentProcess(), ptr, tempcpy, sizeof(tempcpy), NULL);
						if (tempcpy[0] == 0xFF && tempcpy[1] == 0x15 && tempcpy[2] == 0x7C && tempcpy[3] == 0xD1)
							imBad = true;
					}
				}
			}
		}
    #endif
}

void CL_InitDownloads_Hook()
{
	httpMainUrl[0] = 0;

	if ( !cl_allowhttpdownload->integer )
	{
		Ori_CL_InitDownloads();
		return;
	}

	char servInf[50];
	char ipStr[50];

	Q_strncpyz(ipStr, va("%u.%u.%u.%u", servAddr->ip[0], servAddr->ip[1], servAddr->ip[2], servAddr->ip[3]), sizeof(ipStr));
	int sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	sockaddr_in peer;
	peer.sin_addr.s_addr = inet_addr(ipStr);
	peer.sin_port    	= servAddr->port;
	peer.sin_family  	= AF_INET;
	memset(servInf, 0, sizeof(servInf));
	send_recv(sd, PORTREQ, sizeof(PORTREQ), servInf, sizeof(servInf), &peer);

	char *portPtr = servInf+4;
	if ( !strcmp(portPtr, "disabled") || *portPtr == 0 )
	{
		Ori_CL_InitDownloads();
		return;
	}

	if ( strstr(portPtr, "http://") )
		Q_strncpyz(httpMainUrl, portPtr, sizeof(httpMainUrl));
	else
		Q_strncpyz(httpMainUrl, va("http://%s:%i", ipStr, atoi(portPtr)), sizeof(httpMainUrl));

	if ( !cl_allowdownload->integer )
	{
		cl_allowdownload->integer = 1;
		Ori_CL_InitDownloads();
		cl_allowdownload->integer = 0;
	}
	else
	{
		Ori_CL_InitDownloads();
	}
}

void CL_BeginDownload_Hook(const char *localName, const char *remoteName)
{
	if ( !httpMainUrl[0] )
	{
		Ori_CL_BeginDownload(localName, remoteName);
		return;
	}

	httpMutex = InitMutex();
	LockMutex(httpMutex);
	httpResult = DOWNPROC;
	Q_strncpyz(httpCurrFileUrl, va("%s/%s", httpMainUrl, remoteName), sizeof(httpCurrFileUrl));
	Q_strncpyz(httpToFilePath, va("%s/%s", fs_homepath->string, localName), sizeof(httpToFilePath));
	Q_strncpyz(httpToFileName, localName, sizeof(httpToFileName));
	httpDownSize = 0;
	httpDownPos = 0;
	httpPosLastSec = 0;
	httpDownSpeed = 0;
	httpDownNextSet = 0;
	
	Imp_Com_Printf("^7JK2MF^1: ^7Downloading file %s to %s\n", httpCurrFileUrl, httpToFilePath);
	
	UnlockMutex(httpMutex);

	StartDownloadThread((void*)DownloadThread);
}

void Con_CheckResize_Hook()
{
	Ori_Con_CheckResize();

	if ( *clsState == CA_LOADING || *clsState == CA_PRIMED )
	{
		if ( !servInf[0] )
		{
			char ipStr[50];
			Q_strncpyz(ipStr, va("%u.%u.%u.%u", servAddr->ip[0], servAddr->ip[1], servAddr->ip[2], servAddr->ip[3]), sizeof(ipStr));

			int sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			sockaddr_in peer;
			peer.sin_addr.s_addr = inet_addr(ipStr);
			peer.sin_port    	= servAddr->port;
			peer.sin_family  	= AF_INET;
			send_recv(sd, INFREQ, sizeof(INFREQ), servInf, sizeof(servInf), &peer);
		}

		char strbuf[800];
		Q_strncpyz(strbuf, va("^1[ ^7JK2MF %s ^1]", VERSION), sizeof(strbuf));
		Imp_SCR_DrawStringExt(470, 5, 8.5, strbuf, stdcol, qfalse);

		if ( strlen(servInf) >= 20 )
		{
			if ( !strcmp(Info_ValueForKey(servInf+19, "jk2mf_anticheat"), "1") && !strcmp(Info_ValueForKey(servInf+19, "jk2mf_forceplugin"), "1") )
			{
				Q_strncpyz(strbuf, va("Anticheat: ^2On", VERSION), sizeof(strbuf));
				Imp_SCR_DrawStringExt(470, 22, 8.5, strbuf, stdcol, qfalse);
			}
			else if ( !strcmp(Info_ValueForKey(servInf+19, "jk2mf_anticheat"), "1") && !strcmp(Info_ValueForKey(servInf+19, "jk2mf_forceplugin"), "0") )
			{
				Q_strncpyz(strbuf, va("Anticheat: ^3Optional", VERSION), sizeof(strbuf));
				Imp_SCR_DrawStringExt(470, 22, 8.5, strbuf, stdcol, qfalse);
			}
			else
			{
				Q_strncpyz(strbuf, va("Anticheat: ^1Off", VERSION), sizeof(strbuf));
				Imp_SCR_DrawStringExt(470, 22, 8.5, strbuf, stdcol, qfalse);
			}

			if ( !strcmp(Info_ValueForKey(servInf+19, "jk2mf_httpdownloads"), "1") )
			{
				Q_strncpyz(strbuf, va("HTTP-Downloads: ^2On", VERSION), sizeof(strbuf));
				Imp_SCR_DrawStringExt(470, 34, 8.5, strbuf, stdcol, qfalse);
			}
			else
			{
				Q_strncpyz(strbuf, va("HTTP-Downloads: ^1Off", VERSION), sizeof(strbuf));
				Imp_SCR_DrawStringExt(470, 34, 8.5, strbuf, stdcol, qfalse);
			}
		}
	}
	else
		servInf[0] = 0;


	if ( !httpMutex )
		return;

	LockMutex(httpMutex);

	if ( httpResult == DOWNPROC )
	{
		drw->SetColor(blackcol);
		drw->DrawStretchPic(0, 0, 640, 480, 0, 0, 0, 0, *clsWhiteShader);
		drw->SetColor(NULL);

		Imp_SCR_DrawStringExt(0, 0, 6.0, va("JK2MF %s", VERSION), stdcol, qfalse);
		Imp_SCR_DrawStringExt(320 - strlen("[ JK2MF HTTP Download ]") * 4, 50, 9.3f, "^1[ ^7JK2MF HTTP Download ^1]", stdcol, qfalse);

		char strbuf[800];

		Q_strncpyz(strbuf, "Download^1:", sizeof(strbuf));
		Imp_SCR_DrawStringExt(320 - SCR_Strlen(strbuf) * 4, 300, 9.0, strbuf, stdcol, qfalse);

		int percent = 0;
		if ( httpDownSize )
			percent = (int)(httpDownPos / httpDownSize * 100);
		Q_strncpyz(strbuf, va("%s (%i%%)", httpToFileName, percent), sizeof(strbuf));
		Imp_SCR_DrawStringExt(320 - SCR_Strlen(strbuf) * 4, 315, 9.0, strbuf, stdcol, qfalse);

		char haveBuf[50], willBuf[50];
		UI_ReadableSize(haveBuf, sizeof(haveBuf), httpDownPos);
		UI_ReadableSize(willBuf, sizeof(willBuf), httpDownSize);
		Q_strncpyz(strbuf, va("(%s ^1of ^7%s)", haveBuf, willBuf), sizeof(strbuf));
		Imp_SCR_DrawStringExt(320 - SCR_Strlen(strbuf) * 4, 330, 9.0, strbuf, stdcol, qfalse);

		if ( httpDownNextSet < *clsRealtime )
		{
			httpDownNextSet = *clsRealtime + 250;
			httpDownSpeed = (httpDownPos - httpPosLastSec) * 4;
			httpPosLastSec = httpDownPos;
		}

		if ( httpDownSpeed )
		{
			char transBuf[50];
			UI_ReadableSize(transBuf, sizeof(transBuf), httpDownSpeed);
			Q_strncpyz(strbuf, va("%s/s", transBuf), sizeof(strbuf));
			Imp_SCR_DrawStringExt(320 - SCR_Strlen(strbuf) * 4, 345, 9.0, strbuf, stdcol, qfalse);

			int n = httpDownSize / httpDownSpeed;
			PrintTime(transBuf, sizeof(transBuf), (n - (((httpDownPos/1024) * n) / (httpDownSize/1024))));
			Imp_SCR_DrawStringExt(320 - SCR_Strlen(transBuf) * 4, 360, 9.0, transBuf, stdcol, qfalse);
		}
	}
	else if ( httpResult == DOWNSUCCESS || httpResult == DOWNFAIL )
	{
		UnlockMutex(httpMutex);
		DeleteMutex(&httpMutex);
		Imp_CL_NextDownload();
		return;
	}

	UnlockMutex(httpMutex);
}

void DownloadThread()
{
	char fileUrl[800], toFileReal[800];

	LockMutex(httpMutex);
	Q_strncpyz(fileUrl, httpCurrFileUrl, sizeof(fileUrl));
	Q_strncpyz(toFileReal, httpToFilePath, sizeof(toFileReal));
	Q_strncpyz(toFileTmp, va("%s.tmp", httpToFilePath), sizeof(toFileTmp));

	CURL *curl = curl_easy_init();
	if ( !curl )
	{
		httpResult = DOWNFAIL;
		UnlockMutex(httpMutex);
		EndDownloadThread();
		return;
	}

	httpFile = fopen(toFileTmp, "wb");

	if ( !httpFile )
	{
		httpResult = DOWNFAIL;
		UnlockMutex(httpMutex);
		EndDownloadThread();
		return;
	}

	UnlockMutex(httpMutex);

	curl_easy_setopt(curl, CURLOPT_URL, fileUrl);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, filewriter);
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
	curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, DownloadThreadProcess);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, httpFile);
	CURLcode code = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	fclose(httpFile);

	LockMutex(httpMutex);
	if ( code != CURLE_OK )
	{
		remove(toFileTmp);
		httpResult = DOWNFAIL;
		UnlockMutex(httpMutex);
		EndDownloadThread();
		return;
	}

	rename(toFileTmp, toFileReal);
	httpResult = DOWNSUCCESS;
	UnlockMutex(httpMutex);
	EndDownloadThread();
}

int DownloadThreadProcess(void *ptr, double TotalToDownload, double NowDownloaded, double TotalToUpload, double NowUploaded)
{
    LockMutex(httpMutex);

	if ( httpResult == DOWNENDNOW )
	{
		fclose(httpFile);
		remove(toFileTmp);
		httpResult = DOWNFAIL;
		UnlockMutex(httpMutex);
		EndDownloadThread();
	}

	httpDownSize = TotalToDownload;
	httpDownPos = NowDownloaded;

	UnlockMutex(httpMutex);
	return 0;
}
