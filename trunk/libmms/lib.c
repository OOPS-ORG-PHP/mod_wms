/*
 * $Id: lib.c,v 1.1 2004-06-15 09:21:11 oops Exp $
 */

#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <iconv.h>
#include "client.h"

#include <sys/fcntl.h>
#include <netinet/tcp.h>

#include <php.h>

extern int verbose;

int string_utf_iconv (char *dest, char *src, int len, int type) {
	// int i;
	// int orig_len = len;
	// int utf16_len = len * 2 + 32;

#ifdef HAVE_LIBICONV
	const
#endif
	char* inbuf_p = src;
	char* outbuf_p = dest;

	size_t il = strlen(src) + 1;
	size_t ol = 1000;

	iconv_t cd;

	if ( type == 16 )
		cd = iconv_open ("UTF-16LE", "EUC-KR");
	else
		cd = iconv_open ("UTF-8", "EUC-KR");

	if ( !cd )
		return 1;

	if ( iconv (cd, &inbuf_p, &il, &outbuf_p, &ol) == -1 || il != 0)
		return 1;

	/*
	php_printf (stderr, "==> %s\n", src);
	for (i = 0; i < orig_len * 2; i++)
		php_printf ("[%02x] %c\n", dest[i], dest[i]);
	*/

	iconv_close(cd);
	return 0;
}

int is_anychar (char c) {
	/*
	 * 0-9 => 45-57
	 * a-z => 97-122
	 * A-Z => 65-90
	 * /   => 47
	 * .   => 46
	 * -   => 45
	 * =   => 61
	 */
	if ( c > 45 && c < 58 )
		return 0;

	if ( c == 61 )
		return 0;

	if ( c > 64 && c < 91 )
		return 0;

	if ( c > 96 && c < 123 )
		return 0;

	return 1;
}

void URLEncode (char *dst, char *src) {
	int   i = 0, j = 0;
	char *p = src;
	char  buf[5] = { 0, };

	if ( src == NULL || dst == NULL )
		return;

	while ( src[i] != 0 ) {
		if ( ! is_anychar (src[i]) )
			sprintf (buf, "%c", src[i]);
		else
			sprintf (buf, "%%%02x", src[i] & 0xff);

		memcpy (dst + j, buf, strlen(buf));
		j += strlen (buf);
		i++;
	}
}

int nonblock_func (int socket, int nonblock) {
	int flags;

	flags = fcntl (socket, F_GETFL, 0);

	if ( TRUE == nonblock )
		flags |= O_NONBLOCK;
	else
		flags &= ~O_NONBLOCK;

  return fcntl (socket, F_SETFL, flags);
}

/*
 * This code get check-utils package from BPL License
 * See Also http://cvs.oops.org/index.cgi/check-utils/BPL
 */
int HOSTconnect (char * addr, int timeout, int port, int *ecode) {

	int                  s ;
	struct sockaddr_in   sa;
	struct sockaddr_in   la;
	struct hostent      *hp;
	int                  len;
	int                  opt, optlength = 0, result = 0;
	struct timeval       timeouts;
	fd_set               c_fds, r_fds;
	int                  sval;
	socklen_t            slen;

	/* DNS lookup */
	if ((hp = gethostbyname(addr)) == NULL) {
		if ( verbose ) php_error (E_WARNING, "Host name lookup failure.\n");
		*ecode = HOSTS_NOT_FOUND;
		return -1;
	}

	timeouts.tv_sec = timeout;
	timeouts.tv_usec = 0;

	setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, &timeouts, sizeof(timeouts));
	setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &timeouts, sizeof(timeouts));
	setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, NULL, 0);
	setsockopt(s, SOL_SOCKET, SO_REUSEADDR, NULL, 0);
	setsockopt(s, IPPROTO_TCP, TCP_NODELAY, NULL, 0);

	/* fill socket structure */

	bcopy ((char *) hp->h_addr, (char *) &sa.sin_addr, hp->h_length);
	sa.sin_family = hp->h_addrtype;
	/*sa.sin_port = 0x5000;*/ /* http port (80 => 50 Hex, switch Hi-/Lo-Word ) */

	sa.sin_port = htons(port) ; /* be2me_16(1755);  mms port5 */

	if (verbose) php_printf ("port : %08x, %d\n", sa.sin_port, ntohs (sa.sin_port));

	/* open socket */

	if ((s = socket(hp->h_addrtype, SOCK_STREAM, 0))<0) {
		if ( verbose ) php_printf ("can't create socket\n");
		*ecode = SOCKET_CREATE_FAIL;
		return -1;
	}

	if (verbose) php_printf ("socket open\n");

	/* local socket */
	la.sin_family = hp->h_addrtype;
	la.sin_addr.s_addr = htonl (INADDR_ANY);
	la.sin_port = htons (0);

	/* local bind */
	if ( bind (s, (struct sockaddr *) &la, sizeof (la)) < 0 ) {
		close (s);
		*ecode = BIND_ERROR;
		return -1;
	}

	/* make it nonblocking funtion connection */
	if ( nonblock_func (s, TRUE) == -1 ) {
		close (s);
		*ecode = NONBLOCK_ERROR;
		return -1;
	}

	/* try to connect */

	if ( connect (s, (struct sockaddr *)&sa, sizeof sa) < 0 && errno == EINPROGRESS) {
		/* connected nonblockingly. wait until timeout */
		timeouts.tv_sec = timeout;
		timeouts.tv_usec = 0;

		FD_ZERO (&c_fds);
		FD_ZERO (&r_fds);
		FD_SET (s, &c_fds);
		FD_SET (s, &r_fds);

		/* wait untim something happens */
		if ( (result = select ( s + 1, &r_fds, &c_fds, NULL, &timeouts )) != 1 ) {
			/* failed connect () */
			if ( verbose ) php_printf ("==> ERROR: connect failed code %d\n", result);
			close (s);
			*ecode = CONNECT_FAIL;
			return -1;
		}

		if ( FD_ISSET (s, &r_fds) || FD_ISSET (s, &c_fds) ) {
			if ( FD_ISSET (s, &r_fds) && FD_ISSET (s, &c_fds) ) {
				slen = sizeof (sval);

				if ( getsockopt (s, SOL_SOCKET, SO_ERROR, &sval, &slen) < 0 ) {
					if ( verbose ) php_printf ("==> ERROR: %s\n", strerror (ETIMEDOUT));
					close (s);

					*ecode = C_ETIMEDOUT;
					return -1;
				}

				if ( sval != 0 ) {
					if ( verbose ) php_printf ("==> ERROR: %s\n", strerror (sval));
					close (s);

					switch ( sval ) {
						case ECONNREFUSED :
							*ecode = C_ECONNREFUSED;
							break;
						case ECONNABORTED :
							*ecode = C_ECONNABORTED;
							break;
						case ECONNRESET :
							*ecode = C_ECONNRESET;
							break;
						case ENETRESET :
							*ecode = C_ENETRESET;
							break;
						default :
							*ecode = CONNECT_FAIL;
					}

					return -1;
				}
			}
		}
	}

	if (verbose) php_printf ("connected\n");

	/* return blocking mode */
	if ( nonblock_func (s, FALSE) == -1 ) {
		close (s);
		*ecode = NONBLOCK_ERROR;
		return -1;
	}

  return s;
}

char * o_wmsmsg ( int errn, char * addr ) {
	static char ret[1024] = { 0, };
	if ( addr == NULL )
	addr = "host";

	switch ( errn ) {
		case NOMAL :
			sprintf (ret, "OK");
			break;
		case HOSTS_NOT_FOUND :
			sprintf (ret, "%s not found", addr);
			break;
		case SOCKET_CREATE_FAIL :
			sprintf (ret, "socket create failed");
			break;
		case CONNECT_FAIL :
			sprintf (ret, "connect failed");
			break;
		case MALLOC_ERROR :
			sprintf (ret, "memory allocation error");
			break;
		case ICONV_ERROR:
			sprintf (ret, "failed iconv convert");
			break;
		case NONBLOCK_ERROR:
			sprintf (ret, "failed transport non blocking mode");
			break;
		case BIND_ERROR:
			sprintf (ret, "failed to bind local port");
			break;
		case WRITE_ERROR:
			sprintf (ret, "failed to write on socket");
			break;
		case INVALID_URL:
			sprintf (ret, "%s is invalid address", addr);
			break;
		case CLOSE_PORT:
			sprintf (ret, "Maybe closed5 port on %s", addr);
			break;
		case C_ETIMEDOUT:
			sprintf (ret, "connect timeout on %s", addr);
			break;
		case C_ECONNREFUSED:
			sprintf (ret, "connection refused on %s", addr);
			break;
		case C_ECONNABORTED:
			sprintf (ret, "Connection aborted on %s", addr);
			break;
		case C_ECONNRESET:
			sprintf (ret, "Connection reset on %s", addr);
			break;
		case C_ENETRESET:
			sprintf (ret, "Connection aborted by network on %s", addr);
			break;
		/* mms return msg */
		case FILE_NOT_FOUND :
			sprintf (ret, "file not found");
			break;
		case CORRUPTED_MEDIA :
			sprintf (ret, "media corrucpted");
			break;
		case FILE_WRONG :
			sprintf (ret, "file wrong");
			break;
		case NOT_MEDIA :
			sprintf (ret, "is not media file");
			break;
		/* rtsp return msg */
		case CONTINUE :
			sprintf (ret, "Continue");
			break;
		case OK :
			sprintf (ret, "OK");
			break;
		case CREATED :
			sprintf (ret, "Created");
			break;
		case LOW_ON_STORAGE_SPACE :
			sprintf (ret, "Low On Storage Space");
			break;
		case MULTIPLE_CHOICES :
			sprintf (ret, "Multiple Choices");
			break;
		case MOVED_PERMANENTLY :
			sprintf (ret, "Moved Permanently");
			break;
		case MOVED_TEMPORARILY :
			sprintf (ret, "Moved Temporarily");
			break;
		case SEE_OTHER :
			sprintf (ret, "See Other");
			break;
		case NOT_MODIFIED :
			sprintf (ret, "Not Modified");
			break;
		case USE_PROXY :
			sprintf (ret, "Use Proxy");
			break;
		case GOING_AWAY :
			sprintf (ret, "Going Away");
			break;
		case LOAD_BALANCING :
			sprintf (ret, "Load Balancing");
			break;
		case BAD_REQUEST :
			sprintf (ret, "Bad Request");
			break;
		case UNAUTHORIZED :
			sprintf (ret, "Unauthorized");
			break;
		case PAYMENT_REQUIRED :
			sprintf (ret, "Payment Required");
			break;
		case FORBIDDEN :
			sprintf (ret, "Forbidden");
			break;
		case NOT_FOUND :
			sprintf (ret, "Not Found");
			break;
		case METHOD_NOT_ALLOWED :
			sprintf (ret, "Method Not Allowed");
			break;
		case NOT_ACCEPTABLE :
			sprintf (ret, "Not Acceptable");
			break;
		case PROXY_AUTHENTICATION_REQUIRED :
			sprintf (ret, "Proxy Authentication Required");
			break;
		case REQUEST_TIMEOUT :
			sprintf (ret, "Request Timeout");
			break;
		case GONE :
			sprintf (ret, "Gone");
			break;
		case LENGTH_REQUIRED :
			sprintf (ret, "Length Required");
			break;
		case PRECONDITION_FAILED :
			sprintf (ret, "Precondition Failed");
			break;
		case REQUEST_ENTITY_TOO_LARGE :
			sprintf (ret, "Request Entity Too Large");
			break;
		case REQUEST_URI_TOO_LARGE :
			sprintf (ret, "Request URI Too Large");
			break;
		case UNSUPPORTED_MEDIA_TYPE :
			sprintf (ret, "Unsupported Media Type");
			break;
		case PARAMETER_NOT_UNDERSTOOD :
			sprintf (ret, "Parameter Not Understood");
			break;
		case RESERVED :
			sprintf (ret, "Reserved");
			break;
		case NOT_ENOUGH_BANDWIDTH :
			sprintf (ret, "Not Enough Bandwidth");
			break;
		case SESSION_NOT_FOUND :
			sprintf (ret, "Session Nto Found");
			break;
		case METHOD_NOT_VALID_IN_THIS_STATE :
			sprintf (ret, "Method Not Valid in the State");
			break;
		case HEADER_FIELD_NOT_VALID_FOR_RESOURCE :
			sprintf (ret, "Header Field Not Valid For Resource");
			break;
		case INVALID_RANGE :
			sprintf (ret, "Invalid Range");
			break;
		case PARAMETER_IS_READ_ONLY :
			sprintf (ret, "Parameter is Read Only");
			break;
		case AGGREGATE_OPERATION_NOT_ALLOWED :
			sprintf (ret, "Aggregate Operation not Allowed");
			break;
		case ONLY_AGGREGETE_OPERATION_ALLOWED :
			sprintf (ret, "Only Aggregete Operation Allowed");
			break;
		case UNSUPPORTED_TRANSPORT :
			sprintf (ret, "Unsupported Transport");
			break;
		case DESTINATION_UNREACHABLE :
			sprintf (ret, "Destination Unreachable");
			break;
		case INTERNAL_SERVER_ERROR :
			sprintf (ret, "Internal Server Error");
			break;
		case NOT_IMPLEMENTED :
			sprintf (ret, "Not Implemented");
			break;
		case BAD_GATEWAY :
			sprintf (ret, "Bad Gateway");
			break;
		case SERVICE_UNAVAILABLE :
			sprintf (ret, "Service Unavailable");
			break;
		case GATEWAY_TIMEOUT :
			sprintf (ret, "Gateway Timeout");
			break;
		case RTSP_VERSION_NOT_SUPPORTED :
			sprintf (ret, "RTSP Version Not Supported");
			break;
		case OPTION_NOT_SUPPORTED :
			sprintf (ret, "Option Not Supported");
			break;
		default :
			sprintf (ret, "Other error");
			break;
	}

	return ret;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
