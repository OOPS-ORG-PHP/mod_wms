dnl $Id: config.m4,v 1.3 2008-07-15 11:46:03 oops Exp $
dnl config.m4 for extension wms

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_ENABLE(wms, whether to enable wms support,
[  --enable-wms            Enable wms support])

PHP_ARG_WITH(wms-iconv, for iconv dir for WMS support,
[  --with-wms-iconv=<DIR>      iconv dir for WMS], yes, no)

if test "$PHP_WMS" != "no"; then
  AC_DEFINE(HAVE_WMS,1,[ ])

  OOPS_PARAMETER=$CFLAGS
  PHP_SUBST(CPPFLAGS)

  ICONV_SEARCH_PATH="/usr/local /usr $with_wms_iconv"

  for i in $ICONV_SEARCH_PATH
  do
    if test -f $i/giconv.h -o -f $i/iconv.h ; then
      WMS_INCLUDE=$i
      break
    elif test -f $i/include/giconv.h -o -f $i/include/iconv.h ; then
      WMS_INCLUDE=$i/include
      break
    fi
  done

  for i in $ICONV_SEARCH_PATH
  do
    if test -f $i/libiconv.so -o -f $i/libiconv.a ; then
      WMS_LIB=$i
      break
    elif test -f $i/$PHP_LIBDIR/libiconv.a -o -f $i/$PHP_LIBDIR/libiconv.a ; then
      WMS_LIB=$i/$PHP_LIBDIR
      break
    fi
  done

  if test -n "$WMS_LIB" ; then
    PHP_ADD_LIBRARY_WITH_PATH(iconv, $WMS_LIB, WMS_SHARED_LIBADD)
    AC_DEFINE(HAVE_LIBICONV, 1, [ ])
  fi

  if test -z "$WMS_INCLUDE"; then
    AC_MSG_ERROR([Unable to find iconv.h anywhere under $ICONV_SEARCH_PATH])
  else
    if test -z "$WMS_LIB"; then
      with_wms_iconv=yes
    fi
  fi

  krextra_sources="libmms/mms.c libmms/rtsp.c libmms/lib.c"
  KR_PARAMETER="$OOPS_PARAMETER -I./libmms"
  PHP_ADD_BUILD_DIR(./libmms)

  PHP_EXPAND_PATH($WMS_INCLUDE, WMS_INCLUDE)
  PHP_ADD_INCLUDE($WMS_INCLUDE)
  PHP_SUBST(KR_PARAMETER)

  PHP_SUBST(WMS_SHARED_LIBADD)

  dnl if php version is under 4.2.x, use PHP_EXTENSION
  dnl bug over php 4.2.x, use PHP_NEW_EXTENSION
  dnl PHP_EXTENSION(wms, $ext_shared)
  PHP_NEW_EXTENSION(wms, wms.c $krextra_sources, $ext_shared,, \\$(KR_PARAMETER))
fi
