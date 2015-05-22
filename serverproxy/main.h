/***********************************
		   JK2MF Project

		 made for the lulz
***********************************/

#include <string.h>

#ifndef MAIN_H
#define MAIN_H

#ifdef WIN32
    #define EXPORTFUNC extern "C" __declspec( dllexport )
#else
    #define EXPORTFUNC extern "C"
#endif

#define CVAR_INTERNAL 0x00000800
#define	MAX_CVAR_VALUE_STRING	256
typedef int	cvarHandle_t;
typedef struct {
	cvarHandle_t	handle;
	int			modificationCount;
	float		value;
	int			integer;
	char		string[MAX_CVAR_VALUE_STRING];
} vmCvar_t;

static void Q_strncpyz( char *dest, const char *src, int destsize ) {
	strncpy( dest, src, destsize-1 );
	dest[destsize-1] = 0;
}

static void Q_strcat( char *dest, int size, const char *src ) {
	int		l1;

	l1 = strlen( dest );
	Q_strncpyz( dest + l1, src, size - l1 );
}

#endif /* MAIN_H */
