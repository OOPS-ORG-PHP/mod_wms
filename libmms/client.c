/*
 * mms://netshow.msn.com/msnbc8 
 * mms://216.106.172.144/bbc1099/ads/ibeam/0_ibeamEarth_aaab00020_15_350k.asf
 * mms://195.124.124.82/56/081001_angriffe_1200.wmv
 * mms://193.159.244.12/n24_wmt_mid
 */

#include <config.h>
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
//#include <zend_errors.h>
#include <php.h>
#include "bswap.h"

#include <sys/fcntl.h>
#include <netinet/tcp.h>


#ifdef __CYGWIN__
typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef unsigned long long int uint64_t;
#else
#include <inttypes.h>
#endif

#define BUF_SIZE 102400

//int o_mmscheck (char *url, int verbose);

int nonblock_func (int socket, int nonblock) {
  int flags;

  flags = fcntl (socket, F_GETFL, 0);

  if ( TRUE == nonblock )
    flags |= O_NONBLOCK;
  else
    flags &= ~O_NONBLOCK;

  return fcntl (socket, F_SETFL, flags);
}

typedef struct {

  uint8_t buf[BUF_SIZE];
  int     num_bytes;

} command_t;

int seq_num;
int num_stream_ids;
int stream_ids[20];
int verbose = 0;
unsigned int cr = 0;
unsigned int total_packet_len = 0;

static void put_32 (command_t *cmd, uint32_t value) {

  cmd->buf[cmd->num_bytes  ] = value % 256;
  value = value >> 8;
  cmd->buf[cmd->num_bytes+1] = value % 256 ;
  value = value >> 8;
  cmd->buf[cmd->num_bytes+2] = value % 256 ;
  value = value >> 8;
  cmd->buf[cmd->num_bytes+3] = value % 256 ;

  cmd->num_bytes += 4;
}

static uint32_t get_32 (unsigned char *cmd, int offset) {

  uint32_t ret;

  ret = cmd[offset] ;
  ret |= cmd[offset+1]<<8 ;
  ret |= cmd[offset+2]<<16 ;
  ret |= cmd[offset+3]<<24 ;

  return ret;
}

static int send_command (int s, int command, uint32_t switches, 
			  uint32_t extra, int length,
			  char *data) {
  
  command_t  cmd;
  int        len8;
  int        i;

  len8 = (length + (length%8)) / 8;

  cmd.num_bytes = 0;

  put_32 (&cmd, 0x00000001); /* start sequence */
  put_32 (&cmd, 0xB00BFACE); /* #-)) */
  put_32 (&cmd, length + 32);
  put_32 (&cmd, 0x20534d4d); /* protocol type "MMS " */
  put_32 (&cmd, len8 + 4);
  put_32 (&cmd, seq_num);
  seq_num++;
  put_32 (&cmd, 0x0);        /* unknown */
  put_32 (&cmd, 0x0);
  put_32 (&cmd, len8+2);
  put_32 (&cmd, 0x00030000 | command); /* dir | command */
  put_32 (&cmd, switches);
  put_32 (&cmd, extra);

  memcpy (&cmd.buf[48], data, length);

  if (write (s, cmd.buf, length+48) != (length+48)) {
    if ( verbose )
      php_printf ("==> ERROR: write error. maybe closed prot 1755.\n");

    return -1;
  }

  if (verbose) {
    php_printf ("\n***************************************************\ncommand sent, %d bytes\n", length+48);

    php_printf ("start sequence %08x\n", get_32 (cmd.buf,  0));
    php_printf ("command id     %08x\n", get_32 (cmd.buf,  4));
    php_printf ("length         %8x \n", get_32 (cmd.buf,  8));
    php_printf ("len8           %8x \n", get_32 (cmd.buf, 16));
    php_printf ("sequence #     %08x\n", get_32 (cmd.buf, 20));
    php_printf ("len8  (II)     %8x \n", get_32 (cmd.buf, 32));
    php_printf ("dir | comm     %08x\n", get_32 (cmd.buf, 36));
    php_printf ("switches       %08x\n", get_32 (cmd.buf, 40));

    php_printf ("ascii contents>");
    for (i=48; i<(length+48); i+=2) {
      unsigned char c = cmd.buf[i];

      if ((c>=32) && (c<=128))
        php_printf ("%c", c);
      else
        php_printf (".");
    }
    php_printf ("\n");

    php_printf ("complete hexdump of package follows:\n");
    for (i=0; i<(length+48); i++) {
      unsigned char c = cmd.buf[i];

      php_printf ("%02x", c);

      if ((i % 16) == 15)
        php_printf ("\n");

      if ((i % 2) == 1)
        php_printf (" ");

    }
    php_printf ("\n");
  }

  return 0;
}

static void string_utf16(char *dest, char *src, int len) {
  int i;

  memset (dest, 0, 1000);

  for (i=0; i<len; i++) {
    dest[i*2] = src[i];
    dest[i*2+1] = 0;
  }

  dest[i*2] = 0;
  dest[i*2+1] = 0;
}

static int string_utf16_iconv(char *dest, char *src, int len) {
  //int i;
  //int orig_len = len;
  //int utf16_len = len * 2 + 32;

#ifdef HAVE_LIBICONV
  const
#endif
  char* inbuf_p = src;
  char* outbuf_p = dest;

  size_t il = strlen(src) + 1;
  size_t ol = 1000;

  iconv_t cd = iconv_open ("UTF-16LE", "EUC-KR");

  if (!cd)
    return 1;

  if (iconv (cd, &inbuf_p, &il, &outbuf_p, &ol) == -1 || il != 0)
    return 1;

  /*
  php_error (E_WARNING, "==> %s\n", src);
  for (i = 0; i < orig_len; i++)
    php_printf ("[%02x] %c\n", dest[i], dest[i]);
  */

  iconv_close(cd);
  return 0;
}

static int print_answer (char *data, int len) {

  int i;
  unsigned long s;

  if (verbose) {
    php_printf ("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\nanswer received, %d bytes\n", len);

    php_printf ("start sequence %08x\n", get_32 (data, 0));
    php_printf ("command id     %08x\n", get_32 (data, 4));
    php_printf ("length         %8x \n", get_32 (data, 8));
    php_printf ("len8           %8x \n", get_32 (data, 16));
    php_printf ("sequence #     %08x\n", get_32 (data, 20));
    php_printf ("len8  (II)     %8x \n", get_32 (data, 32));
    php_printf ("dir | comm     %08x\n", get_32 (data, 36));
    php_printf ("switches       %08x\n", get_32 (data, 40));

    for (i=48; i<len; i+=2) {
      unsigned char c = data[i];
    
      if ((c>=32) && (c<128))
        php_printf ("%c", c);
      else
        php_printf (" %02x ", c);
    
    }
    php_printf ("\n");
  }

  s = get_32 (data, 40);
  switch(s){
    case 0x80070002: // media server 9
    case 0xc00d001a: // media server 4.1
      return FILE_NOT_FOUND;
    case 0x8007000d: // media server 9
    case 0xc00d002f: // mdeia server 4.1
      return CORRUPTED_MEDIA;
    case 0xc00d1581: // media server 9
    case 0xc00d002b: // media server 4.1
      return FILE_WRONG;
    case 0xc00d157f: // media server 9
    case 0xc00d07e2: // mdeia server 4.1
      return NOT_MEDIA;
    default:
  }

  if ((s & 0xfff00000) == 0x80000000 || (s & 0xffff0000) == 0xc00d0000) {
      return OTHER_ERROR;
  }

  return NOMAL;
}  

static int get_answer (int s) {

  char  data[BUF_SIZE];
  int   command = 0x1b;
  int   result = NOMAL;

  while (command == 0x1b) {
    int len;

    len = read (s, data, BUF_SIZE) ;
    if (!len) {
      if (verbose) php_printf ("\n==> ERROR: alert! eof\n");
      return OTHER_ERROR;
    }

    if ( (result = print_answer (data, len)) != 0 ) {
      return result;
    }

    command = get_32 (data, 36) & 0xFFFF;

    if (command == 0x1b) {
      if ( send_command (s, 0x1b, 0, 0, 0, data) == -1 ) {
        return WRITE_ERROR;
      }
    }
  }

  return NOMAL;
}

int interp_header (uint8_t *header, int header_len) {

  int i;
  int packet_length = 0;

  /*
   * parse header
   */

  i = 30;
  while (i<header_len) {
    
    uint64_t  guid_1, guid_2, length;

    guid_2 = (uint64_t)header[i] | ((uint64_t)header[i+1]<<8) 
      | ((uint64_t)header[i+2]<<16) | ((uint64_t)header[i+3]<<24)
      | ((uint64_t)header[i+4]<<32) | ((uint64_t)header[i+5]<<40)
      | ((uint64_t)header[i+6]<<48) | ((uint64_t)header[i+7]<<56);
    i += 8;

    guid_1 = (uint64_t)header[i] | ((uint64_t)header[i+1]<<8) 
      | ((uint64_t)header[i+2]<<16) | ((uint64_t)header[i+3]<<24)
      | ((uint64_t)header[i+4]<<32) | ((uint64_t)header[i+5]<<40)
      | ((uint64_t)header[i+6]<<48) | ((uint64_t)header[i+7]<<56);
    i += 8;
    
    if (verbose) php_printf ("guid found: %016llx%016llx\n", guid_1, guid_2);

    length = (uint64_t)header[i] | ((uint64_t)header[i+1]<<8) 
      | ((uint64_t)header[i+2]<<16) | ((uint64_t)header[i+3]<<24)
      | ((uint64_t)header[i+4]<<32) | ((uint64_t)header[i+5]<<40)
      | ((uint64_t)header[i+6]<<48) | ((uint64_t)header[i+7]<<56);

    i += 8;

    if ( (guid_1 == 0x6cce6200aa00d9a6) && (guid_2 == 0x11cf668e75b22630) ) {
      if (verbose) php_printf ("header object\n");
    } else if ((guid_1 == 0x6cce6200aa00d9a6) && (guid_2 == 0x11cf668e75b22636)) {
      if (verbose) php_printf ("data object\n");
    } else if ((guid_1 == 0x6553200cc000e48e) && (guid_2 == 0x11cfa9478cabdca1)) {

      packet_length = get_32(header, i+92-24);

      if (verbose) {
        php_printf ("file object, packet length = %d (%d)\n",
	        packet_length, get_32(header, i+96-24));
      }


    } else if ((guid_1 == 0x6553200cc000e68e) && (guid_2 == 0x11cfa9b7b7dc0791)) {

      int stream_id = header[i+48] | header[i+49] << 8;

      if (verbose) php_printf ("stream object, stream id: %d\n", stream_id);

      stream_ids[num_stream_ids] = stream_id;
      num_stream_ids++;
      

      /*
	} else if ((guid_1 == 0x) && (guid_2 == 0x)) {
	php_printf ("??? object\n");
      */
    } else {
      if (verbose) php_printf ("unknown object\n");
    }

    if (verbose) php_printf ("length    : %lld\n", length);

    i += length-24;

  }

  return packet_length;

}


//int main (int argc, char **argv) {
int o_mmscheck (char *url, int timeout, int verbose) {

  int                  s ;
  struct sockaddr_in   sa;
  struct sockaddr_in   la;
  struct hostent      *hp;
  char                 str[1024];
  char                 data[1024];
  int                  len;
  char                 host[256];
  char                *path, *file, *cp;
  int                  opt, optlength = 0, result = 0;
  struct timeval       timeouts;
  fd_set               c_fds;
  fd_set               r_fds;
  int                  sval;
  socklen_t            slen;

  /* parse url */
  strncpy (host, &url[6], 255);
  cp = strchr(host,'/');
  *cp= 0;

  path = strchr(&url[6], '/') +1;

  if (verbose) {
    php_printf ("host : >%s<\n", host);
    php_printf ("path : >%s<\n", path);
  }

  file = strrchr (url, '/');

  /* DNS lookup */

  if ((hp = gethostbyname(host)) == NULL) {
    if ( verbose ) php_error(E_WARNING, "Host name lookup failure.\n");
    return HOST_NOT_FOUND;
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

  sa.sin_port = htons(1755) ; /* be2me_16(1755);  mms port 1755 */

  if (verbose) php_printf ("port : %08x, %d\n", sa.sin_port, ntohs (sa.sin_port));

  /* open socket */

  if ((s = socket(hp->h_addrtype, SOCK_STREAM, 0))<0) {
    if ( verbose ) php_printf ("==> ERROR: can't create socket\n");
    return  SOCKET_CREATE_FAIL;
  }

  if (verbose) php_printf ("socket open\n");

  /* local socket */
  la.sin_family = hp->h_addrtype;
  la.sin_addr.s_addr = htonl (INADDR_ANY);
  la.sin_port = htons (0);

  /* local bind */
  if ( bind (s, (struct sockaddr *) &la, sizeof (la)) < 0 ) {
    close (s);
    return BIND_ERROR;
  }

  /* make it nonblocking funtion connection */
  if ( nonblock_func (s, TRUE) == -1 ) {
    close (s);
    return NONBLOCK_ERROR;
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
    if ( (result = select ( s + 1, &r_fds, &c_fds, NULL, &timeouts )) < 1 ) {
      /* failed connect () */
      if ( verbose ) php_printf ("==> ERROR: connect failed code %d\n", result);
      close (s);
      return CONNECT_FAIL;
    }

    if ( FD_ISSET (s, &r_fds) || FD_ISSET (s, &c_fds) ) {
      if ( FD_ISSET (s, &r_fds) && FD_ISSET (s, &c_fds) ) {
        slen = sizeof (sval);

	if ( getsockopt (s, SOL_SOCKET, SO_ERROR, &sval, &slen) < 0 ) {
	  if ( verbose ) php_printf ("==> ERROR: %s\n", strerror (ETIMEDOUT));
	  close (s);
	  return C_ETIMEDOUT;
	}

	if ( sval != 0 ) {
          if ( verbose ) php_printf ("==> ERROR: %s\n", strerror (sval));
	  close (s);

	  switch ( sval ) {
            case ECONNREFUSED :
              return C_ECONNREFUSED;
	    case ECONNABORTED :
	      return C_ECONNABORTED;
	    case ECONNRESET :
	      return C_ECONNRESET;
	    case ENETRESET :
	      return C_ENETRESET;
	    default :
	      return CONNECT_FAIL;
	  }
	}
      }
    }
  }

  if (verbose) php_printf ("connected\n");

  /* return blocking mode */
  if ( nonblock_func (s, FALSE) == -1 ) {
    close (s);
    return NONBLOCK_ERROR;
  }

  /* cmd1 */
  
  sprintf (str, "\034\003NSPlayer/7.0.0.1956; {33715801-BAB3-9D85-24E9-03B90328270A}; Host: %s", host);
  string_utf16 (data, str, strlen(str)+2);

  if ( send_command (s, 1, 0, 0x0004000b, strlen(str) * 2+8, data) == -1 ) {
    close (s);
    return WRITE_ERROR;
  }

  len = read (s, data, BUF_SIZE) ;
  if (len) {
    if ( (result = print_answer (data, len)) != 0 ) {
      close (s);
      return result;
    }
  }

  /* cmd2 */

  string_utf16 (&data[8], "\002\000\\\\192.168.0.129\\TCP\\1037\0000", 
		28);
  memset (data, 0, 8);
  if ( send_command (s, 2, 0, 0, 28*2+8, data) == -1 ) {
    close (s);
    return WRITE_ERROR;
  }

  len = read (s, data, BUF_SIZE) ;
  if (len) {
    if ( (result = print_answer (data, len)) != 0 ) {
      close (s);
      return result;
    }
  }

  /* 0x5 */

  if ( (result = string_utf16_iconv (&data[8], path, strlen(path))) == 1 ) {
    close (s);
    return ICONV_ERROR;
  }

  memset (data, 0, 8);
  if ( send_command (s, 5, 0, 0, strlen(path)*2+12, data) == -1 ) {
    close (s);
    return WRITE_ERROR;
  }

  result = get_answer (s);

  /* 0x15 */

  close (s);

  return result;
}
