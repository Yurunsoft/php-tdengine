#include "ext_tdengine.h"
#include "ext_taos.h"

/* {{{ void setOptions() */
PHP_FUNCTION(setOptions)
{
	zval *options;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "a", &options) == FAILURE) {
		RETURN_THROWS();
	}
	zval *hashData;
	zend_string *hashKey, *str;

	ZEND_HASH_FOREACH_STR_KEY_VAL_IND(Z_ARRVAL_P(options), hashKey, hashData) {
		str = zval_get_string(hashData);
        if (0 != taos_options((TSDB_OPTION)(zend_long)hashKey, ZSTR_VAL(str)))
        {
			zend_string_release(str);
            RETURN_FALSE;
        }
		zend_string_release(str);
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
