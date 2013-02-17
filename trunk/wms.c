/*
  +----------------------------------------------------------------------+
  | PHP Version 4                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2002 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 2.02 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available at through the world-wide-web at                           |
  | http://www.php.net/license/2_02.txt.                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: JoungKyun Kim <http://www.oops.org>                          |
  +----------------------------------------------------------------------+

  $Id$
*/

/*
 * PHP4 WMS String modue "wms" - only wms
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "SAPI.h"
#include "ext/standard/info.h"

#include "php_wms.h"

/* If you declare any globals in php_wms.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(wms)
*/

/* True global resources - no need for thread safety here */
static int le_wms;

/* {{{ wms_functions[]
 *
 * Every user visible function must have an entry in wms_functions[].
 */
const zend_function_entry wms_functions[] = {
	PHP_FE(mmscheck,			NULL)
	PHP_FE(mmschecks,			NULL)
	PHP_FE(rtspcheck,			NULL)
	PHP_FE(rtspchecks,			NULL)
	PHP_FE(getrtspcode,			NULL)
	PHP_FE(wmsmsg,				NULL)
	PHP_FE(wmsversion,			NULL)
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ wms_module_entry
 */
zend_module_entry wms_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"wms",
	wms_functions,
	PHP_MINIT(wms),
	NULL,
	NULL,
	NULL,
	PHP_MINFO(wms),
#if ZEND_MODULE_API_NO >= 20010901
	WMS_BUILDVER, /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_WMS
ZEND_GET_MODULE(wms)
#endif

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(wms)
{
	REGISTER_LONG_CONSTANT ("WMS_NORMAL",          WMS_NORMAL, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT ("WMS_HOSTS_NOT_FOUND", WMS_HOSTS_NOT_FOUND, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT ("WMS_INVALID_URL",     WMS_INVALID_URL, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT ("WMS_CLOSE_PORT",      WMS_CLOSE_PORT, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT ("WMS_MALLOC_ERROR",    WMS_MALLOC_ERROR, CONST_PERSISTENT | CONST_CS);
	REGISTER_LONG_CONSTANT ("WMS_ICONV_ERROR",     WMS_ICONV_ERROR, CONST_PERSISTENT | CONST_CS);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(wms)
{
	php_info_print_table_start();
	php_info_print_table_colspan_header(2, "WMS extension support");
	php_info_print_table_row(2, "Build No", WMS_BUILDNO);
	php_info_print_table_row(2, "Build version", WMS_BUILDVER);
	php_info_print_table_row(2, "URL", "http://devel.oops.org/");
	php_info_print_table_end();
}
/* }}} */

/* {{{ proto int [mms/rtsp]checks (string addr[, string uri[, string opt[, int timeout[, int is_type]]]])
 *  get mms or rtps header and check with seperate url on addr and uri and option ext */
static void streaming_checks (INTERNAL_FUNCTION_PARAMETERS, zend_bool is_type)
{
	char	 * host, *uri, *opt;
	char	 * request = NULL;
	int		   hostlen = 0, urilen = 0, optlen = 0;
	int		   result;
	int		   timeout = 10, debug = 0;
	char	 * usep;
	char	 * osep;

	host = NULL;
	uri  = NULL;
	opt  = NULL;
	hostlen = 0;

	if ( wms_parameters
			("s|sslb", &host, &hostlen, &uri, &urilen, &opt, &optlen, &timeout, &is_type) == FAILURE )
	{
		return;
	}

	if ( hostlen < 4 )
		RETURN_LONG (WMS_HOSTS_NOT_FOUND);

	if ( ! strncmp ("://", host + 4, 3) || ! strncmp ("://", host + 5, 3) ||
	     ! strncmp ("://", host + 5, 3) || ! strncmp ("://", host + 6, 3) )
		RETURN_LONG (WMS_INVALID_URL);

	usep = ( urilen == 0 || uri[0] != '/' ) ? "/" : "";
	osep = ( optlen != 0 && opt[0] != '?' ) ? "?" : "";

	request = emalloc (sizeof (char) * (hostlen + urilen + optlen + 16));
	if ( request == NULL ) {
		RETURN_LONG (WMS_MALLOC_ERROR);
	}

	memset (request, 0, sizeof (char) * (hostlen + urilen + optlen + 16));
	sprintf (request, "%s://%s%s%s%s%s", is_type ? "rtsp" : "mms", host, usep, uri, osep, opt);
	//php_printf ("*** %s : %s : %c : %s : %c : %s\n", request, host, usep, uri, osep, opt);

	if ( request != NULL ) {
		if ( is_type )
			result = o_rtspcheck (request, timeout, debug);
		else
			result = o_mmscheck (request, timeout, debug);

		efree (request);
	} else
		result = WMS_MALLOC_ERROR;

	RETURN_LONG (result);
}

PHP_FUNCTION(mmschecks)
{
	streaming_checks (INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}

PHP_FUNCTION(rtspchecks)
{
	streaming_checks (INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto int [mms/rtsp]check (string url[, int timeout[, int method]])
 *  get mms or rtps header and check with completed url */
static void streaming_check (INTERNAL_FUNCTION_PARAMETERS, zend_bool is_type)
{
	char	 * host;
	int		   hostlen = 0;
	int		   result;
	int		   timeout = 10, debug = 0;

	host = NULL;
	hostlen = 0;

	if ( wms_parameters ("s|lb", &host, &hostlen, &timeout, &is_type) == FAILURE )
		return;

	if ( hostlen < 4 ) {
		RETURN_LONG (WMS_HOSTS_NOT_FOUND);
	}

	if ( (!is_type && strncmp ("mms://", host, 6)) || (is_type && strncmp ("rtsp://", host, 7)) ) {
		RETURN_LONG (WMS_INVALID_URL);
	}

	if ( is_type )
		result = o_rtspcheck (host, timeout, debug);
	else
		result = o_mmscheck (host, timeout, debug);


	RETURN_LONG (result);
}

PHP_FUNCTION(mmscheck)
{
	streaming_check (INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}

PHP_FUNCTION(rtspcheck)
{
	streaming_check (INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ proto int getrtspcode (void)
 *  print rtsp header return code message */
PHP_FUNCTION(getrtspcode)
{
	int ret;

	ret = o_getrtspcode();

	RETURN_LONG (ret);
}
/* }}} */

/* {{{ proto int wmsmsg (int returnCode [, string address ] ])
 *  print wms return code message */
PHP_FUNCTION(wmsmsg)
{
	zval ** G_code, ** G_host;
	char *host;
	int argc = ZEND_NUM_ARGS();
	int hostlen = 0;
	int i, result;
	int code = 0;

	host = NULL;
	hostlen = 0;

	if ( wms_parameters ("l|s", &code, &host, &hostlen) == FAILURE )
		return;

	if ( hostlen == 0 )
		host = "host";

	RETURN_STRING ((char *) o_wmsmsg (code, host), 1);
}
/* }}} */

/* {{{ proto unsigned char wmsversion(void)
 *  print wms extension version */
PHP_FUNCTION(wmsversion)
{
	RETURN_STRING (WMS_BUILDVER,1);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
