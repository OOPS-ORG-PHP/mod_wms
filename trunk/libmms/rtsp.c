/* 
 * RTSP header check
 * $Id$
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <php.h>

#ifdef __CYGWIN__
typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef unsigned long long int uint64_t;
#else
#include <inttypes.h>
#endif

#include "client.h"

#define BUF_SIZE 2048

extern int verbose;
int rtspcode = 0;

uint32_t get_flag ( char *data, int offset, int byte ) {
	    uint32_t ret;
	        int      i;
		    int      m;

		        m = 8 * (byte - 1);

			    ret = data[offset] << m;

	for ( i=1; i<byte; i++ ) {
		m -= 8;
		ret |= data[offset + i] << m;
	}

	return ret;
}

int send_command (int s, char *host, char *uri) {
	char query[1024] = { 0, };
	char enstr[1024] = { 0, };
	char utfstr[1024] = { 0, };

	string_utf_iconv (utfstr, uri, strlen (uri), 8);
	URLEncode (enstr, utfstr);

	sprintf (query, "DESCRIBE rtsp://%s/%s RTSP/1.0\r\n"
					"User-Agent: WMPlayer/9.0.0.2980 guid/3300AD50-2C39-46C0-AE0A-F3ED6E6D7421\r\n"
					"Accept: application/sdp\r\n"
					"Accept-Charset: UTF-8, *;q=0.1\r\n"
					"X-Accept-Authentication: Negotiate, NTLM, Digest, Basic\r\n"
					"Accept-Language: ko-KR, *;q=0.1\r\n"
					"CSep: 1\r\n"
					"Supported: com.microsoft.wm.srvppair, com.microsoft.wm.sswitch, "
					"com.microsoft.wm.eosmsg, com.microsoft.wm.predstrm\r\n\r\n",
					host, enstr);

	if ( write (s, query, strlen (query) ) != strlen (query) ) {
		if ( verbose ) php_error (E_WARNING, "==> ERROR: write error. maybe closed prot 554.\n");
		close (s);
		exit (1);
	}

	if ( verbose )
		php_printf ("\nQUERY :\n%s\n\n", query);

	return strlen (query);
}

int get_code ( char * data ) {
	uint32_t s;

	rtspcode = rtsp_responce (data);
	s = get_flag (data, 9, 3);

	switch (s) {
		case 0x343034 :
			return FILE_NOT_FOUND;
		case 0x343135 :
			return UNSUPPORTED_MEDIA_TYPE;
		case 0x353030 :
			return INTERNAL_SERVER_ERROR;
	}

	if ( s > 0x340000 )
		return OTHER_ERROR;

	return NOMAL;
}

int rtsp_responce ( char * data ) {
	char code[4] = { 0, };
	int  ret;

	strncpy ( code, data + 9, 3 );
	ret = atoi (code);

	return ret;
}

int o_rtspcheck (char *url, int timeout, int debug) {
	int      s;
	char     str[1024] = { 0, };
	char     data[2048] = { 0, };
	int      len;
	char     host[256] = { 0, };
	char    *path, *file, *cp;
	int      opt, optlength = 0, result = 0;
	int      errcode = NOMAL;
	int      ret = NOMAL;

	verbose = debug ? debug : 0;
	rtspcode = 0;

	if ( strncmp (url, "rtsp://", 7) ) {
		if ( verbose )
			php_error (E_WARNING, "ERROR: invaild rtsp url\n\n");

		return INVALID_URL;
	}

	/* parse url */
	strncpy (host, url + 7, 255);
	cp = strchr (host, '/');
	*cp = 0;

	path = strchr (url + 7, '/') + 1;

	if ( verbose ) {
		php_printf ("host: >%s<\n", host);
		php_printf ("path: >%s<\n", path);
	}

	file = strrchr (url, '/');

	/* host connect */
	if ( (s = HOSTconnect (host, timeout, 554, &errcode)) < 0 ) {
		return errcode;
	}

	/* send describe query */
	len = send_command (s, host, path);

	/* get return code */
	memset (data, 0, sizeof (data));
	len = recv (s, data, 12, 0);

	ret = get_code (data);

	if ( verbose ) {
		uint32_t ofs;

		php_printf ("RETURN QYERY\n\n");
		php_printf ("%s", data);

		len = 1;
		while ( ofs != 0x0d0a && len > 0 ) {
			memset (data, 0, sizeof (data));
			len = recv (s, data, BUF_SIZE, 0);
			php_printf ("%s", data);
			ofs = get_flag (data, len - 2, 2);
		}
	}

	close (s);

	return ret;
}

int o_getrtspcode (void) {
	int x = 0;

	x = rtspcode;

	return x;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
