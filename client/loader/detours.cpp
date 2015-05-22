/***********************************
		   JK2MF Project

		 made for the lulz
***********************************/

#ifdef WIN32
    #include <windows.h>
#else
    #include <unistd.h>
    #include <sys/mman.h>
#endif
#include <stdlib.h>
#include <string.h>
#include "detours.h"

FuncAddresses addresses[] =
{
	/* JKMP104EXE   JK2MP102EXE   ASPYRMAC    JKMP104EXEHL JKMP102EXEHL ASPYRMACHL */
	{ 0x004285F0,   0x00427CF0,  0x000275E3,      0x00,       0x00,       0x00,    }, // COM_PRINTF
	{ 0x0042B190,   0x0042A870,  0x00029DB7,      0x00,       0x00,       0x00,    }, // CVAR_GET
	{ 0x00444F40,   0x004445B0,  0x00029381,      0x05,       0x05,       0x07,    }, // COM_INIT
	{ 0x0040B160,   0x0040ABA0,  0x00014035,      0x08,       0x08,       0x06,    }, // CL_CONNECT
	{ 0x0040FCF0,   0x0040F5B0,  0x00015E9B,      0x05,       0x05,       0x06,    }, // SCR_DRAWSCREENFIELD
	{ 0x0040F730,   0x0040F010,  0x00015C57,      0x00,       0x00,       0x00,    }, // SCR_DRAWSTRINGEXT
	{ 0x00435350,   0x00434970,  0x000DDA6E,      0x00,       0x00,       0x00,    }, // INFO_VALUEFORKEY
	{ 0x0040AC10,   0x0040A660,  0x0001345A,      0x05,       0x05,       0x06,    }, // CL_DISCONNECT
	{ 0x0082852C,   0x00820F14,  0x0089A2AC,      0x00,       0x00,       0x00,    }, // CURR_SERVERADDR
	{ 0x00871020,   0x00869A08,  0x008E33A0,      0x00,       0x00,       0x00,    }, // CLS_STATE
	{ 0x00000000,   0x00000000,  0x00027F87,      0x00,       0x00,       0x07,    }, // COM_ERROR
};

ADDR GetAddr(GVER ver, FUNC name)
{
	if ( ver == JK2MP104EXE )
		return addresses[name].JK2MP104EXE;
	else if (ver == JK2MP102EXE)
		return addresses[name].JK2MP102EXE;
	else if ( ver == ASPYRMAC )
		return addresses[name].ASPYRMAC;
	else
		return NULL;
}

FUNC Gethl(GVER ver, FUNC name)
{
	if ( ver == JK2MP104EXE )
		return addresses[name].JK2MP104EXEHL;
	else if (ver == JK2MP102EXE)
		return addresses[name].JK2MP102EXEHL;
	else if ( ver == ASPYRMAC )
		return addresses[name].ASPRMACHL;
	else
		return NULL;
}

// ==============
// Detours
//
// Detours for windows and linux
// ==============
#ifdef WIN32

#define GetPage( a ) ((void*)(((unsigned long)a)&0xfffff000))
#define UnprotectMemory( addr, len ) ( VirtualProtect( addr, len, PAGE_EXECUTE_READWRITE, &oldprot ) )

void *Detour( void *orig, void *det, int size )
{
	unsigned char *tramp;
	DWORD   oldprot = 0;
    
	if( size < 5 ) {
		return 0;
	}
    
	tramp = (unsigned char *)malloc( size + 5 );
	memcpy( tramp, orig, size );
	tramp[ size ] = 0xE9;
	*( (void **)( tramp + size + 1 ) ) = (void *)( ( ( (uint)orig ) + size ) - (uint)( tramp + size + 5 ) );
    
    UnprotectMemory( GetPage( orig ), 4096 );
    *( (unsigned char *)orig ) = 0xE9;
    *( (void **)( (uint)orig + 1 ) ) = (void *)( ( (uint)det ) - ( ( (uint)orig ) + 5 ) );
    
    return tramp;
}

#else

void *Detour( void *orig, void *det, int size )
{
	unsigned long mask = ~(sysconf(_SC_PAGESIZE)-1);
	unsigned long page = (unsigned long)orig & mask;
	unsigned long jmprel;
	unsigned char *gateway;
	unsigned char *p;
    
	mprotect( (void*)page, 5, ( PROT_READ | PROT_WRITE | PROT_EXEC ) );
    
	jmprel = (unsigned long)det - (unsigned long)orig - 5;
    
	gateway = (unsigned char*)malloc( size + 5 );
	memcpy( gateway, orig, size );
	gateway[size] = 0xE9;
	*(unsigned long*)( gateway + size + 1 ) = (unsigned long)( (uint)orig + size ) - (unsigned long)( gateway + size ) - 5;
    
	p = (unsigned char*)orig;
	p[0] = 0xE9;
	*(unsigned long*)( p + 1 ) = jmprel;
    
	return gateway;
}

#endif
