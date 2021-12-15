#include "ext_tdengine.h"
#include "ext_taos.h"

/* {{{ void setOptions() */
PHP_FUNCTION(setOptions)
{
	zval *options;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "a", &options) == FAILURE) {
		RETURN_THROWS();
	}
	zend_ulong    hashIndex;
	zval *hashData;
	zend_string *hashKey;

	ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(options), hashIndex, hashKey, hashData) {
        if (0 != taos_options((TSDB_OPTION)(zend_long)hashKey, Z_STRVAL_P(hashData)))
        {
            RETURN_FALSE;
        }
	} ZEND_HASH_FOREACH_END();
    RETURN_TRUE;
}
/* }}} */

/* {{{ void getClientInfo() */
PHP_FUNCTION(getClientInfo)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RETURN_STRING(taos_get_client_info());
}
/* }}} */
