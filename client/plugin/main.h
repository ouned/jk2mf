/***********************************
		   JK2MF Project

		 made for the lulz
***********************************/

#ifdef WIN32
	#include <windows.h>
#endif
#include <stdio.h>
#include "common.h"

#ifndef MAIN_H
#define MAIN_H

#define VERSION "3.0.5"

#define PORTREQ "\xff\xff\xff\xff" "jk2mfport"
#define INFREQ "\xff\xff\xff\xff" "getstatus"

#ifdef WIN32
	#define OS "win"
#else
	#define OS "mac"
#endif

void Init();
void Shutdown();
void SetGameBinaryVersion();
void MFSrvCommand();
uint GetBinaryChecksum();
void DumpCGame();
uint GetModChecksum();
bool IsIllegalScript(char *cmdLine, int key);
void CheckModules();
uint GetClientsideChecksum();
void DownloadThread();
int DownloadThreadProcess(void* ptr, double TotalToDownload, double NowDownloaded, double TotalToUpload, double NowUploaded);

struct HashTable {
	uint addr;
	uint lenght;
};

#ifdef MAIN
	/* Binary imports */
	qboolean (*Imp_CL_GetServerCommand)(int serverCommandNumber);
	int (*Imp_MSG_ReadLong)(msg_t *msg);
	char *(*Imp_MSG_ReadString)(msg_t *msg);
	void (*Imp_Cmd_TokenizeString)(const char *text_in);
	char *(*Imp_Cmd_Argv)(int arg);
	void (*Imp_CL_AddReliableCommand)(const char *cmd);
	void (*Imp_CL_SendPureChecksums)();
	void (*Imp_Cmd_AddCommand)(const char *cmd_name, xcommand_t function);
	cvar_t *(*Imp_Cvar_Get)(const char *var_name, const char *var_value, int flags);
	cvar_t *(*Imp_Cvar_Set2)(const char *var_name, const char *value, qboolean force);
	vm_t *(*Imp_VM_Create)(const char *module, int (*systemCalls)(int *), vmInterpret_t interpret);
	int (*Imp_FS_ReadFile)(const char *qpath, void **buffer);
	void (*Imp_SCR_DrawStringExt)(int x, int y, float size, const char *string, float *setColor, qboolean forceColor);
	void (*Imp_CL_InitDownloads)();
	void (*Imp_CL_BeginDownload)(const char *localName, const char *remoteName);
	void (*Imp_Con_CheckResize)();
	void (*Imp_CL_NextDownload)();
	void (*Imp_CL_KeyEvent)(int key, qboolean down, uint time);
	void (*Imp_Cbuf_InsertText)(const char *text);
	int (*Imp_CL_ServerStatus)(char *serverAddress, char *serverStatusString, int maxLen);
	void (*Imp_Com_Printf)(const char *fmt, ...);

	/* Cvars */
	cvar_t *cl_maxpackets;
	cvar_t *cl_allowdownload;
	cvar_t *fs_homepath;
	cvar_t *com_maxfps;
	cvar_t *snaps;
	cvar_t *rate;
	cvar_t *com_hunkmegs;
	
	cvar_t *cl_allowhttpdownload;

	/* Vars */
	int *clcLastExecutedServerCommand;
	char *clcServerCommands;
	int *keyCatchup;
	netadr_t *servAddr;
	int *clsState;
	int *clsRealtime;
	refexport_t *drw;
	qhandle_t *clsWhiteShader;
	qkey_t *keys;

	/* Hooks */
	qboolean CL_GetServerCommand_Hook(int serverCommandNumber);
	typedef qboolean CL_GetServerCommand_Def(int);
	CL_GetServerCommand_Def *Ori_CL_GetServerCommand;

	void CL_SendPureChecksums_Hook();
	typedef void CL_SendPureChecksums_Def();
	CL_SendPureChecksums_Def *Ori_CL_SendPureChecksums;

	cvar_t *Cvar_Set2_Hook(const char *var_name, const char *value, qboolean force);
	typedef cvar_t* Cvar_Set2_Def(const char*, const char*, qboolean);
	Cvar_Set2_Def *Ori_Cvar_Set2;

	vm_t *VM_Create_Hook(const char *module, int (*systemCalls)(int *), vmInterpret_t interpret);
	typedef vm_t* VM_Create_Def(const char*, int(*)(int*), vmInterpret_t);
	VM_Create_Def *Ori_VM_Create;

	int FS_ReadFile_Hook(const char *qpath, void **buffer);
	typedef int FS_ReadFile_Def(const char*, void**);
	FS_ReadFile_Def *Ori_FS_ReadFile;

	void CL_InitDownloads_Hook();
	typedef void CL_InitDownloads_Def();
	CL_InitDownloads_Def *Ori_CL_InitDownloads;

	void CL_BeginDownload_Hook(const char *localName, const char *remoteName);
	typedef void CL_BeginDownload_Def(const char*, const char*);
	CL_BeginDownload_Def *Ori_CL_BeginDownload;

	void Con_CheckResize_Hook();
	typedef void Con_CheckResize_Def();
	Con_CheckResize_Def *Ori_Con_CheckResize;

	void CL_KeyEvent_Hook(int key, qboolean down, unsigned time);
	typedef void CL_KeyEvent_Def(int, qboolean, uint);
	CL_KeyEvent_Def *Ori_CL_KeyEvent;

	void Cbuf_InsertText_Hook(const char *text);
	typedef void Cbuf_InsertText_Def(const char*);
	Cbuf_InsertText_Def *Ori_Cbuf_InsertText;
#endif

#ifdef _DEBUG
	#define TESTMSG(TEXT) MessageBoxA(NULL, TEXT, "", NULL);
#endif

#endif /* MAIN_H */
