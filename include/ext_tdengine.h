#ifndef PHP_EXT_TDENGINE_H
# define PHP_EXT_TDENGINE_H

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#include <stdio.h>
#include <thread>
#include <signal.h>
#include "php.h"
#include "Zend/zend_exceptions.h"
#include "ext/spl/spl_exceptions.h"

#define IS_WIN (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(WIN64) || defined(_WIN64) || defined(__WIN64__) || defined(__NT__)) ? 1 : 0

// PHP < 8.0
#ifndef RETURN_THROWS
#define RETURN_THROWS() RETURN_FALSE
#endif

#ifndef ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE
#define ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(pass_by_ref, name, type_hint, allow_null, default_value) ZEND_ARG_TYPE_INFO(pass_by_ref, name, type_hint, allow_null)
#endif

BEGIN_EXTERN_C()
#include "tdengine_arginfo.h"
END_EXTERN_C()
#include <taos.h>
#include <taoserror.h>
#ifdef HAVE_SWOOLE
#include "tdengine_swoole.h"
#endif
using namespace std;

#define this_object(class) zend_object_to_object_ptr(Z_OBJ_P(ZEND_THIS), class)

#define this_object_container(class) zend_object_to_object(Z_OBJ_P(ZEND_THIS), class)

#define zend_object_to_object(zobj, class) ((class *)((char *)(zobj) - XtOffsetOf(class, std)))

#define zend_object_to_object_ptr(zobj, class) (zend_object_to_object(zobj, class))->ptr

#define object_to_zend_object(obj) ((zend_object*)(obj + 1) - 1)

#endif	/* PHP_EXT_TDENGINE_H */