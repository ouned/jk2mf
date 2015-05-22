/***********************************
		   JK2MF Project

		 made for the lulz
***********************************/

#include "main.h"

static int (*syscall)( int arg, ... );

static void Sys_Cvar_VariableStringBuffer(const char *var_name, char *buffer, int bufsize)
{
	syscall(7, var_name, buffer, bufsize);
}
