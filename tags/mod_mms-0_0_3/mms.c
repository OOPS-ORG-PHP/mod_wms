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

  $Id: mms.c,v 1.1.1.1 2004-05-19 18:49:08 oops Exp $
*/

/*
 * PHP4 MMS String modue "mms" - only mms
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "SAPI.h"
#include "ext/standard/info.h"

#include "php_mms.h"

/* If you declare any globals in php_mms.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(mms)
*/

/* True global resources - no need for thread safety here */
static int le_mms;

/* {{{ mms_functions[]
 *
 * Every user visible function must have an entry in mms_functions[].
 */
function_entry mms_functions[] = {
	PHP_FE(mmscheck,			NULL)
	PHP_FE(mmschecks,			NULL)
	PHP_FE(mmsversion,			NULL)
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ mms_module_entry
 */
zend_module_entry mms_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"mms",
	mms_functions,
	NULL,
	NULL,
	NULL,
	NULL,
	PHP_MINFO(mms),
#if ZEND_MODULE_API_NO >= 20010901
	MMS_BUILDVER, /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_MMS
ZEND_GET_MODULE(mms)
#endif

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(mms)
{
	php_info_print_table_start();
	php_info_print_table_colspan_header(2, "MMS extension support");
	php_info_print_table_row(2, "Build No", MMS_BUILDNO);
	php_info_print_table_row(2, "Build version", MMS_BUILDVER);
	php_info_print_table_row(2, "URL", "http://devel.oops.org/");
	php_info_print_table_end();
}
/* }}} */

/* {{{ proto int mmschecks (string addr [, string uri [, string opt [, int timeout [, int debug] ] ] ])
 *  print mms extension build number */
PHP_FUNCTION(mmschecks)
{
	zval ** G_addr, ** G_uri, ** G_opt, ** G_timeout, **G_debug;
	char *host, *uri, *opt;
	char *request = NULL;
	int argc = ZEND_NUM_ARGS();
	int hostlen, urilen, optlen;
	int i, result;
	int timeout = 10, debug = 0;
	char *usep;
	char *osep;

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
		RETURN_LONG (HOST_NOT_FOUND);
	}

	if ( ! strncmp ("://", host+4, 3) || ! strncmp ("://", host+5, 3) ) {
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
	sprintf (request, "mms://%s%s%s%s%s", host, usep, uri, osep, opt);
	//php_printf ("*** %s : %s : %c : %s : %c : %s\n", request, host, usep, uri, osep, opt);

	if ( request != NULL ) {
		result = o_mmscheck (request, timeout, debug);
		efree (request);
	} else result = MALLOC_ERROR;

	RETURN_LONG (result);
}
/* }}} */

/* {{{ proto int mmscheck (string addr [, string uri [, string opt [, int timeout [, int debug] ] ] ])
 *  print mms extension build number */
PHP_FUNCTION(mmscheck)
{
	zval ** G_addr, ** G_timeout, **G_debug;
	char *host;
	char *request = NULL;
	int argc = ZEND_NUM_ARGS();
	int hostlen;
	int i, result;
	int timeout = 10, debug = 0;

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
		RETURN_LONG (HOST_NOT_FOUND);
	}

	if ( strncmp ("mms://", host, 6) ) {
		RETURN_LONG (INVALID_URL);
	}

	result = o_mmscheck (host, timeout, debug);

	RETURN_LONG (result);
}
/* }}} */

/* {{{ proto unsigned char mmsversion(void)
 *  print mms extension version */
PHP_FUNCTION(mmsversion)
{
	RETURN_STRING (MMS_BUILDVER,1);
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
