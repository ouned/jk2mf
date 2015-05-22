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

struct FuncAddresses {
	ADDR JK2MP104EXE;
	ADDR JK2MP102EXE;
    ADDR ASPYRMAC;
    FUNC JK2MP104EXEHL;
	FUNC JK2MP102EXEHL;
    FUNC ASPRMACHL;
};

enum GameBins {
	NULLBIN,
	JK2MP104EXE,
	JK2MP102EXE,
    ASPYRMAC,
};

enum {
	COM_PRINTF, CVAR_GET, COM_INIT, CL_CONNECT,
	SCR_DRAWSCREENFIELD, SCR_DRAWSTRINGEXT, INFO_VALUEFORKEY,
	CL_DISCONNECT, CURR_SERVERADDR, CLS_STATE, COM_ERROR,
};

ADDR GetAddr(GVER ver, FUNC name);
FUNC Gethl(GVER ver, FUNC name);

void *Detour( void *orig, void *det, int size );

#endif /* DETOURS_H */
