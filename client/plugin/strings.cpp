/***********************************
		   JK2MF Project

		 made for the lulz
***********************************/

#include <stdlib.h>
#include <string>
#include "strings.h"
#include "common.h"
#include "main.h"

static char *strPtr;

char strings[][STRINGLENGTH] = {
	"cl_maxpackets", "50",
	"com_maxfps", "85",
	"snaps", "30",
	"rate", "5000",
	"jk2mfcl",
	"mfsrvcmd version \"%s\" \"%s\"",
	"mfclcmd ",
	"chksum",
	"mfsrvcmd chksum %s %s",
	"%u ",
	"mfsrvcmd scripter",
	"bind ",
	"wait",
	"vstr",
	"exec",
	"vm/cgame.qvm",
	"opengl32.dll",
	"set ",
	"seta ",
	"setu ",
	VERSION,
	"state",
	"com_hunkmegs", "128",
	OS,
};

// ==============
// InitStrings
//
// Because strings are encrypted copy them to the heap and decrypt them
// ==============
void InitStrings()
{
	strPtr = (char*)malloc(sizeof(strings));
	memcpy(strPtr, strings, sizeof(strings));
}

void FreeStrings()
{
	free(strPtr);
}

char *GetStr(int index)
{
	return strPtr + ( index * STRINGLENGTH);
}

uint GetStrChksum()
{
	return hash(strPtr, sizeof(strings), 23565);
}
