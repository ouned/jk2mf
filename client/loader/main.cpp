/***********************************
		   JK2MF Project

		 made for the lulz
***********************************/

#define MAIN
#ifdef WIN32
    #include <windows.h>
    #include "libcurl\curl.h"
#else
    #include <stdlib.h>
    #include <dirent.h>
    #include <dlfcn.h>
    #include <sys/stat.h>
    #include <arpa/inet.h>
    #include <sys/select.h>
    #include <curl/curl.h>
#endif
#include "common.h"
#include "detours.h"
#include "main.h"

/* global vars */
static bool init;
static GVER gVer; // binary instance
static int downloadStep;
static int downMirror = -1;
static HMODULE currclplg; // current jk2mfclXX.dll
static char modFileName[60]; // jk2mfclXX.dll

// ==============
// Init Clientloader
// ==============
void Init()
{
	if ( init )
		return;

	init = true;

	SetGameBinaryVersion();

	if ( gVer == NULLBIN ) // This is not a supported jk2 version
		return;

	// Init function addresses
	Imp_Com_Printf = (void(*)(const char *, ...))(GetAddr(gVer, COM_PRINTF));
	Imp_Cvar_Get = (cvar_t*(*)(const char *, const char *, int))(GetAddr(gVer, CVAR_GET));
	
	#ifdef WIN32
		Imp_Com_Init = (void(*)())(GetAddr(gVer, COM_INIT));
	#else
		Imp_Com_Init = (void(*)(char *))(GetAddr(gVer, COM_INIT));
	#endif
	
	Imp_CL_Connect = (void(*)())(GetAddr(gVer, CL_CONNECT));
	Imp_SCR_DrawScreenField = (void(*)(stereoFrame_t))(GetAddr(gVer, SCR_DRAWSCREENFIELD));
	Imp_SCR_DrawStringExt = (void(*)(int, int, float, const char*, float*, qboolean))(GetAddr(gVer, SCR_DRAWSTRINGEXT));
	Imp_Info_ValueForKey = (char*(*)(const char*, const char*))(GetAddr(gVer, INFO_VALUEFORKEY));
	Imp_CL_Disconnect = (void(*)(qboolean))(GetAddr(gVer, CL_DISCONNECT));
	#ifdef __APPLE__
		Imp_Com_Error = (void(*)(int, const char*, ...))(GetAddr(gVer, COM_ERROR));
	#endif

	// Init detours
	Ori_Com_Init = (Com_Init_Def*)Detour((void*)Imp_Com_Init, (void*)Com_Init_Hook, Gethl(gVer, COM_INIT));
	Ori_SCR_DrawScreenField = (SCR_DrawScreenField_Def*)Detour((void*)Imp_SCR_DrawScreenField, (void*)SCR_DrawScreenField_Hook, Gethl(gVer, SCR_DRAWSCREENFIELD));
	Ori_CL_Disconnect = (CL_Disconnect_Def*)Detour((void*)Imp_CL_Disconnect, (void*)CL_Disconnect_Hook, Gethl(gVer, CL_DISCONNECT));
	#ifdef __APPLE__
        if ( gVer == ASPYRMAC )
            Ori_Com_Error = (Com_Error_Def*)Detour((void*)Imp_Com_Error, (void*)Com_Error_Hook, Gethl(gVer, COM_ERROR));
	#endif

	// Init vars
	servAddr = (netadr_t*)(GetAddr(gVer, CURR_SERVERADDR));
	clsState = (int*)(GetAddr(gVer, CLS_STATE));

	currclplg = NULL;
	downloadStep = 0;

	// Init winsockets
	#ifdef WIN32
		WSADATA	wsadata;
		WSAStartup(MAKEWORD(1,0), &wsadata);
	#endif
}

void InitCvars()
{
	// Init cvars
	fs_homepath = Imp_Cvar_Get("fs_homepath", "", 0);
}

// ==============
// Copy original memory back
// Should never be called on MacOSX
// ==============
void Shutdown()
{
	if ( gVer == NULLBIN )
		return;

	if ( !init )
		return;

	init = false;

	// Undetour everything
	memcpy((void*)Imp_Com_Init, (void*)Ori_Com_Init, Gethl(gVer, COM_INIT));
	free((void*)Ori_Com_Init);
	memcpy((void*)Imp_SCR_DrawScreenField, (void*)Ori_SCR_DrawScreenField, Gethl(gVer, SCR_DRAWSCREENFIELD));
	free((void*)Ori_SCR_DrawScreenField);
	memcpy((void*)Imp_CL_Disconnect, (void*)Ori_CL_Disconnect, Gethl(gVer, CL_DISCONNECT));
	free((void*)Ori_CL_Disconnect);
	#ifdef __APPLE__
		memcpy((void*)Imp_Com_Error, (void*)Ori_Com_Error, Gethl(gVer, COM_ERROR));
		free((void*)Ori_Com_Error);
	#endif
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
		else if (strstr(jkMPExe_1_02, "JK2MP: v1.02"))
			gVer = JK2MP102EXE;
	#else
		char *jkAspyreMac = (char*)(0xfe7e8); // 0xfe7e8 Jedi Knight II -> JK2MP: v1.04
        if ( strstr(jkAspyreMac, "JK2MP: v1.5") )
            gVer = ASPYRMAC;
    #endif
}

// ==============
// NET_Init
//
// Not loading any basedll anymore
// but print version information
// ==============
#ifdef WIN32
void Com_Init_Hook()
#else
void Com_Init_Hook(char *commandLine)
#endif
{
	#ifdef WIN32
		Ori_Com_Init();
	#else
		Ori_Com_Init(commandLine);
	#endif

	InitCvars();
  Imp_Com_Printf("^1[ ^7JK2MF Clientside ^1v^7%s ^1]^7\n", VERSION);
}

void ClearTMPData()
{
    #ifdef WIN32
		char path[800];
		WIN32_FIND_DATAA FindFileData;
		Q_strncpyz(path, va("%s\\jk2mf\\*", getenv("appdata")), sizeof(path));

		HANDLE hFind = FindFirstFileA(path, &FindFileData);
		if ( hFind == INVALID_HANDLE_VALUE )
			return;

		do
		{
			if ( strstr(FindFileData.cFileName, ".tmp") )
				remove(va("%s\\jk2mf\\%s", getenv("appdata"), FindFileData.cFileName));
		} while ( FindNextFileA(hFind, &FindFileData) );
    #else
		char path[800];
		Q_strncpyz(path, va("%s/.jk2mf", getenv("HOME")), sizeof(path));
    
		DIR *pdir = NULL;
		pdir = opendir(path);
		struct dirent *pent = NULL;
		while ( pdir && (pent = readdir(pdir)) )
		{
			if ( strstr(pent->d_name, ".tmp") )
				remove(va("%s/.jk2mf/%s", getenv("HOME"), pent->d_name));
		}
    
        if ( pdir )
            closedir(pdir);
    #endif
}

void SCR_DrawScreenField_Hook(stereoFrame_t stereoFrame)
{
	Ori_SCR_DrawScreenField(stereoFrame);

	if ( downloadStep == DOWNLOAD )
	{
		DownloadServersClientPlugin();
	}

	if ( *clsState == CA_CHALLENGING ) // cls.state
	{
		DownloadServersClientPlugin();
		downloadStep++;
	}
}

void DownloadServersClientPlugin()
{
	if ( downloadStep == INIT )
	{
		// Get Serverinfo
		char servInf[2048];

		if ( servAddr->ip[0] == 0 && servAddr->ip[1] == 0 && servAddr->ip[2] == 0 && servAddr->ip[3] == 0 )
		{
			downloadStep = DONOTHING;
			return;
		}

		int sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		sockaddr_in peer;
		peer.sin_addr.s_addr = inet_addr(va("%u.%u.%u.%u", servAddr->ip[0], servAddr->ip[1], servAddr->ip[2], servAddr->ip[3]));
		peer.sin_port    	= servAddr->port;
		peer.sin_family  	= AF_INET;

		// Request status and search for the JK2MF Cvar
		// This is done because it makes connecting faster on none JK2MF servers
		send_recv(sd, INFOREQ, sizeof(INFOREQ), servInf, sizeof(servInf), &peer);
		if ( !strstr(servInf, "\\JK2MF\\") )
		{
			downloadStep = DONOTHING;
			return;
		}

		// Send the real JK2MF request
		// Makes it impossible to fake for noobs
		int outlen = send_recv(sd, MFREQ, sizeof(MFREQ), servInf, sizeof(servInf), &peer);

		char verStr[50];
		Q_strncpyz(verStr, servInf+4, sizeof(verStr));

		if ( !outlen || !strchr(verStr, '.') )
		{
			downloadStep = DONOTHING;
			return;
		}

		char filtered[50];
		int count = 0;
		for( int i = 0; i < (int)strlen(verStr); i++ )
		{
			if ( count == sizeof(verStr)-1 )
				break;
			if ( verStr[i] == '.' )
				continue;

			filtered[count] = verStr[i];
			count++;
		}
		filtered[count] = 0;
        
        #ifdef WIN32
            Q_strncpyz(modFileName, va("jk2mfcl%s.dll", filtered), sizeof(modFileName));
        #else
            Q_strncpyz(modFileName, va("jk2mfcl%s.dylib", filtered), sizeof(modFileName));
        #endif

        #ifdef WIN32
		if ( FileExists(va("%s\\jk2mf\\%s", getenv("appdata"), modFileName)) )
        #else
        if ( FileExists(va("%s/.jk2mf/%s", getenv("HOME"), modFileName)) )
        #endif
		{
			LoadClientSide();
			downloadStep = DONOTHING;
			return;
		}

		char drawText[100];
		Q_strncpyz(drawText, "Downloading JK2MF Clientside", sizeof(drawText));

		float color[4] = {1.0, 0.0, 0.0, 1.0};
		Imp_SCR_DrawStringExt(320 - strlen(drawText) * 4, 40, 10, drawText, color, qtrue);
	}
	else if ( downloadStep == DOWNLOAD )
	{
		SetDownloadMirror();

		char avaiables[6000];
		#ifdef WIN32
			bool res = GetWebPageText(avaiables, "clplgs.inf");
		#else
			bool res = GetWebPageText(avaiables, "clplgsmac.inf");
		#endif
		if ( !res )
		{
			Imp_Com_Printf("JK2MF^1: ^7Download impossible (check your internet connection), proceeding without...\n");
			downloadStep = DONOTHING;
			return;
		}

		if ( !strstr(avaiables, va("%s|", modFileName)) )
		{
			Imp_Com_Printf("JK2MF^1: ^7Invalid plugin version, proceeding without...\n");
			downloadStep = DONOTHING;
			return;
		}

		char downPath[800];
		char downUrl[800];
		Imp_Com_Printf("JK2MF^1: ^7Downloading %s from %s\n", modFileName, mirrorNames[downMirror]);

        #ifdef WIN32
            CreateDirectoryA(va("%s\\jk2mf", getenv("appdata")), NULL);
        #else
            mkdir(va("%s/.jk2mf", getenv("HOME")), 755);
        #endif
		ClearTMPData();
        
        #ifdef WIN32
            Q_strncpyz(downPath, va("%s\\jk2mf\\%s.tmp", getenv("appdata"), modFileName), sizeof(downPath));
        #else
            Q_strncpyz(downPath, va("%s/.jk2mf/%s.tmp", getenv("HOME"), modFileName), sizeof(downPath));
        #endif
        Q_strncpyz(downUrl, va("clplgs/%s", modFileName), sizeof(downUrl));
        
		if ( DownloadFile(downPath, downUrl) )
		{
            #ifdef WIN32
                rename(downPath, va("%s\\jk2mf\\%s", getenv("appdata"), modFileName));
            #else
                rename(downPath, va("%s/.jk2mf/%s", getenv("HOME"), modFileName));
            #endif
			LoadClientSide();
		}
		else
		{
			Imp_Com_Printf("JK2MF^1: ^7Download failed, proceeding without clientside...\n");
		}

		downloadStep = DONOTHING;
	}
}

void LoadClientSide()
{
	if ( !currclplg )
	{
		Imp_Com_Printf("JK2MF^1: ^7Loading clientplugin %s\n", modFileName);
        
        #ifdef WIN32
            currclplg = LoadLibraryA(va("%s\\jk2mf\\%s", getenv("appdata"), modFileName));
        #else
            currclplg = dlopen(va("%s/.jk2mf/%s", getenv("HOME"), modFileName), RTLD_NOW);
        #endif
	}
}

void CL_Disconnect_Hook(qboolean showMainMenu)
{
	Ori_CL_Disconnect(showMainMenu);

	downloadStep = INIT;

	if ( currclplg )
	{
		Imp_Com_Printf("JK2MF^1: ^7Unloading %s\n", modFileName);
        #ifdef WIN32
            FreeLibrary(currclplg);
        #else
            dlclose(currclplg);
        #endif
		currclplg = NULL;
	}
}

// ==============
// Com_Error
//
// For some reason the MacOSX versions are throwing an error if someone trys to
// connect to a server with disabled sv_pure and there are pk3's referenced.
// ==============
#ifdef __APPLE__
void Com_Error_Hook(int code, const char *fmt, ...)
{
	if ( strstr(fmt, "You are missing some files referenced") )
	{
		Imp_Com_Printf(fmt);
		return;
	}

	Ori_Com_Error(code, fmt);
}
#endif

// ==============
// Get Serverinfo
//
// Get status of the server
// to determine which JK2MF clientside is required
// ==============
int send_recv(int sd, const char *in, int inlen, char *out, int outlen, sockaddr_in *peer)
{
	int retry = 0;

	for(retry = 2; retry > 0; retry--) 
	{
    	sendto(sd, in, inlen, 0, (sockaddr*)peer, sizeof(sockaddr_in));
		if( !timeout(sd) )
			break;
	}
	if ( !retry )
		return 0;

	outlen = recvfrom(sd, out, outlen, 0, NULL, NULL);
	out[outlen] = 0;

	return outlen;
}

int timeout(int sock)
{
	struct  timeval tout;
	fd_set  fd_read;
	int 	err;

	tout.tv_sec = 0;
	tout.tv_usec = 900000;
	FD_ZERO(&fd_read);
	FD_SET(sock, &fd_read);
	err = select(sock + 1, &fd_read, NULL, NULL, &tout);
	if(!err) return(-1);
	return(0);
}

// ==============
// Download procedures
//
// Download plugins & other information
// not only from ouned.de anymore
// ==============

void SetDownloadMirror()
{
	CURL *curl = curl_easy_init();
	
	if ( !curl )
		return;

	for ( int i = 0; i < sizeof(mirrors) / sizeof(mirrors[0]); i++ )
	{
		char out[10000] = "";

		curl_easy_setopt(curl, CURLOPT_URL, va("%s/mirrstatus.cfg", mirrors[i]));
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, out);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
		CURLcode code = curl_easy_perform(curl);

		if ( code == CURLE_OK && !strcmp(out, "active") )
		{
			downMirror = i;
			break;
		}
	}

	curl_easy_cleanup(curl);
}

bool GetWebPageText(char *out, const char *url)
{
	CURL *curl = curl_easy_init();

	if ( !curl || downMirror == -1 )
		return false;

	curl_easy_setopt(curl, CURLOPT_URL, va("%s/%s", mirrors[downMirror], url));
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, out);
	CURLcode code = curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	if ( code != CURLE_OK )
		return false;

	if ( !strlen(out) )
		return false;

	return true;
}

bool DownloadFile(const char *toPath, const char *url)
{
	CURL *curl = curl_easy_init();

	if ( !curl || downMirror == -1 )
		return false;

	FILE *file = fopen(toPath, "wb");

	if ( !file )
		return false;

	curl_easy_setopt(curl, CURLOPT_URL, va("%s/%s", mirrors[downMirror], url));
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, filewriter);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
	CURLcode code = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	fclose(file);

	if ( code != CURLE_OK )
		return false;

	return true;
}
