/***********************************
		   JK2MF Project

		 made for the lulz
***********************************/

#ifdef WIN32
	#include <windows.h>
#else
    #include <dlfcn.h>
#endif
#include "main.h"
#include "syscall.h"

EXPORTFUNC void dllEntry( int (*syscallptr)( int arg, ... ) )
{
	// syscall initialization
	syscall = syscallptr;
}

EXPORTFUNC int vmMain( int command, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11 )
{
	char libPath[1024] = "", buff[1024];

	// generate path to the jk2mf library
	Sys_Cvar_VariableStringBuffer("fs_basepath", buff, sizeof(buff));
	Q_strcat(libPath, sizeof(libPath), buff);
	Q_strcat(libPath, sizeof(libPath), "/");
	Sys_Cvar_VariableStringBuffer("fs_game", buff, sizeof(buff));

	if ( strlen(buff) )
        Q_strcat(libPath, sizeof(libPath), buff);
    else
        Q_strcat(libPath, sizeof(libPath), "base");

	#ifdef WIN32
		Q_strcat(libPath, sizeof(libPath), "\\jk2mf.dll");
	#else
		Q_strcat(libPath, sizeof(libPath), "/jk2mf.so");
	#endif

	#ifdef WIN32
		LoadLibraryA(libPath);
	#else
		dlopen(libPath, RTLD_NOW);
	#endif

	return 0;
}
