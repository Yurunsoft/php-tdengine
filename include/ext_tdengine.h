#ifndef PHP_EXT_TDENGINE_H
# define PHP_EXT_TDENGINE_H

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#include <stdio.h>
#include <string>
#include <thread>
#include <signal.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "php.h"
#include "Zend/zend_exceptions.h"
#include "ext/spl/spl_exceptions.h"

#ifdef __cplusplus
}
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(WIN64) || defined(_WIN64) || defined(__WIN64__) || defined(__NT__)
    #define IS_WIN 1
#else
    #define IS_WIN 0
#endif

#if IS_WIN
#	ifdef PHP_TDENGINE_EXPORTS
#		define PHP_TDENGINE_API __declspec(dllexport)
#	elif defined(COMPILE_DL_TDENGINE)
#		define PHP_TDENGINE_API __declspec(dllimport)
#	else
#		define PHP_TDENGINE_API /* nothing special */
#	endif
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_TDENGINE_API __attribute__ ((visibility("default")))
#else
#	define PHP_TDENGINE_API
#endif

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

#if !HAVE_TAOS_BIND
#   define TAOS_BIND TAOS_MULTI_BIND
#endif

using namespace std;

#ifdef NO_TSTRERROR
inline const char* inner_tstrerror(int32_t err)
{
    return std::to_string(err).c_str();
}
#endif

#define this_object(class) zend_object_to_object_ptr(Z_OBJ_P(ZEND_THIS), class)

#define this_object_container(class) zend_object_to_object(Z_OBJ_P(ZEND_THIS), class)

#define zend_object_to_object(zobj, class) ((class *)((char *)(zobj) - XtOffsetOf(class, std)))

#define zend_object_to_object_ptr(zobj, class) (zend_object_to_object(zobj, class))->ptr

#define object_to_zend_object(obj) ((zend_object*)(obj + 1) - 1)

#endif	/* PHP_EXT_TDENGINE_H */