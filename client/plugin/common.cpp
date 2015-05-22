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
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdio.h>
#include "common.h"
#include "main.h"

#ifdef WIN32

// Windows DLL entrypoint
BOOL WINAPI DllMain(HANDLE hDll, DWORD dwReason, LPVOID lpReserved)
{
	if ( dwReason == DLL_PROCESS_ATTACH )
		Init();

	if ( dwReason == DLL_PROCESS_DETACH )
		Shutdown();
    
	return TRUE;
}

#else

void __attribute__ ((constructor)) entry(void);
void __attribute__ ((destructor)) unload(void);

void entry(void)
{
    Init();
}
void unload(void)
{
    Shutdown();
}

#endif

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

void Q_strncpyz( char *dest, const char *src, int destsize ) {
	strncpy( dest, src, destsize-1 );
	dest[destsize-1] = 0;
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

void Q_strcat( char *dest, int size, const char *src ) {
	int		l1;

	l1 = strlen( dest );
	if ( l1 >= size ) {
		return;
	}
	Q_strncpyz( dest + l1, src, size - l1 );
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

char *Info_ValueForKey( const char *s, const char *key ) {
	char	pkey[BIG_INFO_KEY];
	static	char value[2][BIG_INFO_VALUE];
	static	int	valueindex = 0;
	char	*o;
	
	if ( !s || !key ) {
		return "";
	}

	valueindex ^= 1;
	if (*s == '\\')
		s++;
	while (1)
	{
		o = pkey;
		while (*s != '\\')
		{
			if (!*s)
				return "";
			*o++ = *s++;
		}
		*o = 0;
		s++;

		o = value[valueindex];

		while (*s != '\\' && *s)
		{
			*o++ = *s++;
		}
		*o = 0;

		if (!Q_stricmp (key, pkey) )
			return value[valueindex];

		if (!*s)
			break;
		s++;
	}

	return "";
}

int Q_stricmp (const char *s1, const char *s2) {
	return (s1 && s2) ? Q_stricmpn (s1, s2, 99999) : -1;
}

size_t filewriter(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
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

void UI_ReadableSize(char *buf, int bufsize, int value)
{
	if (value > 1024*1024*1024 ) { // gigs
		Com_sprintf( buf, bufsize, "%d", value / (1024*1024*1024) );
		Com_sprintf( buf+strlen(buf), bufsize-strlen(buf), ".%02d GB", 
			(value % (1024*1024*1024))*100 / (1024*1024*1024) );
	} else if (value > 1024*1024 ) { // megs
		Com_sprintf( buf, bufsize, "%d", value / (1024*1024) );
		Com_sprintf( buf+strlen(buf), bufsize-strlen(buf), ".%02d MB", 
			(value % (1024*1024))*100 / (1024*1024) );
	} else if (value > 1024 ) { // kilos
		Com_sprintf( buf, bufsize, "%d KB", value / 1024 );
	} else { // bytes
		Com_sprintf( buf, bufsize, "%d bytes", value );
	}
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

int send_recv(int sd, const char *in, int inlen, char *out, int outlen, sockaddr_in *peer)
{
	int retry = 0;

	for(retry = 2; retry > 0; retry--) 
	{
    	sendto(sd, in, inlen, 0, (sockaddr*)peer, sizeof(sockaddr_in));
		if( !timeout(sd) )
			break;
	}
	if ( !retry )
		return 0;
	
	outlen = recvfrom(sd, out, outlen, 0, NULL, NULL);
	out[outlen] = 0;

	return outlen;
}

int timeout(int sock)
{
	struct  timeval tout;
	fd_set  fd_read;
	int 	err;

	tout.tv_sec = 0;
	tout.tv_usec = 700000;
	FD_ZERO(&fd_read);
	FD_SET(sock, &fd_read);
	err = select(sock + 1, &fd_read, NULL, NULL, &tout);
	if(!err) return(-1);
	return(0);
}

void StartDownloadThread(void *startAddress)
{
#ifdef WIN32
	_beginthread((void(*)(void*))startAddress, 0, NULL);
#else
    pthread_t th;
    pthread_create(&th, NULL, (void*(*)(void*))startAddress, NULL);
#endif
}

void EndDownloadThread()
{
#ifdef WIN32
	_endthread();
#else
    pthread_exit(NULL);
#endif
}

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

void byte2encnum(unsigned char in, char *out)
{
	int r = in % 22;
	*(out+1) = 'A' + r;
	in /= 22;
	r = in % 22;
	*out = 'A' + r;
}

void EncryptText(char *in, char *out, uint outlen, uint key)
{
	uint i;
	uint currKey = key;

	for ( i = 0; i < strlen(in); i++ )
	{
		unsigned char outByte;

		if ( out + outlen - 1 <= out )
			break;

		outByte = (uint)in[i] + currKey + i;
		byte2encnum(outByte, out);
		out += 2;
		
		currKey ^= ( ( ~currKey + 1854 ) & 0xF46 | i ) * 1932 ^ 4 | i;
	}

	*out = 0;
}

void EncryptMessage(char *in, char *out, uint outlen, uint key)
{
	EncryptText(in, out, outlen, key);

	char inPass[50];
	sprintf(inPass, "%u", key);
	char outPass[50];
	EncryptText(inPass, outPass, sizeof(outPass), 231181);

	strcat(out, "Z");
	strcat(out, outPass);
}
