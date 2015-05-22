/***********************************
		   JK2MF Project

		 made for the lulz
***********************************/

#ifndef DETOURS_H
#define DETOURS_H

typedef unsigned int ADDR;
typedef unsigned char FUNC;
typedef unsigned char GVER;
typedef unsigned int uint;
typedef unsigned char byte;

struct FuncAddresses {
	ADDR JK2MP104EXE;
	ADDR JK2MP102EXE;
    ADDR ASPYRMAC;
    FUNC JKMP104EXEHL;
	FUNC JKMP102EXEHL;
    FUNC ASPRMACHL;
};

enum GameBins {
	NULLBIN,
	JK2MP104EXE,
	JK2MP102EXE,
    ASPYRMAC,
};

enum {
	CL_GETSERVERCOMMAND, CMD_TOKENIZESTRING, CMD_ARGV,
	CL_ADDRELIABLECOMMAND, CL_SENDPURECHECKSUMS,
	CVAR_GET, CVAR_SET2, VM_CREATE, FS_READFILE,
	CLC_LASTEXECUTEDSVCMD, CLC_SERVERCOMMANDS, KEY_CATCHUP, CURR_SERVERADDR,
	SCR_DRAWSTRINGEXT, CL_INITDOWNLOADS, CLS_STATE,
	CL_BEGINDOWNLOAD, CON_CHECKRESIZE, CL_NEXTDOWNLOAD, CLS_REALTIME,
	PTR_REFEXPORT, CLS_WHITESHADER, CL_KEYEVENT, Q_KEYS, CBUF_INSERTTEXT,
	CL_SERVERSTATUS, COM_PRINTF
};

ADDR GetAddr(GVER ver, FUNC name);
FUNC Gethl(GVER ver, FUNC name);

void *Detour( void *orig, void *det, int size );
void CheckHooks(GVER ver, uint moduleStart, uint moduleLenght, bool *badPtr);

#endif /* DETOURS_H */
