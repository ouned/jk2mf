/***********************************
		   JK2MF Project

		 made for the lulz
***********************************/

#ifdef __APPLE__
    #include <unistd.h>
#endif
#include "common.h"
#include "main.h"

#ifdef __APPLE__

void __attribute__ ((constructor)) entry(void);

void entry(void)
{
    Init();
}

#endif

bool FileExists(char *file)
{
	if (access(file, 0) == 0)
	{
		return true;
	}
	else return false;
}

size_t filewriter(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
}

int writer(char *data, size_t size, size_t nmemb, char *writerData)
{
	strncat(writerData, data, size*nmemb);
	return size*nmemb;
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

char *va( const char *format, ... ) {
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
