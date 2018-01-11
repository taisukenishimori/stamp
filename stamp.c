#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h> /* isdigit */
#include <string.h>
#include <windows.h>

char *encode_digits( char *src , size_t size );

int main( int argc, char **argv )
{
	struct tm *tmp_time;
	time_t t;

	char enc[14] = "\0";
	char dest[14] = "\0";
	char buf[7] = "\0";
	char buf2[7] = "\0";
	char *ptr;

	
	HANDLE hData;            /* handle to clip data */
	char *clipbuf = NULL;    /* pointer to clip data */
	char *convbuf = NULL;
	int convlen = 0;
	int encode_year = 0;


	while ( ( argc > 1 ) && ( argv[1][0] == '-' ) ) {
		switch ( argv[1][1] ) {
		case 'e':
			encode_year = 1;
			break;
		default:
			break;
		}
		argv++;
		argc--;
	} /* while */


	time( &t );
	tmp_time = localtime( &t );


	memset( buf, 0, sizeof( buf ) );
	memset( buf2, 0, sizeof( buf2 ) );
	memset( dest, 0, sizeof( dest ) );


	sprintf( buf, "%02d%02d%02d", \
			 (tmp_time->tm_year)%100, \
			 tmp_time->tm_mon + 1, \
			 tmp_time->tm_mday );

	if ( encode_year == 1 ) {
		ptr = buf;
		while ( *ptr != '\0' ) {
			enc[0] = *ptr++;
			if ( isdigit( enc[0] ) )
				sprintf( enc, "%d", ( 10 - atoi( enc ) )%10 ); /* the point of this program*/
			strcat( dest, enc );
		}
	}
	if ( encode_year != 1 ) {
		strcat( dest, buf );
	}
		
		/*		encode_digits( buf, sizeof( buf ) );*/
	
	sprintf( buf2, "-%02d%02d%02d", \
			 tmp_time->tm_hour, \
			 tmp_time->tm_min, \
			 tmp_time->tm_sec );

	strcat( dest, buf2 );
	
	printf( "%s", dest );
/*	printf( "\nstrlen = %d\n", strlen( dest ) ); */

	convbuf = dest;
	convlen = strlen( dest );

	if (!(hData = GlobalAlloc(GMEM_MOVEABLE, convlen + 2))) {
      fprintf(stderr, "Couldn't allocate global buffer for write.\n");
      free(convbuf);
      return 1;
    }
    if (!(clipbuf = (char*) GlobalLock(hData))) {
      fprintf(stderr, "Couldn't lock global buffer.\n");
      free(convbuf);
      return 1;
    }
    
    memcpy(clipbuf, convbuf, convlen);
    *(clipbuf + convlen) = '\0';
    *(clipbuf + convlen + 1) = '\0';

    GlobalUnlock (hData);


    OpenClipboard(0);

	EmptyClipboard();

	if (! SetClipboardData (CF_TEXT, hData)) {
      fprintf(stderr, "Couldn't write CF_TEXT format to the clipboard.\n");
      free(convbuf);
      return 1;
    }
    CloseClipboard();    
    if (GlobalFree(hData)) { 
      fprintf(stderr, "Couldn't free global buffer after write to clipboard.\n");
      free(convbuf);
      return 1;
    }
    hData = NULL;
    clipbuf = NULL;

	return 0;
}

/*
convert numbers
0 -> 0, 1 -> 9, 2 -> 8, 3 -> 7, 4 -> 6, 5 -> 5
6 -> 4, 7 -> 3, 8 -> 2, 9 -> 1, others -> no encode
*/  
char *encode_digits( char *src , size_t size )
{
	char buf[2] = "\0";
	char *dest;

	dest = malloc( size );
	
	while ( *src != '\0' ) {
		buf[0] = *src++;
		if ( isdigit( buf[0] ) )
			sprintf( buf, "%d", ( 10 - atoi( buf ) )%10 ); /* the point of this program*/
		strcat( dest, buf );
	}
	return dest;
	free( dest );
}
