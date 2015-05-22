/***********************************
		   JK2MF Project

		 made for the lulz
***********************************/

#ifdef WIN32
    #include <windows.h>
    #include <process.h>
#else
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <sys/select.h>
    #include <pthread.h>
#endif
#include "detours.h"

#ifndef COMMON_H
#define COMMON_H

#ifdef WIN32
#define JunkCode() \
	__asm jmp end \
	__asm push eax \
	__asm push edx \
	__asm jmp eax \
	__asm jz end \
	__asm xchg eax, edx \
	__asm setpo al \
	__asm sal edx, 16 \
	__asm mov eax, [esi-4] \
	__asm xchg eax, edx \
	__asm push eax \
	__asm push edx \
	__asm mov edx, [esi+4*ebx] \
	__asm jz end \
	__asm xchg eax, edx \
	__asm setpo al \
	__asm sal edx, 12 \
	__asm setpo al \
	__asm jmp end \
	__asm push eax \
	__asm push edx \
	__asm zw: jmp realend \
	__asm jz end \
	__asm xchg eax, edx \
	__asm setpo al \
	__asm sal edx, 10 \
	__asm setpo al \
	__asm jmp end \
	__asm push eax \
	__asm push edx \
	__asm sal edx, 3 \
	__asm pop eax \
	__asm mov eax, [esi-4] \
	__asm mov edx, [edi+2*ebx] \
	__asm pop eax \
	__asm sal edx, 1 \
	__asm pop eax \
	__asm pop eax \
	__asm push edx \
	__asm jz end \
	__asm xchg eax, edx \
	__asm end: jmp zw \
	__asm jmp end \
	__asm mov BYTE PTR [ebx], 2 \
	__asm push eax \
	__asm push edx \
	__asm sal edx, 67 \
	__asm realend:
#endif

typedef enum {qfalse, qtrue} qboolean;
#define MAX_TOKEN_CHARS 1024
typedef unsigned int uint;
typedef void (*xcommand_t) (void);
#define Q_COLOR_ESCAPE	'^'
#define Q_IsColorString(p)	( p && *(p) == Q_COLOR_ESCAPE && *((p)+1) && *((p)+1) != Q_COLOR_ESCAPE )
#define	MAX_CMD_LINE	1024

#define	CVAR_ARCHIVE		1	// set to cause it to be saved to vars.rc
								// used for system variables, not for player
								// specific configurations
#define	CVAR_USERINFO		2	// sent to server on connect or change
#define	CVAR_SERVERINFO		4	// sent in response to front end requests
#define	CVAR_SYSTEMINFO		8	// these cvars will be duplicated on all clients
#define	CVAR_INIT			16	// don't allow change from console at all,
								// but can be set from the command line
#define	CVAR_LATCH			32	// will only change when C code next does
								// a Cvar_Get(), so it can't be changed
								// without proper initialization.  modified
								// will be set, even though the value hasn't
								// changed yet
#define	CVAR_ROM			64	// display only, cannot be set by user at all
#define	CVAR_USER_CREATED	128	// created by a set command
#define	CVAR_TEMP			256	// can be set even when cheats are disabled, but is not archived
#define CVAR_CHEAT			512	// can not be changed if cheats are disabled
#define CVAR_NORESTART		1024	// do not clear when a cvar_restart is issued

typedef enum {
	STEREO_CENTER,
	STEREO_LEFT,
	STEREO_RIGHT
} stereoFrame_t;

typedef struct {
	qboolean	allowoverflow;	// if false, do a Com_Error
	qboolean	overflowed;		// set to true if the buffer size failed (with allowoverflow set)
	qboolean	oob;			// set to true if the buffer size failed (with allowoverflow set)
	byte	*data;
	int		maxsize;
	int		cursize;
	int		readcount;
	int		bit;				// for bitwise reads and writes
} msg_t;

#define	MAX_QPATH		64
typedef struct vm_s vm_t;

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

#define	BIG_INFO_STRING		8192
#define BIG_INFO_VALUE 8192

typedef struct serverStatus_s
{
	char string[BIG_INFO_STRING];
	netadr_t address;
	int time, startTime;
	qboolean pending;
	qboolean print;
	qboolean retrieved;
} serverStatus_t;

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

typedef int		qhandle_t;

typedef struct {
	// called before the library is unloaded
	// if the system is just reconfiguring, pass destroyWindow = qfalse,
	// which will keep the screen from flashing to the desktop.
	void	(*Shutdown)( qboolean destroyWindow );

	// All data that will be used in a level should be
	// registered before rendering any frames to prevent disk hits,
	// but they can still be registered at a later time
	// if necessary.
	//
	// BeginRegistration makes any existing media pointers invalid
	// and returns the current gl configuration, including screen width
	// and height, which can be used by the client to intelligently
	// size display elements
	void	(*BeginRegistration)(  );
	qhandle_t (*RegisterModel)( const char *name );
	qhandle_t (*RegisterSkin)( const char *name );
	qhandle_t (*RegisterShader)( const char *name );
	qhandle_t (*RegisterShaderNoMip)( const char *name );
	void	(*LoadWorld)( const char *name );

	// the vis data is a large enough block of data that we go to the trouble
	// of sharing it with the clipmodel subsystem
	void	(*SetWorldVisData)( const byte *vis );
	void	(*SetWorldVisdData)( const byte *vis );

	// EndRegistration will draw a tiny polygon with each texture, forcing
	// them to be loaded into card memory
	void	(*EndRegistration)( void );

	// a scene is built up by calls to R_ClearScene and the various R_Add functions.
	// Nothing is drawn until R_RenderScene is called.
	void	(*ClearScene)( void );
	void	(*AddRefEntityToScene)( );
	void	(*AddPolyToScene)(  );
	int		(*LightForPoint)(  );
	void	(*AddLightToScene)( );
	void	(*AddAdditiveLightToScene)();
	void	(*RenderScene)();

	void	(*SetColor)( const float *rgba );	// NULL = 1,1,1,1
	void	(*DrawStretchPic) ( float x, float y, float w, float h, 
		float s1, float t1, float s2, float t2, qhandle_t hShader );	// 0 = white
} refexport_t;

struct vm_s {
    // DO NOT MOVE OR CHANGE THESE WITHOUT CHANGING THE VM_OFFSET_* DEFINES
    // USED BY THE ASM CODE
    int			programStack;		// the vm may be recursively entered
    int			(*systemCall)( int *parms );

	//------------------------------------
   
    char		name[MAX_QPATH];

	// for dynamic linked modules
	void		*dllHandle;
	int			(*entryPoint)( int callNum, ... );

	// for interpreted modules
	qboolean	currentlyInterpreting;

	qboolean	compiled;
	byte		*codeBase;
	int			codeLength;

	int			*instructionPointers;
	int			instructionPointersLength;

	byte		*dataBase;
	int			dataMask;

	int			stackBottom;		// if programStack < stackBottom, error

	int			numSymbols;
	struct vmSymbol_s	*symbols;

	int			callLevel;			// for debug indenting
	int			breakFunction;		// increment breakCount on function entry to this
	int			breakCount;

// fqpath member added 7/20/02 by T.Ray
	char		fqpath[MAX_QPATH+1] ;
};

#define	MAX_KEYS		256
#define BIG_INFO_KEY 8192

typedef struct {
	qboolean	down;
	int			repeats;		// if > 1, it is autorepeating
	char		*binding;
} qkey_t;

typedef enum {
	VMI_NATIVE,
	VMI_BYTECODE,
	VMI_COMPILED
} vmInterpret_t;

typedef enum {
	CA_UNINITIALIZED,
	CA_DISCONNECTED,
	CA_AUTHORIZING,
	CA_CONNECTING,
	CA_CHALLENGING,
	CA_CONNECTED,
	CA_LOADING,
	CA_PRIMED,
	CA_ACTIVE,
	CA_CINEMATIC
} connstate_t;

#define KEY_MWHEELUP 137
#define KEY_MWHEELDOWN 139

#define DOWNSUCCESS 1
#define DOWNPROC 0
#define DOWNFAIL 3
#define DOWNENDNOW 4

static float stdcol[] = {1.0, 1.0, 1.0, 1.0};
static float blackcol[] = {0.0, 0.0, 0.0, 0.5};

void Com_sprintf( char *dest, int size, const char *fmt, ...);
int Q_strncmp (const char *s1, const char *s2, int n);
void Q_strncpyz( char *dest, const char *src, int destsize );
void Q_strcat(char *dest, int size, const char *src);
int Q_stricmpn (const char *s1, const char *s2, int n);
char *Info_ValueForKey(const char *s, const char *key);
int Q_stricmp (const char *s1, const char *s2);
char *va( char *format, ... );
unsigned int hash(const void * key, int len, unsigned int seed);
int send_recv(int sd, const char *in, int inlen, char *out, int outlen, sockaddr_in *peer);
int timeout(int sock);
void StartDownloadThread(void *startAddress);
void EndDownloadThread();
void *InitMutex();
void DeleteMutex(void **mutex);
void LockMutex(void *mutex);
void UnlockMutex(void *mutex);
size_t filewriter(void *ptr, size_t size, size_t nmemb, FILE *stream);
void UI_ReadableSize(char *buf, int bufsize, int value);
int SCR_Strlen( const char *str );
void PrintTime(char *buf, int bufsize, uint time);
void EncryptMessage(char *in, char *out, uint outlen, uint key);

#endif /* COMMON_H */
