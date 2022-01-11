dnl config.m4 for extension tdengine

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary.

dnl If your extension references something external, use 'with':

dnl PHP_ARG_WITH([tdengine],
dnl   [for tdengine support],
dnl   [AS_HELP_STRING([--with-tdengine],
dnl     [Include tdengine support])])

dnl Otherwise use 'enable':

PHP_ARG_ENABLE([tdengine],
  [whether to enable tdengine support],
  [AS_HELP_STRING([--enable-tdengine],
    [Enable tdengine support])],
  [no])

PHP_ARG_ENABLE(swoole, swoole support,
[  --enable-swoole           Enable swoole support], [enable_swoole="yes"])

if test "$PHP_TDENGINE" != "no"; then
  dnl Write more examples of tests here...

  dnl Remove this code block if the library does not support pkg-config.
  dnl PKG_CHECK_MODULES([LIBFOO], [foo])
  dnl PHP_EVAL_INCLINE($LIBFOO_CFLAGS)
  dnl PHP_EVAL_LIBLINE($LIBFOO_LIBS, TDENGINE_SHARED_LIBADD)

  dnl If you need to check for a particular library version using PKG_CHECK_MODULES,
  dnl you can use comparison operators. For example:
  dnl PKG_CHECK_MODULES([LIBFOO], [foo >= 1.2.3])
  dnl PKG_CHECK_MODULES([LIBFOO], [foo < 3.4])
  dnl PKG_CHECK_MODULES([LIBFOO], [foo = 1.2.3])

  dnl Remove this code block if the library supports pkg-config.
  dnl --with-tdengine -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/tdengine.h"  # you most likely want to change this
  dnl if test -r $PHP_TDENGINE/$SEARCH_FOR; then # path given as parameter
  dnl   TDENGINE_DIR=$PHP_TDENGINE
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for tdengine files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       TDENGINE_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$TDENGINE_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the tdengine distribution])
  dnl fi

  dnl Remove this code block if the library supports pkg-config.
  dnl --with-tdengine -> add include path
  dnl PHP_ADD_INCLUDE($TDENGINE_DIR/include)

  dnl Remove this code block if the library supports pkg-config.
  dnl --with-tdengine -> check for lib and symbol presence
  dnl LIBNAME=TDENGINE # you may want to change this
  dnl LIBSYMBOL=TDENGINE # you most likely want to change this

  dnl If you need to check for a particular library function (e.g. a conditional
  dnl or version-dependent feature) and you are using pkg-config:
  dnl PHP_CHECK_LIBRARY($LIBNAME, $LIBSYMBOL,
  dnl [
  dnl   AC_DEFINE(HAVE_TDENGINE_FEATURE, 1, [ ])
  dnl ],[
  dnl   AC_MSG_ERROR([FEATURE not supported by your tdengine library.])
  dnl ], [
  dnl   $LIBFOO_LIBS
  dnl ])

  dnl If you need to check for a particular library function (e.g. a conditional
  dnl or version-dependent feature) and you are not using pkg-config:
  dnl PHP_CHECK_LIBRARY($LIBNAME, $LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $TDENGINE_DIR/$PHP_LIBDIR, TDENGINE_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_TDENGINE_FEATURE, 1, [ ])
  dnl ],[
  dnl   AC_MSG_ERROR([FEATURE not supported by your tdengine library.])
  dnl ],[
  dnl   -L$TDENGINE_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(TDENGINE_SHARED_LIBADD)

  dnl In case of no dependencies
  PHP_ARG_WITH([tdengine_dir],
    [dir of tdengine],
    [AS_HELP_STRING([[--with-tdengine-dir[=DIR]]],
      [Include TDengine support (requires TDengine >= 2.0.0)])], [no], [no])

  AC_DEFINE(HAVE_TDENGINE, 1, [ Have tdengine support ])
  if test "$PHP_TDENGINE_DIR" != "no"; then
    TDENGINE_INCLUDE="${PHP_TDENGINE_DIR}/include"
    TDENGINE_LIBDIR="${PHP_TDENGINE_DIR}/driver"
  else
    TDENGINE_INCLUDE="/usr/local/taos/include"
    TDENGINE_LIBDIR="/usr/local/taos/driver"
  fi

  PHP_CHECK_LIBRARY(taos, taos_init,
  [
  ], [
    AC_MSG_ERROR(tdengine module requires libtaos >= 2.0.0)
  ], [
    -L$TDENGINE_LIBDIR
  ])

  PHP_ADD_LIBRARY_WITH_PATH(taos, $TDENGINE_LIBDIR, TDENGINE_SHARED_LIBADD)
  PHP_SUBST(TDENGINE_SHARED_LIBADD)

  PHP_ADD_INCLUDE($TDENGINE_INCLUDE)

  if test "$PHP_SWOOLE" = "yes"; then
    AC_DEFINE(HAVE_SWOOLE, 1, [use swoole])
    PHP_ADD_INCLUDE([$phpincludedir/ext/swoole])
    PHP_ADD_INCLUDE([$phpincludedir/ext/swoole/include])
    PHP_ADD_EXTENSION_DEP(tdengine, swoole)
  fi

  PHP_NEW_EXTENSION(tdengine, tdengine.cc src/ext_taos.cc src/ext_taos_connection.cc src/ext_taos_resource.cc src/ext_taos_statement.cc, $ext_shared,,, cxx)

  PHP_REQUIRE_CXX()
  
  CXXFLAGS="$CXXFLAGS -Wall -Wno-unused-function -Wno-deprecated -Wno-deprecated-declarations -std=c++11"
fi
