dnl $Id: config.m4,v 1.1.1.1 2004-05-19 18:49:08 oops Exp $
dnl config.m4 for extension mms

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_ENABLE(mms, whether to enable mms support,
[  --enable-mms            Enable mms support])

PHP_ARG_WITH(mms-iconv, for iconv dir for MMS support,
[  --with-mms-iconv=<DIR>      iconv dir for MMS], yes, no)

if test "$PHP_MMS" != "no"; then
  AC_DEFINE(HAVE_MMS,1,[ ])

  OOPS_PARAMETER=$CFLAGS
  PHP_SUBST(CPPFLAGS)

  ICONV_SEARCH_PATH="/usr/local /usr $with_mms_iconv"

  for i in $ICONV_SEARCH_PATH
  do
    if test -f $i/giconv.h -o -f $i/iconv.h ; then
      MMS_INCLUDE=$i
      break
    elif test -f $i/include/giconv.h -o -f $i/include/iconv.h ; then
      MMS_INCLUDE=$i/include
      break
    fi
  done

  for i in $ICONV_SEARCH_PATH
  do
    if test -f $i/libiconv.so -o -f $i/libiconv.a ; then
      MMS_LIB=$i
      break
    elif test -f $i/lib/libiconv.a -o -f $i/lib/libiconv.a ; then
      MMS_LIB=$i/lib
      break
    fi
  done

  if test -n "$MMS_LIB" ; then
    PHP_ADD_LIBRARY_WITH_PATH(iconv, $MMS_LIB, MMS_SHARED_LIBADD)
    AC_DEFINE(HAVE_LIBICONV, 1, [ ])
  fi

  if test -z "$MMS_INCLUDE"; then
    AC_MSG_ERROR([Unable to find iconv.h anywhere under $ICONV_SEARCH_PATH])
  else
    if test -z "$MMS_LIB"; then
      with_mms_iconv=yes
    fi
  fi

  krextra_sources="libmms/client.c"
  KR_PARAMETER="$OOPS_PARAMETER -I./libmms"
  PHP_ADD_BUILD_DIR(./libmms)

  PHP_EXPAND_PATH($MMS_INCLUDE, MMS_INCLUDE)
  PHP_ADD_INCLUDE($MMS_INCLUDE)
  PHP_SUBST(KR_PARAMETER)

  PHP_SUBST(MMS_SHARED_LIBADD)

  dnl if php version is under 4.2.x, use PHP_EXTENSION
  dnl bug over php 4.2.x, use PHP_NEW_EXTENSION
  dnl PHP_EXTENSION(mms, $ext_shared)
  PHP_NEW_EXTENSION(mms, mms.c $krextra_sources, $ext_shared,, \\$(KR_PARAMETER))
fi
