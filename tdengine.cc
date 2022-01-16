/* tdengine extension for PHP */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "Zend/zend_exceptions.h"
#include <taos.h>
#include "php_tdengine.h"
#include "ext_taos.h"
#include "ext_taos_connection.h"
#include "ext_taos_resource.h"
#include "ext_taos_statement.h"

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(tdengine)
{
	register_constants(module_number);
	register_class_TDengine_Connection();
	register_class_TDengine_Resource();
	register_class_TDengine_Statement();
	register_class_TDengine_Exception(zend_ce_exception);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(tdengine)
{
	if (taos_inited)
	{
		taos_cleanup();
		taos_inited = false;
	}
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(tdengine)
{
#if defined(ZTS) && defined(COMPILE_DL_TDENGINE)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(tdengine)
{
	php_info_print_table_start();
    php_info_print_table_row(2, "version", PHP_TDENGINE_VERSION);
	php_info_print_table_header(2, "tdengine support", "enabled");
    php_info_print_table_row(2, "tdengine", taos_get_client_info());
#ifdef HAVE_SWOOLE
    php_info_print_table_row(2, "swoole", SWOOLE_VERSION);
#endif
	php_info_print_table_end();
}
/* }}} */

/* {{{ tdengine_module_entry */
zend_module_entry tdengine_module_entry = {
	STANDARD_MODULE_HEADER,
	"tdengine",					/* Extension name */
	ext_functions,					/* zend_function_entry */
	PHP_MINIT(tdengine),			/* PHP_MINIT - Module initialization */
	PHP_MSHUTDOWN(tdengine),		/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(tdengine),			/* PHP_RINIT - Request initialization */
	nullptr,						/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(tdengine),			/* PHP_MINFO - Module info */
	PHP_TDENGINE_VERSION,		/* Version */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_TDENGINE
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(tdengine)
#endif
