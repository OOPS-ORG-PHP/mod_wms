/*
 * this file include return code of this program
 * $Id: client.h,v 1.1 2004-06-15 09:21:11 oops Exp $
 */

#ifndef CLIENT_H
#define CLIENT_H

/*
 * return code
 */ 

// nomal return code
#define NOMAL 0

// return code about connection
#define HOSTS_NOT_FOUND                    2001
#define SOCKET_CREATE_FAIL                 2002
#define CONNECT_FAIL                       2003
#define NONBLOCK_ERROR                     2004
#define BIND_ERROR                         2005
#define WRITE_ERROR                        2006
#define INVALID_URL                        2100
#define CLOSE_PORT                         2101
#define C_ETIMEDOUT                        2102
#define C_ECONNREFUSED                     2103
#define C_ECONNABORTED                     2104
#define C_ECONNRESET                       2105
#define C_ENETRESET                        2106

// return code about system
#define MALLOC_ERROR                       4000
#define ICONV_ERROR                        5000

// return code about mms service
#define FILE_NOT_FOUND                     1404
#define CORRUPTED_MEDIA                    1402
#define FILE_WRONG                         1403
#define NOT_MEDIA                          1500
#define OTHER_ERROR                         600

// return code about rtsp service
#define CONTINUE                            100
#define OK                                  200
#define CREATED                             201
#define LOW_ON_STORAGE_SPACE                250
#define MULTIPLE_CHOICES                    300
#define MOVED_PERMANENTLY                   301
#define MOVED_TEMPORARILY                   302
#define SEE_OTHER                           303
#define NOT_MODIFIED                        304
#define USE_PROXY                           305
#define GOING_AWAY                          350
#define LOAD_BALANCING                      351
#define BAD_REQUEST                         400
#define UNAUTHORIZED                        401
#define PAYMENT_REQUIRED                    402
#define FORBIDDEN                           403
#define NOT_FOUND                           404
#define METHOD_NOT_ALLOWED                  405
#define NOT_ACCEPTABLE                      406
#define PROXY_AUTHENTICATION_REQUIRED       407
#define REQUEST_TIMEOUT                     408
#define GONE                                410
#define LENGTH_REQUIRED                     411
#define PRECONDITION_FAILED                 412
#define REQUEST_ENTITY_TOO_LARGE            413
#define REQUEST_URI_TOO_LARGE               414
#define UNSUPPORTED_MEDIA_TYPE              415
#define PARAMETER_NOT_UNDERSTOOD            451
#define RESERVED                            452
#define NOT_ENOUGH_BANDWIDTH                453
#define SESSION_NOT_FOUND                   454
#define METHOD_NOT_VALID_IN_THIS_STATE      455
#define HEADER_FIELD_NOT_VALID_FOR_RESOURCE 456
#define INVALID_RANGE                       457
#define PARAMETER_IS_READ_ONLY              458
#define AGGREGATE_OPERATION_NOT_ALLOWED     459
#define ONLY_AGGREGETE_OPERATION_ALLOWED    460
#define UNSUPPORTED_TRANSPORT               461
#define DESTINATION_UNREACHABLE             462
#define INTERNAL_SERVER_ERROR               500
#define NOT_IMPLEMENTED                     501
#define BAD_GATEWAY                         502
#define SERVICE_UNAVAILABLE                 503
#define GATEWAY_TIMEOUT                     504
#define RTSP_VERSION_NOT_SUPPORTED          505
#define OPTION_NOT_SUPPORTED                551

#ifndef TRUE
#       define TRUE                           0
#endif
#ifndef FALSE
#       define FALSE                          1
#endif

int nonblock_func (int socket, int nonblock);
int HOSTconnect (char * addr, int timeout, int port, int *ecode);
int string_utf_iconv (char *dest, char *src, int len, int type);
void URLEncode (char *dst, char *src);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
