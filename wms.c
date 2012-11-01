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
	NULL,
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

/* {{{ proto int [mms/rtsp]checks (string addr [, string uri [, string opt [, int timeout [, int debug] ] ] ])
 *  get mms or rtps header and check with seperate url on addr and uri and option ext */
static void streaming_checks (INTERNAL_FUNCTION_PARAMETERS, zend_bool is_type)
{
	zval	** G_addr,
			** G_uri,
		   	** G_opt,
			** G_timeout,
			** G_debug;
	char	 * host, *uri, *opt;
	char	 * request = NULL;
	int		   argc = ZEND_NUM_ARGS();
	int		   hostlen, urilen, optlen;
	int		   result;
	int		   timeout = 10, debug = 0;
	char	 * usep;
	char	 * osep;

	host = NULL;
	uri  = NULL;
	opt  = NULL;
	hostlen = 0;

	switch (argc) {
		case 5 :
			if (zend_get_parameters_ex (argc, &G_addr, &G_uri, &G_opt, &G_timeout, &G_debug) == FAILURE)
				WRONG_PARAM_COUNT;

			convert_to_string_ex(G_uri);
			convert_to_string_ex(G_opt);
			convert_to_long_ex(G_timeout);
			convert_to_long_ex(G_debug);

			uri = Z_STRVAL_PP (G_uri);
			opt = Z_STRVAL_PP (G_opt);
			timeout = Z_LVAL_PP (G_timeout);
			debug = Z_LVAL_PP (G_debug);
			break;
		case 4 :
			if (zend_get_parameters_ex (argc, &G_addr, &G_uri, &G_opt, &G_timeout) == FAILURE)
				WRONG_PARAM_COUNT;

			convert_to_string_ex(G_uri);
			convert_to_string_ex(G_opt);
			convert_to_long_ex(G_timeout);

			uri = Z_STRVAL_PP (G_uri);
			opt = Z_STRVAL_PP (G_opt);
			debug = Z_LVAL_PP (G_timeout);
			break;
		case 3 :
			if (zend_get_parameters_ex (argc, &G_addr, &G_uri, &G_opt) == FAILURE)
				WRONG_PARAM_COUNT;

			convert_to_string_ex(G_uri);
			convert_to_string_ex(G_opt);

			uri = Z_STRVAL_PP (G_uri);
			opt = Z_STRVAL_PP (G_opt);
			break;
		case 2 :
			if (zend_get_parameters_ex (argc, &G_addr, &G_uri) == FAILURE)
				WRONG_PARAM_COUNT;

			convert_to_string_ex(G_uri);
			uri = Z_STRVAL_PP (G_uri);
			break;
		case 1 :
			if (zend_get_parameters_ex (argc, &G_addr) == FAILURE)
				WRONG_PARAM_COUNT;

			break;
		default:
			WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(G_addr);
	host = Z_STRVAL_PP (G_addr);
	hostlen = Z_STRLEN_PP (G_addr);

	if ( hostlen < 4 ) {
		RETURN_LONG (HOSTS_NOT_FOUND);
	}

	if ( ! strncmp ("://", host+4, 3) || ! strncmp ("://", host+5, 3) ||
	     ! strncmp ("://", host+5, 3) || ! strncmp ("://", host+6, 3) ) {
		RETURN_LONG (INVALID_URL);
	}

	urilen = (uri != NULL) ? Z_STRLEN_PP (G_uri) : 0;
	optlen = (opt != NULL) ? Z_STRLEN_PP (G_opt) : 0;

	usep = ( urilen == 0 || uri[0] != '/' ) ? "/" : "";
	osep = ( optlen != 0 && opt[0] != '?' ) ? "?" : "";

	request = emalloc ( sizeof (char) * (hostlen + urilen + optlen + 16) );
	if ( request == NULL ) {
		RETURN_LONG (MALLOC_ERROR);
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
	} else result = MALLOC_ERROR;

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

/* {{{ proto int [mms/rtsp]check (string url [, int timeout [, int debug] ] ] ])
 *  get mms or rtps header and check with completed url */
static void streaming_check (INTERNAL_FUNCTION_PARAMETERS, zend_bool is_type)
{
	zval	** G_addr,
			** G_timeout,
		   	** G_debug;
	char	 * host;
	int		   argc = ZEND_NUM_ARGS();
	int		   hostlen;
	int		   result;
	int		   timeout = 10, debug = 0;

	host = NULL;
	hostlen = 0;

	switch (argc) {
		case 3 :
			if (zend_get_parameters_ex (argc, &G_addr, &G_timeout, &G_debug) == FAILURE)
				WRONG_PARAM_COUNT;

			convert_to_long_ex(G_timeout);
			convert_to_long_ex(G_debug);

			timeout = Z_LVAL_PP (G_timeout);
			debug = Z_LVAL_PP (G_debug);
			break;
		case 2 :
			if (zend_get_parameters_ex (argc, &G_addr, &G_timeout) == FAILURE)
				WRONG_PARAM_COUNT;

			convert_to_long_ex(G_timeout);
			debug = Z_LVAL_PP (G_timeout);
			break;
		case 1 :
			if (zend_get_parameters_ex (argc, &G_addr) == FAILURE)
				WRONG_PARAM_COUNT;

			break;
		default:
			WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(G_addr);
	host = Z_STRVAL_PP (G_addr);
	hostlen = Z_STRLEN_PP (G_addr);

	if ( hostlen < 4 ) {
		RETURN_LONG (HOSTS_NOT_FOUND);
	}

	if ( (!is_type && strncmp ("mms://", host, 6)) || (is_type && strncmp ("rtsp://", host, 7)) ) {
		RETURN_LONG (INVALID_URL);
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
	int hostlen;
	int i, result;
	int code = 0;

	host = NULL;
	hostlen = 0;

	switch (argc) {
		case 2 :
			if (zend_get_parameters_ex (argc, &G_code, &G_host) == FAILURE)
				WRONG_PARAM_COUNT;

			convert_to_string_ex(G_host);
			host = Z_STRVAL_PP (G_host);
			hostlen = Z_STRLEN_PP (G_host);
			break;
		case 1 :
			if (zend_get_parameters_ex (argc, &G_code) == FAILURE)
				WRONG_PARAM_COUNT;

			break;
		default:
			WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(G_code);
	code = Z_LVAL_PP (G_code);

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
