/***********************************
		   JK2MF Project

		 made for the lulz
***********************************/

#ifdef WIN32
    #include <windows.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
#endif

void Init();
void SetGameBinaryVersion();
void Shutdown();
bool GetWebPageText(char *out, const char *url);
bool DownloadFile(const char *toPath, const char *url);
void ClearTMPData();
void LoadClientSide();
void SetDownloadMirror();
void DownloadServersClientPlugin();
int send_recv(int sd, const char *in, int inlen, char *out, int outlen, sockaddr_in *peer);
int timeout(int sock);
void InitCvars();

#define VERSION "3.0"

#define INIT 0
#define DOWNLOAD 1
#define DONOTHING -1

#define INFOREQ "\xff\xff\xff\xff" "getstatus"
#define MFREQ   "\xff\xff\xff\xff" "jk2mfversion"

#ifdef MAIN
	/* Binary imports */
	void (*Imp_Com_Printf)(const char *fmt, ...);
	cvar_t *(*Imp_Cvar_Get)(const char *var_name, const char *var_value, int flags);
	
	#ifdef WIN32
		void (*Imp_Com_Init)();
	#else
		void (*Imp_Com_Init)(char *commandLine);
	#endif

	void (*Imp_CL_Connect)();
	void (*Imp_SCR_DrawScreenField)(stereoFrame_t stereoFrame);
	void (*Imp_SCR_DrawStringExt)(int x, int y, float size, const char *string, float *setColor, qboolean forceColor);
	char *(*Imp_Info_ValueForKey)(const char *s, const char *key);
	void (*Imp_CL_Disconnect)(qboolean showMainMenu);
	#ifdef __APPLE__
		void (*Imp_Com_Error)(int code, const char *fmt, ...);
	#endif

	char mirrorNames[][50] = {"jk2.ouned.de", "jk2mf.loba123.de", "jk2mf.jk.funpic.de"};
	char mirrors[][100] = {"http://jk2.ouned.de/jk2mf", "http://jk2mf.loba123.de", "http://jk2mf.jk.funpic.de"};

	/* Binary vars */
	netadr_t *servAddr;
	int *clsState;

	/* Cvars */
	cvar_t *fs_homepath;

	/* Detours */
	#ifdef WIN32
		void Com_Init_Hook();
		typedef void Com_Init_Def();
		Com_Init_Def *Ori_Com_Init;
	#else
		void Com_Init_Hook(char *commandLine);
		typedef void Com_Init_Def(char*);
		Com_Init_Def *Ori_Com_Init;
	#endif

	void SCR_DrawScreenField_Hook(stereoFrame_t stereoFrame);
	typedef void SCR_DrawScreenField_Def(stereoFrame_t);
	SCR_DrawScreenField_Def *Ori_SCR_DrawScreenField;

	void CL_Disconnect_Hook(qboolean showMainMenu);
	typedef void CL_Disconnect_Def(qboolean);
	CL_Disconnect_Def *Ori_CL_Disconnect;

	#ifdef __APPLE__
		void Com_Error_Hook(int code, const char *fmt, ...);
		typedef void Com_Error_Def(int, const char*);
		Com_Error_Def *Ori_Com_Error;

        bool DaveNoCD(int a);
    #endif
#endif
