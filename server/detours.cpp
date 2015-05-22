/***********************************
		   JK2MF Project

		 made for the lulz
***********************************/

#ifndef WIN32
    #include <unistd.h>
    #include <sys/mman.h>
#endif
#include <stdlib.h>
#include <string.h>
#include "detours.h"

FuncAddresses addresses[] =
{
	/* JKDEDEXE    JKMPEXE     JKLINUX     JKDEDEXEHL	JKMPEXEHL	JKLINUXHL */
	{ 0x0041CEA0, 0x0043EF60, 0x0807CBD4,  0x08,		0x08,		0x09	  }, // VM_CALL
	{ 0x00408770, 0x004285F0, 0x08064624,  0x00,		0x00,		0x00	  }, // COM_PRINTF
	{ 0x00410CC0, 0x00431FD0, 0x0806F824,  0x07,		0x07,		0x06	  }, // MSG_READSTRINGLINE
	{ 0x00410BE0, 0x00431EA0, 0x0806F614,  0x00,		0x00,		0x00	  }, // MSG_READBYTE
	{ 0x0042D400, 0x0044DC50, 0x08052BA4,  0x00,		0x00,		0x00	  }, // SV_GETUSERINFO
	{ 0x004133E0, 0x004356B0, 0x08076A94,  0x00,		0x00,		0x00	  }, // INFO_SETVALUEFORKEY
	{ 0x00413100, 0x00435350, 0x08076544,  0x00,		0x00,		0x00	  }, // INFO_VALUEFORKEY
	{ 0x0042D380, 0x0044DBD0, 0x08052B04,  0x00,		0x00,		0x00	  }, // SV_SETUSERINFO
	{ 0x0040B220, 0x0042B190, 0x08067F34,  0x00,		0x00,		0x00	  }, // CVAR_GET
	{ 0x0042A730, 0x0044AF80, 0x0804F744,  0x07,		0x07,		0x06	  }, // SV_GAMESENDSERVERCOMMAND
	{ 0x005A75D4, 0x00B1C00C, 0x082A56A4,  0x00,		0x00,		0x00	  }, // SVS_TIME
	{ 0x00408240, 0x00427FF0, 0x08063E54,  0x00,		0x00,		0x00	  }, // CMD_ARGC
	{ 0x00408250, 0x00428000, 0x08063E64,  0x00,		0x00,		0x00	  }, // CMD_ARGV
	{ 0x0042AA30, 0x0044B280, 0x0804FB54,  0x08,		0x08,		0x06	  }, // SV_LOCATEGAMEDATA
	{ 0x00430350, 0x00450BA0, 0x08056E44,  0x00,		0x00,		0x00	  }, // SV_UNLINKENTITY
	{ 0x0041CE30, 0x0043EEF0, 0x0807CB54,  0x00,		0x00,		0x00	  }, // VM_ARGPTR
	{ 0x004297F0, 0x0044A080, 0x0804CE74,  0x05,		0x05,		0x06	  }, // SV_BEGINDOWNLOAD
	{ 0x0042EAD0, 0x0044F340, 0x08054A64,  0x05,		0x05,		0x09	  }, // SVC_REMOTECOMMAND
	{ 0x006140DC, 0x00B88B14, 0x0811F244,  0x00,		0x00,		0x00	  }, // LAST_RCONTIME
	{ 0x0042A0F0, 0x0044A960, 0x0804DA44,  0x06,		0x06,		0x06	  }, // SV_UPDATEUSERINFO
	{ 0x005A75DC, 0x00B1C014, 0x082A56AC,  0x00,		0x00,		0x00	  }, // SVS_CLIENTS
	{ 0x00429F60, 0x0044A7D0, 0x0804D874,  0x00,		0x00,		0x00	  }, // SV_USERINFOCHANGED
	{ 0x006140D8, 0x00B88B10, 0x0811F248,  0x00,		0x00,		0x00	  }, // G_VM
	{ 0x0041CE60, 0x0043EF20, 0x0807CB94,  0x06,		0x06,		0x06	  }, // VM_EXPLICITARGPTR
	{ 0x0042D290, 0x0044DAD0, 0x080529A4,  0x00,		0x00,		0x00	  }, // SV_GETCONFIGSTRING
	{ 0x0042A150, 0x0044A9B0, 0x0804DAB4,  0x05,		0x05,		0x06	  }, // SV_EXECUTECLIENTCOMMAND
	{ 0x00410C40, 0x00431F00, 0x0806F694,  0x00,		0x00,		0x00	  }, // MSG_READLONG
	{ 0x00410C60, 0x00431F20, 0x0806F704,  0x00,		0x00,		0x00	  }, // MSG_READSTRING
	{ 0x00408320, 0x00428170, 0x08064054,  0x00,		0x00,		0x00	  }, // CMD_TOKENIZESTRING
	{ 0x0042AA70, 0x0044B2C0, 0x0804FB94,  0x05,		0x05,		0x06	  }, // SV_GETUSERCMD
	{ 0x0042ECC0, 0x0044F540, 0x08054CB4,  0x06,		0x06,		0x06	  }, // SV_CONNECTIONLESSPACKET
	{ 0x00412010, 0x00433B20, 0x08072934,  0x00,		0x00,		0x00	  }, // NET_OUTOFBANDPRINT
	{ 0x00426A20, 0x00447130, 0x0804A304,  0x05,		0x05,		0x06	  }, // SV_BOTALLOCATECLIENT
	{ 0x0040DB20, 0x0042DE20, 0x0806BF84,  0x00,		0x00,		0x00	  }, // FS_GETFILELIST
	{ 0x00408900, 0x00428780, 0x080647F4,  0x05,		0x05,		0x09	  }, // COM_DPRINTF
	{ 0x0040E670, 0x0042ECD0, 0x0806DC54,  0x07,		0x07,		0x06	  }, // FS_REFPAKNAMES
	{ 0x005577A4, 0x00AD2BF4, 0x081D3140,  0x00,		0x00,		0x00	  }, // FS_SEARCHPATH
	{ 0x0040E5E0, 0x0042EB00, 0x0806D9E4,  0x07,		0x07,		0x06	  }, // FS_REFPAKCHKSUM
	{ 0x0041C970, 0x0043EA20, 0x0807C444,  0x00,		0x00,		0x00      }, // VM_RESTART
	{ 0x00475E38, 0x00504080, 0x08100D44,  0x00,		0x00,		0x00      }, // QVM_SRVNAME
	{ 0x0041CD90, 0x0043EE50, 0x0807CA84,  0x00,		0x00,		0x00      }, // VM_FREE
	{ 0x0041CAE0, 0x0043EBA0, 0x0807C674,  0x08,		0x08,		0x09      }, // VM_CREATE
	{ 0x0042AAC0, 0x0044B310, 0x0804FC44,  0x05,		0x05,		0x06      }, // GAME_SYSCALLS
	{ 0x0040B450, 0x0042B3D0, 0x080681A4,  0x05,        0x05,       0x06      }, // CVAR_SET2
};

ADDR GetAddr(GVER ver, FUNC name)
{
	if (ver == JK2MFDED)
		return (uint)gae(name);
	else if ( ver == JK2DEDEXE )
		return addresses[name].JK2DEDEXE;
	else if ( ver == JK2MPEXE )
		return addresses[name].JK2MPEXE;
	else
		return addresses[name].JK2DEDLINUX;
}

FUNC Gethl(GVER ver, FUNC name)
{
	if ( ver == JK2DEDEXE )
		return addresses[name].JKDEDEXEHL;
	else if ( ver == JK2MPEXE )
		return addresses[name].JKMPEXEHL;
	else
		return addresses[name].JKLINUXHL;
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
