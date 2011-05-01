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
  | Author:                                                              |
  +----------------------------------------------------------------------+

  $Id$
*/

#ifndef PHP_WMS_H
#define PHP_WMS_H

#ifdef PHP_WIN32
#define PHP_WMS_API __declspec(dllexport)
#else
#define PHP_WMS_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

extern zend_module_entry wms_module_entry;
#define wms_module_ptr &wms_module_entry

PHP_MINIT_FUNCTION(wms);
PHP_MSHUTDOWN_FUNCTION(wms);
PHP_RINIT_FUNCTION(wms);
PHP_RSHUTDOWN_FUNCTION(wms);
PHP_MINFO_FUNCTION(wms);

PHP_FUNCTION(wmsversion);
PHP_FUNCTION(mmscheck);
PHP_FUNCTION(mmschecks);
PHP_FUNCTION(rtspcheck);
PHP_FUNCTION(rtspchecks);
PHP_FUNCTION(getrtspcode);
PHP_FUNCTION(wmsmsg);

/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(wms)
	int   global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(wms)
*/

/* In every utility function you add that needs to use variables 
   in php_wms_globals, call TSRM_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMG_CC
   after the last function argument and declare your utility function
   with TSRMG_DC after the last declared argument.  Always refer to
   the globals in your function as WMS_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define WMS_G(v) TSRMG(wms_globals_id, zend_wms_globals *, v)
#else
#define WMS_G(v) (wms_globals.v)
#endif

#define WMS_BUILDNO "200406151738"
#define WMS_BUILDVER "1.0.0"

#define phpext_wms_ptr wms_module_ptr

/*
 * return code
 */

// nomal return code
#define NOMAL                                 0

// return code about connection
#define HOSTS_NOT_FOUND                    2001
#define INVALID_URL                        2100
#define CLOSE_PORT                         2101

// return code about system
#define MALLOC_ERROR                       4000
#define ICONV_ERROR                        5000

int o_mmscheck (char *request, int timeout, int verbose);
int o_rtspcheck (char *request, int timeout, int verbose);
int o_getrtspcode (void);
int o_wmsmsg (int code, char *host);

#endif	/* PHP_WMS_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
