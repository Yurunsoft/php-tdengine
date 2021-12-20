#ifndef PHP_EXT_TDENGINE_H
# define PHP_EXT_TDENGINE_H

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#include <stdio.h>
#include "php.h"
#include "Zend/zend_exceptions.h"
#include "ext/spl/spl_exceptions.h"
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