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

static FuncAddresses addresses[] =
{
	/* JK2MP104EXE JK2MP102EXE  ASPYRMAC   JK2MP104EXEHL JK2MP102EXEHL ASPYRMACHL  */
	{  0x00401470, 0x00401470, 0x00004762,     0x06,         0x06,       0x05,     }, // CL_GETSERVERCOMMAND
	{  0x00428170, 0x00427870, 0x000DA1B2,     0x00,         0x00,       0x00,     }, // CMD_TOKENIZESTRING
	{  0x00428000, 0x00427700, 0x000DA196,     0x00,         0x00,       0x00,     }, // CMD_ARGV
	{  0x0040A0D0, 0x00409AE0, 0x0001006F,     0x00,         0x00,       0x00,     }, // CL_ADDRELIABLECOMMAND
	{  0x0040B4D0, 0x0040AF10, 0x00010996,     0x06,         0x06,       0x09,     }, // CL_SENDPURECHECKSUMS
	{  0x0042B190, 0x0042A870, 0x00029DB7,     0x00,         0x00,       0x00,     }, // CVAR_GET
	{  0x0042B3D0, 0x0042AAB0, 0x0002A107,     0x05,         0x05,       0x05,     }, // CVAR_SET2
	{  0x0043EBA0, 0x0043E270, 0x000D068F,     0x08,         0x08,       0x05,     }, // VM_CREATE
	{  0x0042D660, 0x0042CCF0, 0x000D834E,     0x05,         0x05,       0x05,     }, // FS_READFILE
	{  0x00848960, 0x00841348, 0x001360B8,     0x00,         0x00,       0x00,     }, // CLC_LASTEXECUTEDSVCMD
	{  0x00848964, 0x0084134C, 0x001360B8,     0x00,         0x00,       0x00,     }, // CLC_SERVERCOMMANDS
	{  0x00871024, 0x00869A0C, 0x00136094,     0x00,         0x00,       0x00,     }, // KEY_CATCHUP
	{  0x0082852C, 0x00820F14, 0x0089A2AC,     0x00,         0x00,       0x00,     }, // CURR_SERVERADDR
	{  0x0040F730, 0x0040F010, 0x00015C57,     0x00,         0x00,       0x00,     }, // SCR_DRAWSTRINGEXT
	{  0x0040B8D0, 0x0040B310, 0x0001158C,     0x05,         0x05,       0x0A,     }, // CL_INITDOWNLOADS
	{  0x00871020, 0x00869A08, 0x008E33A0,     0x00,         0x00,       0x00,     }, // CLS_STATE
	{  0x0040B790, 0x0040B1D0, 0x000107EA,     0x05,         0x05,       0x05,     }, // CL_BEGINDOWNLOAD
	{  0x00405E40, 0x00405D70, 0x0000A39D,     0x05,         0x05,       0x05,     }, // CON_CHECKRESIZE
	{  0x0040B830, 0x0040B270, 0x000114CE,     0x00,         0x00,       0x00,     }, // CL_NEXTDOWNLOAD
	{  0x00871144, 0x00869B2C, 0x008E37C4,     0x00,         0x00,       0x00,     }, // CLS_REALTIME
	{  0x009E87E0, 0x009D9E40, 0x0094B280,     0x00,         0x00,       0x00,     }, // PTR_REFEXPORT
	{  0x008D8BD8, 0x008CA3C0, 0x0094B258,     0x00,         0x00,       0x00,     }, // CLS_WHITESHADER
	{  0x00409C50, 0x00409690, 0x0000E1BC,     0x06,         0x06,       0x05,     }, // CL_KEYEVENT
	{  0x00827200, 0x0081FCB8, 0x007A9A18,     0x00,         0x00,       0x00,     }, // Q_KEYS
	{  0x00427CF0, 0x004273F0, 0x000DA728,     0x06,         0x06,       0x05,     }, // CBUF_INSERTTEXT
	{  0x0040DB00, 0x0040D3F0, 0x00000000,     0x00,         0x00,       0x00,     }, // CL_SERVERSTATUS
	{  0x004285F0, 0x00427CF0, 0x000275E3,     0x00,         0x00,       0x00,     }, // COM_PRINTF
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
		return addresses[name].JKMP104EXEHL;
	else if (ver == JK2MP102EXE)
		return addresses[name].JKMP102EXEHL;
	else if ( ver == ASPYRMAC )
		return addresses[name].ASPRMACHL;
	else
		return NULL;
}

void CheckHooks(GVER ver, uint moduleStart, uint moduleLenght, bool *badPtr)
{
	if ( !moduleStart || !moduleLenght )
		return;

	for ( int i = 0; i < sizeof(addresses) / (sizeof(addresses[0])); i++ )
	{
		if ((ver == JK2MP102EXE && addresses[i].JKMP102EXEHL == 0) || (ver == JK2MP104EXE && addresses[i].JKMP104EXEHL == 0))
			continue;

		byte *instrPtr;
		if ( ver == JK2MP102EXE )
			instrPtr = (byte*)addresses[i].JK2MP102EXE;
		else
			instrPtr = (byte*)addresses[i].JK2MP104EXE;
		
		// First byte has to be 0xE9 ( ljmp )
		if ( instrPtr[0] != 0xE9 )
			*badPtr = true;

		// Byte 1 - 4 has to be in the range of our module (in C this is a unsigned int)
		uint *jmpLoc = (uint*)&instrPtr[1];
		if ( *jmpLoc > moduleStart || moduleStart + moduleLenght < *jmpLoc )
			*badPtr = true;
	}
}

// ==============
// Detours
//
// Detours for Windows, Linux and MacOSX
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
