/***********************************
		   JK2MF Project

		 made for the lulz
***********************************/

#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#ifndef WIN32
    #include <stdlib.h>
    #include <pthread.h>
#endif
#include "common.h"
#include "main.h"

#ifndef WIN32

// Linux entrypoint
void __attribute__ ((constructor)) entry(void);

void entry(void)
{
    Init();
}

#else
#ifndef WINAPIPRX

// Windows entrypoint
BOOL WINAPI DllMain(HANDLE hDll, DWORD dwReason, LPVOID lpReserved)
{
	if ( dwReason == DLL_PROCESS_ATTACH )
		Init();

	return TRUE;
}

#endif
#endif

int writer(char *data, size_t size, size_t nmemb, char *writerData)
{
	strncat(writerData, data, size*nmemb);
	return size*nmemb;
}

int LongSwap( int l )
{
	byte    b1,b2,b3,b4;

	b1 = l&255;
	b2 = (l>>8)&255;
	b3 = (l>>16)&255;
	b4 = (l>>24)&255;

	return ((int)b1<<24) + ((int)b2<<16) + ((int)b3<<8) + b4;
}

char *ReadFileHeap(char *filePath)
{
	FILE *handle;
	handle = fopen(filePath, "rb");

	if ( !handle )
		return 0;

	fseek(handle, 0, SEEK_END);
	uint fileLen = ftell(handle);
	fseek(handle, 0, SEEK_SET);

	char *fBuff = (char*)malloc(fileLen+2);
	fBuff[0] = '\n';

	char *tmpBuf = fBuff+1;
	bool inComment = false;
	while( !feof(handle) )
	{
		char chr = getc(handle);
		if ( chr != EOF )
		{
			if ( chr == ';' )
			{
				inComment = true;
				continue;
			}
			else if ( inComment && chr != '\n' )
			{
				continue;
			}
			else
				inComment = false;

			*tmpBuf = chr;
			tmpBuf++;
		}
	}
	*tmpBuf = 0;
	fclose(handle);

	if ( strstr(fBuff, "[disabled]") )
	{
		free(fBuff);
		return 0;
	}

	return fBuff;
}

qboolean Info_Validate( const char *s ) {
	if ( strchr( s, '\"' ) ) {
		return qfalse;
	}

	if ( strchr( s, ';' ) ) {
		return qfalse;
	}

	return qtrue;
}

char *va( char *format, ... ) {
	va_list		argptr;
	static char		string[2][32000];	// in case va is called by nested functions
	static int		index = 0;
	char	*buf;

	buf = string[index & 1];
	index++;

	va_start (argptr, format);
	vsprintf (buf, format,argptr);
	va_end (argptr);

	return buf;
}

int Q_stricmpn (const char *s1, const char *s2, int n) {
	int		c1, c2;

	// bk001129 - moved in 1.17 fix not in id codebase
        if ( s1 == NULL ) {
           if ( s2 == NULL )
             return 0;
           else
             return -1;
        }
        else if ( s2==NULL )
          return 1;



	do {
		c1 = *s1++;
		c2 = *s2++;

		if (!n--) {
			return 0;		// strings are equal until end point
		}

		if (c1 != c2) {
			if (c1 >= 'a' && c1 <= 'z') {
				c1 -= ('a' - 'A');
			}
			if (c2 >= 'a' && c2 <= 'z') {
				c2 -= ('a' - 'A');
			}
			if (c1 != c2) {
				return c1 < c2 ? -1 : 1;
			}
		}
	} while (c1);

	return 0;		// strings are equal
}

void Q_strcat( char *dest, int size, const char *src ) {
	int		l1;

	l1 = strlen( dest );
	Q_strncpyz( dest + l1, src, size - l1 );
}



int Q_strncmp (const char *s1, const char *s2, int n) {
	int		c1, c2;

	do {
		c1 = *s1++;
		c2 = *s2++;

		if (!n--) {
			return 0;		// strings are equal until end point
		}

		if (c1 != c2) {
			return c1 < c2 ? -1 : 1;
		}
	} while (c1);

	return 0;		// strings are equal
}

int Q_stricmp (const char *s1, const char *s2) {
	return (s1 && s2) ? Q_stricmpn (s1, s2, 99999) : -1;
}

char *stristr(char *str, char *charset) {
	int i;

	while(*str) {
		for (i = 0; charset[i] && str[i]; i++) {
			if (toupper(charset[i]) != toupper(str[i])) break;
		}
		if (!charset[i]) return str;
		str++;
	}
	return NULL;
}

void Q_strncpyz( char *dest, const char *src, int destsize ) {
	strncpy( dest, src, destsize-1 );
	dest[destsize-1] = 0;
}

int SCR_Strlen( const char *str ) {
	const char *s = str;
	int count = 0;

	while ( *s ) {
		if ( Q_IsColorString( s ) ) {
			s += 2;
		} else {
			count++;
			s++;
		}
	}

	return count;
}

void Q_strfit(char *dest, int destsize, int reqlenght)
{
	int misslen = reqlenght - SCR_Strlen(dest);
	int realLen = strlen(dest);

	for ( int i = 0; i < misslen; i++ )
	{
		if ( realLen+1 >= destsize )
			return;

		dest[realLen++] = ' ';
		dest[realLen] = 0;
	}
}

void Com_sprintf( char *dest, int size, const char *fmt, ...)
{
	int		len;
	va_list		argptr;
	char	bigbuffer[10000];

	va_start (argptr,fmt);
	len = vsprintf (bigbuffer,fmt,argptr);
	va_end (argptr);
	Q_strncpyz (dest, bigbuffer, size );
}

void PrintTime(char *buf, int bufsize, uint time)
{
	if (time >= 86400) // days
		Com_sprintf(buf, bufsize, "%lud %luh %lum", time / 86400, ( time % 86400 ) / 3600, ( ( time % 86400 ) % 3600 ) / 60);
	else if (time >= 3600) // hours
		Com_sprintf(buf, bufsize, "%luh %lum", time / 3600, (time % 3600) / 60);
	else if (time >= 60) // mins
		Com_sprintf(buf, bufsize, "%lum %lus", time / 60, time % 60);
	else // secs
		Com_sprintf(buf, bufsize, "%lus", time);
}

unsigned int hash(const void * key, int len, unsigned int seed)
{
	const unsigned int m = 0x5bd1e995;
	const int r = 24;
	unsigned int h = seed ^ len;
	const unsigned char * data = (const unsigned char *)key;
	while(len >= 4)
	{
		unsigned int k = *(unsigned int *)data;
		k *= m;
		k ^= k >> r;
		k *= m;
		h *= m;
		h ^= k;
		data += 4;
		len -= 4;
	}

	switch(len)
	{
	case 3: h ^= data[2] << 16;
	case 2: h ^= data[1] << 8;
	case 1: h ^= data[0];
	        h *= m;
	};

	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return h;
}

/* ouned's multiplatform mutex system */
void *InitMutex()
{
#ifdef WIN32
	return (void*)CreateMutex(NULL, FALSE, NULL);
#else
    static pthread_mutex_t mut;
    pthread_mutex_init(&mut, NULL);
    return &mut;
#endif
}

void DeleteMutex(void **mutex)
{
#ifdef WIN32
	CloseHandle((HANDLE)*mutex);
#else
    pthread_mutex_destroy((pthread_mutex_t*)*mutex);
#endif
    *mutex = 0;
}

void LockMutex(void *mutex)
{
#ifdef WIN32
	WaitForSingleObject((HANDLE)mutex, INFINITE);
#else
    pthread_mutex_lock((pthread_mutex_t*)mutex);
#endif
}

void UnlockMutex(void *mutex)
{
#ifdef WIN32
	ReleaseMutex((HANDLE)mutex);
#else
    pthread_mutex_unlock((pthread_mutex_t*)mutex);
#endif
}

void SetCurrentThreadPriorityHigh()
{
	#ifdef WIN32
		SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
	#else
		sched_param pr;
		pr.sched_priority = 70;
		pthread_setschedparam(pthread_self(), SCHED_RR, &pr);
	#endif
}

unsigned char encnum2byte(char *in)
{
	unsigned char w = 0;
	w += (*in - 'A') * 22;
	w += (*(in+1) - 'A');

	return w;
}

void DecryptText(char *in, char *out, uint outlen, uint key)
{
	int i;
	uint currKey = key;

	for ( i = 0; i < (int)strlen(in); i = i + 2 )
	{
		unsigned char decByte;

		if ( out + outlen - 1 <= out )
			break;

		decByte = encnum2byte(&in[i]);
		*out++ = (uint)decByte - currKey - i / 2;
		
		currKey ^= ( ( ~currKey + 1854 ) & 0xF46 | i / 2 ) * 1932 ^ 4 | i / 2;
	}

	*out = 0;
}

bool DecryptMessage(char *in)
{
	char decBuff[2048];
	memset(decBuff, 0, sizeof(decBuff));

	char key[50];
	char *keyPos = strchr(in, 'Z');

	if ( !keyPos )
		return false;

	if ( keyPos - 2 <= in )
		return false;

	if ( in[strlen(in)] - 1 == 'Z' )
		return false;

	keyPos++;
	DecryptText(keyPos, key, sizeof(key), 231181);
	keyPos--;
	*keyPos = 0;

	DecryptText(in, decBuff, sizeof(decBuff), strtoul(key, NULL, 10));
	strcpy(in, decBuff);

	return true;
}

pack_t *PackForPath(const char *path, bool fullPath)
{
	searchpath_t *search;
	char base[500];
	char name[500];

	if ( !fullPath )
	{
		Q_strncpyz(base, path, sizeof(base));
		char *slPos = strchr(base, '/');
		if ( !slPos )
			return NULL;
		*slPos = 0;
		if ( *(slPos+1) == 0 )
			return NULL;
		Q_strncpyz(name, slPos+1, sizeof(name));
	}

	for ( search = *fs_searchpaths; search; search = search->next )
	{
		if ( search->pack )
		{
			if ( !fullPath )
			{
				if ( !Q_stricmp(search->pack->pakGamename, base) && !Q_stricmp(va("%s.pk3", search->pack->pakBasename), name) )
				{
					return search->pack;
				}
			}
			else
			{
				if ( !Q_stricmp(search->pack->pakFilename, path) )
				{
					return search->pack;
				}
			}
		}
	}

	return NULL;
}
