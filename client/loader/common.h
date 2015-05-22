/***********************************
		   JK2MF Project

		 made for the lulz
***********************************/

#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdio.h>
#ifdef WIN32
    #include <windows.h>
    #include <io.h>
#endif

#ifndef COMMON_H
#define COMMON_H

#ifdef __APPLE__
    typedef void* HMODULE;
#endif

typedef enum {qfalse, qtrue} qboolean;
typedef unsigned char byte;
#define	BIG_INFO_STRING		8192

typedef struct cvar_s {
	char		*name;
	char		*string;
	char		*resetString;		// cvar_restart will reset to this value
	char		*latchedString;		// for CVAR_LATCH vars
	int			flags;
	qboolean	modified;			// set each time the cvar is changed
	int			modificationCount;	// incremented each time the cvar is changed
	float		value;				// atof( string )
	int			integer;			// atoi( string )
	struct cvar_s *next;
	struct cvar_s *hashNext;
} cvar_t;

typedef enum {
	NA_BOT,
	NA_BAD,					// an address lookup failed
	NA_LOOPBACK,
	NA_BROADCAST,
	NA_IP,
	NA_IPX,
	NA_BROADCAST_IPX
} netadrtype_t;

typedef struct {
	netadrtype_t	type;

	byte	ip[4];
	byte	ipx[10];

	unsigned short	port;
} netadr_t;

typedef struct serverStatus_s
{
	char string[BIG_INFO_STRING];
	netadr_t address;
	int time, startTime;
	qboolean pending;
	qboolean print;
	qboolean retrieved;
} serverStatus_t;

typedef enum {
	CA_UNINITIALIZED,
	CA_DISCONNECTED, 	// not talking to a server
	CA_AUTHORIZING,		// not used any more, was checking cd key 
	CA_CONNECTING,		// sending request packets to the server
	CA_CHALLENGING,		// sending challenge packets to the server
	CA_CONNECTED,		// netchan_t established, getting gamestate
	CA_LOADING,			// only during cgame initialization, never during main loop
	CA_PRIMED,			// got gamestate, waiting for first frame
	CA_ACTIVE,			// game views should be displayed
	CA_CINEMATIC		// playing a cinematic or a static pic, not connected to a server
} connstate_t;

typedef enum {
	STEREO_CENTER,
	STEREO_LEFT,
	STEREO_RIGHT
} stereoFrame_t;

bool FileExists(char *file);
int writer(char *data, size_t size, size_t nmemb, char *writerData);
size_t filewriter(void *ptr, size_t size, size_t nmemb, FILE *stream);
int Q_strncmp (const char *s1, const char *s2, int n);
void Q_strncpyz( char *dest, const char *src, int destsize );
void Q_strcat(char *dest, int size, const char *src);
char *va( const char *format, ... );

#endif /* COMMON_H */
