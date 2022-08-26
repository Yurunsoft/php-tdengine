#ifndef PHP_EXT_TAOS_H
# define PHP_EXT_TAOS_H

#include "php_tdengine.h"
#include "ext_tdengine.h"

// TDengine functions
PHP_FUNCTION(setOptions);
PHP_FUNCTION(getClientInfo);
const zend_function_entry ext_functions[] = {
	ZEND_NS_FE("TDengine", setOptions, arginfo_TDengine_setOptions)
	ZEND_NS_FE("TDengine", getClientInfo, arginfo_TDengine_getClientInfo)
	ZEND_FE_END
};

inline void register_constants(int module_number)
{
    // version
    REGISTER_NS_STRING_CONSTANT("TDengine", "EXTENSION_VERSION", PHP_TDENGINE_VERSION, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_STRING_CONSTANT("TDengine", "CLIENT_VERSION", taos_get_client_info(), CONST_CS | CONST_PERSISTENT);

    // option
    REGISTER_NS_LONG_CONSTANT("TDengine", "TSDB_OPTION_LOCALE", TSDB_OPTION_LOCALE, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT("TDengine", "TSDB_OPTION_CHARSET", TSDB_OPTION_CHARSET, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT("TDengine", "TSDB_OPTION_TIMEZONE", TSDB_OPTION_TIMEZONE, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT("TDengine", "TSDB_OPTION_CONFIGDIR", TSDB_OPTION_CONFIGDIR, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT("TDengine", "TSDB_OPTION_SHELL_ACTIVITY_TIMER", TSDB_OPTION_SHELL_ACTIVITY_TIMER, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT("TDengine", "TSDB_MAX_OPTIONS", TSDB_MAX_OPTIONS, CONST_CS | CONST_PERSISTENT);

    // data type
    REGISTER_NS_LONG_CONSTANT("TDengine", "TSDB_DATA_TYPE_NULL", TSDB_DATA_TYPE_NULL, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT("TDengine", "TSDB_DATA_TYPE_BOOL", TSDB_DATA_TYPE_BOOL, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT("TDengine", "TSDB_DATA_TYPE_TINYINT", TSDB_DATA_TYPE_TINYINT, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT("TDengine", "TSDB_DATA_TYPE_SMALLINT", TSDB_DATA_TYPE_SMALLINT, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT("TDengine", "TSDB_DATA_TYPE_INT", TSDB_DATA_TYPE_INT, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT("TDengine", "TSDB_DATA_TYPE_BIGINT", TSDB_DATA_TYPE_BIGINT, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT("TDengine", "TSDB_DATA_TYPE_FLOAT", TSDB_DATA_TYPE_FLOAT, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT("TDengine", "TSDB_DATA_TYPE_DOUBLE", TSDB_DATA_TYPE_DOUBLE, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT("TDengine", "TSDB_DATA_TYPE_BINARY", TSDB_DATA_TYPE_BINARY, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT("TDengine", "TSDB_DATA_TYPE_TIMESTAMP", TSDB_DATA_TYPE_TIMESTAMP, CONST_CS | CONST_PERSISTENT);
    REGISTER_NS_LONG_CONSTANT("TDengine", "TSDB_DATA_TYPE_NCHAR", TSDB_DATA_TYPE_NCHAR, CONST_CS | CONST_PERSISTENT);
#ifdef TSDB_DATA_TYPE_UTINYINT
    REGISTER_NS_LONG_CONSTANT("TDengine", "TSDB_DATA_TYPE_UTINYINT", TSDB_DATA_TYPE_UTINYINT, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef TSDB_DATA_TYPE_USMALLINT
    REGISTER_NS_LONG_CONSTANT("TDengine", "TSDB_DATA_TYPE_USMALLINT", TSDB_DATA_TYPE_USMALLINT, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef TSDB_DATA_TYPE_UINT
    REGISTER_NS_LONG_CONSTANT("TDengine", "TSDB_DATA_TYPE_UINT", TSDB_DATA_TYPE_UINT, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef TSDB_DATA_TYPE_UBIGINT
    REGISTER_NS_LONG_CONSTANT("TDengine", "TSDB_DATA_TYPE_UBIGINT", TSDB_DATA_TYPE_UBIGINT, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef TSDB_DATA_TYPE_JSON
    REGISTER_NS_LONG_CONSTANT("TDengine", "TSDB_DATA_TYPE_JSON", TSDB_DATA_TYPE_JSON, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef TSDB_DATA_TYPE_VARBINARY
    REGISTER_NS_LONG_CONSTANT("TDengine", "TSDB_DATA_TYPE_VARBINARY", TSDB_DATA_TYPE_VARBINARY, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef TSDB_DATA_TYPE_DECIMAL
    REGISTER_NS_LONG_CONSTANT("TDengine", "TSDB_DATA_TYPE_DECIMAL", TSDB_DATA_TYPE_DECIMAL, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef TSDB_DATA_TYPE_BLOB
    REGISTER_NS_LONG_CONSTANT("TDengine", "TSDB_DATA_TYPE_BLOB", TSDB_DATA_TYPE_BLOB, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef TSDB_DATA_TYPE_MEDIUMBLOB
    REGISTER_NS_LONG_CONSTANT("TDengine", "TSDB_DATA_TYPE_MEDIUMBLOB", TSDB_DATA_TYPE_MEDIUMBLOB, CONST_CS | CONST_PERSISTENT);
#endif
#ifdef TSDB_DATA_TYPE_MAX
    REGISTER_NS_LONG_CONSTANT("TDengine", "TSDB_DATA_TYPE_MAX", TSDB_DATA_TYPE_MAX, CONST_CS | CONST_PERSISTENT);
#endif
}

#endif	/* PHP_EXT_TAOS_H */
