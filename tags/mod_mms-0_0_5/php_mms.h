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

  $Id: php_mms.h,v 1.3 2004-05-27 19:21:40 oops Exp $
*/

#ifndef PHP_MMS_H
#define PHP_MMS_H

#ifdef PHP_WIN32
#define PHP_MMS_API __declspec(dllexport)
#else
#define PHP_MMS_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

extern zend_module_entry mms_module_entry;
#define mms_module_ptr &mms_module_entry

PHP_MINIT_FUNCTION(mms);
PHP_MSHUTDOWN_FUNCTION(mms);
PHP_RINIT_FUNCTION(mms);
PHP_RSHUTDOWN_FUNCTION(mms);
PHP_MINFO_FUNCTION(mms);

PHP_FUNCTION(mmsversion);
PHP_FUNCTION(mmscheck);
PHP_FUNCTION(mmschecks);
PHP_FUNCTION(mmserror);

/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(mms)
	int   global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(mms)
*/

/* In every utility function you add that needs to use variables 
   in php_mms_globals, call TSRM_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMG_CC
   after the last function argument and declare your utility function
   with TSRMG_DC after the last declared argument.  Always refer to
   the globals in your function as MMS_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define MMS_G(v) TSRMG(mms_globals_id, zend_mms_globals *, v)
#else
#define MMS_G(v) (mms_globals.v)
#endif

#define MMS_BUILDNO "200405290404"
#define MMS_BUILDVER "0.0.5"

#define phpext_mms_ptr mms_module_ptr

#define NOMAL 0
#define HOST_NOT_FOUND 1
#define SOCKET_CREATE_FAIL 2
#define CONNECT_FAIL 3
#define MALLOC_ERROR 4
#define ICONV_ERROR 5
#define NONBLOCK_ERROR 6
#define BIND_ERROR 7
#define WRITE_ERROR 8

#define INVALID_URL 100
#define CLOSE_PORT 101

#define C_ETIMEDOUT 102
#define C_ECONNREFUSED 103
#define C_ECONNABORTED 104
#define C_ECONNRESET 105
#define C_ENETRESET 106

#define FILE_NOT_FOUND 401
#define CORRUPTED_MEDIA 402
#define FILE_WRONG 403
#define NOT_MEDIA 404
#define OTHER_ERROR 405

int o_mmscheck (char *request, int timeout, int verbose);

#endif	/* PHP_MMS_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
