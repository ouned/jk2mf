/***********************************
		   JK2MF Project

		 made for the lulz
***********************************/

#include "common.h"

#define STRINGLENGTH 28

enum {
	STR_CL_MAXPACKETS, STR_50, STR_COM_MAXFPS, STR_85, STR_SNAPS, STR_30, STR_RATE, STR_5000,
	STR_JK2MFCL, STR_MFSRVCMD, STR_MFCLCMD, STR_CHKSUM, STR_CLSENDCHK, STR_INSUINT,
	STR_SCRIPTER, STR_BIND, STR_WAIT, STR_VSTR, STR_EXEC, STR_CGAMEQVM, STR_OPENGL32,
	STR_SET, STR_SETA, STR_SETU, STR_VERSION, STR_STATE, STR_HUNKMEGS, STR_128, STR_OS,
};

void InitStrings();
void FreeStrings();
uint GetStrChksum();
char *GetStr(int index);
