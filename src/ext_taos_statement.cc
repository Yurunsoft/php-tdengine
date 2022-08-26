#include "ext_tdengine.h"
#include "ext_taos_statement.h"
#include "ext_taos_resource.h"

PHP_TDENGINE_API zend_class_entry *TDengine_Statement_ce;
PHP_TDENGINE_API zend_object_handlers tdengine_statement_handlers;
#if HAVE_TAOS_BIND
static int is_null = 1;
#else
static char is_null = 1;
#endif

inline bool parse_taos_bind(TAOS_BIND *bind, int data_type, zval *value)
{
    bind->is_null = nullptr;
    zend_string *str;
    switch (data_type)
    {
        case TSDB_DATA_TYPE_NULL:
            bind->is_null = &is_null;
            bind->buffer_length = 0;
            break;
        case TSDB_DATA_TYPE_BOOL:
            bind->buffer = (int8_t*) emalloc(sizeof(int8_t));
            *((int8_t*) bind->buffer) = Z_TYPE_P(value) == IS_TRUE ? 1 : 0;
            bind->buffer_length = sizeof(int8_t);
            break;
        case TSDB_DATA_TYPE_TINYINT:
            bind->buffer = (int8_t*) emalloc(sizeof(int8_t));
            *((int8_t*) bind->buffer) = (int8_t) zval_get_long(value);
            bind->buffer_length = sizeof(int8_t);
            break;
        case TSDB_DATA_TYPE_SMALLINT:
            bind->buffer = (int16_t*) emalloc(sizeof(int16_t));
            *((int16_t*) bind->buffer) = (int16_t) zval_get_long(value);
            bind->buffer_length = sizeof(int16_t);
            break;
        case TSDB_DATA_TYPE_INT:
            bind->buffer = (int32_t*) emalloc(sizeof(int32_t));
            *((int32_t*) bind->buffer) = (int32_t) zval_get_long(value);
            bind->buffer_length = sizeof(int32_t);
            break;
        case TSDB_DATA_TYPE_BIGINT:
            bind->buffer = (int64_t*) emalloc(sizeof(int64_t));
            *((int64_t*) bind->buffer) = (int64_t) zval_get_long(value);
            bind->buffer_length = sizeof(int64_t);
            break;
        case TSDB_DATA_TYPE_FLOAT:
            bind->buffer = (float*) emalloc(sizeof(float));
            *((float*) bind->buffer) = (float) zval_get_double(value);
            bind->buffer_length = sizeof(float);
            break;
        case TSDB_DATA_TYPE_DOUBLE:
            bind->buffer = (double*) emalloc(sizeof(double));
            *((double*) bind->buffer) = zval_get_double(value);
            bind->buffer_length = sizeof(double);
            break;
        case TSDB_DATA_TYPE_BINARY:
        case TSDB_DATA_TYPE_NCHAR:
#ifdef TSDB_DATA_TYPE_JSON
        case TSDB_DATA_TYPE_JSON:
#endif
            str = zval_get_string(value);
            bind->buffer_length = (int16_t) ZSTR_LEN(str);
            bind->buffer = emalloc(bind->buffer_length);
            memcpy(bind->buffer, ZSTR_VAL(str), bind->buffer_length);
            zend_string_release(str);
            break;
        case TSDB_DATA_TYPE_TIMESTAMP:
            bind->buffer = (int64_t*) emalloc(sizeof(int64_t));
            *((int64_t*) bind->buffer) = zval_get_long(value);
            bind->buffer_length = sizeof(int64_t);
            break;
#ifdef TSDB_DATA_TYPE_UTINYINT
        case TSDB_DATA_TYPE_UTINYINT:
            bind->buffer = (uint8_t*) emalloc(sizeof(uint8_t));
            *((uint8_t*) bind->buffer) = (uint8_t) zval_get_long(value);
            bind->buffer_length = sizeof(uint8_t);
            break;
#endif
#ifdef TSDB_DATA_TYPE_USMALLINT
        case TSDB_DATA_TYPE_USMALLINT:
            bind->buffer = (uint16_t*) emalloc(sizeof(uint16_t));
            *((uint16_t*) bind->buffer) = (uint16_t) zval_get_long(value);
            bind->buffer_length = sizeof(uint16_t);
            break;
#endif
#ifdef TSDB_DATA_TYPE_UINT
        case TSDB_DATA_TYPE_UINT:
            bind->buffer = (uint32_t*) emalloc(sizeof(uint32_t));
            *((uint32_t*) bind->buffer) = (uint32_t) zval_get_long(value);
            bind->buffer_length = sizeof(uint32_t);
            break;
#endif
#ifdef TSDB_DATA_TYPE_UBIGINT
        case TSDB_DATA_TYPE_UBIGINT:
            bind->buffer = (uint64_t*) emalloc(sizeof(uint64_t));
            *((uint64_t*) bind->buffer) = (uint64_t) zval_get_long(value);
            bind->buffer_length = sizeof(uint64_t);
            break;
#endif
        default:
            zend_throw_exception_ex(TDengine_Exception_ce, 0, "Invalid field type %d", data_type);
            return false;
    }
    bind->buffer_type = data_type;
#if HAVE_TAOS_BIND
    bind->length = &bind->buffer_length;
#else
    bind->num = 1;
    bind->length = (int32_t*) &bind->buffer_length;
#endif
    return true;
}

PHP_METHOD(TDengine_Statement, getConnection) {
    TDengineStatement *statement = this_object(StatementObject);

    GC_ADDREF(&statement->connection->std);
    RETURN_OBJ(&statement->connection->std);
}

PHP_METHOD(TDengine_Statement, getSql) {
    TDengineStatement *statement = this_object(StatementObject);

    RETURN_STRING(statement->sql ? statement->sql : "");
}

PHP_METHOD(TDengine_Statement, close) {
    TDengineStatement *statement = this_object(StatementObject);
    check_stmt(statement);
    close_statement(statement);
}

PHP_METHOD(TDengine_Statement, bindParams) {
	zval *params;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "a", &params) == FAILURE) {
		RETURN_THROWS();
	}
    HashTable *params_ht = Z_ARRVAL_P(params);
    TDengineStatement *statement = this_object(StatementObject);
    check_stmt(statement);

    TAOS_BIND *binds = (TAOS_BIND*) ecalloc(zend_array_count(params_ht), sizeof(TAOS_BIND));
    zval *row, *type, *value;
    size_t index;
    bool success = true;
	ZEND_HASH_FOREACH_NUM_KEY_VAL(params_ht, index, row) {
        if (
            ((type = zend_hash_str_find(Z_ARRVAL_P(row), ZEND_STRL("type"))) && (value = zend_hash_str_find(Z_ARRVAL_P(row), ZEND_STRL("value"))))
            || ((type = zend_hash_index_find(Z_ARRVAL_P(row), 0)) && (value = zend_hash_index_find(Z_ARRVAL_P(row), 1)))
        )
        {
            if (!parse_taos_bind(&binds[index], (int) zval_get_long(type), value))
            {
                success = false;
                break;
            }
        }
        else
        {
            success = false;
            zend_throw_exception(spl_ce_InvalidArgumentException, "Invalid bind params", 0);
            break;
        }
	} ZEND_HASH_FOREACH_END();

    if (success)
    {
        int error = taos_stmt_bind_param(statement->stmt, binds);
        efree(binds);
        if (TSDB_CODE_SUCCESS != error)
        {
            throw_taos_exception_by_errno(error);
        }
        int is_insert;
        error = taos_stmt_is_insert(statement->stmt, &is_insert);
        if (TSDB_CODE_SUCCESS == error)
        {
            if (is_insert)
            {
                error = taos_stmt_add_batch(statement->stmt);
                if (TSDB_CODE_SUCCESS != error)
                {
                    throw_taos_exception_by_errno(error);
                }
            }
        }
        else
        {
            throw_taos_exception_by_errno(error);
        }
    }
    else
    {
        efree(binds);
    }
}

PHP_METHOD(TDengine_Statement, execute) {
    TDengineStatement *statement = this_object(StatementObject);
    check_stmt(statement);

    int error = TSDB_CODE_SUCCESS;
#ifdef HAVE_SWOOLE
    if (Coroutine::get_current())
    {
        swoole::coroutine::async([&]() {
            error = taos_stmt_execute(statement->stmt);
        });
    }
    else
    {
#endif
        error = taos_stmt_execute(statement->stmt);
#ifdef HAVE_SWOOLE
    }
#endif

    if (TSDB_CODE_SUCCESS != error)
    {
        throw_taos_exception_by_errno(error);
    }

    TAOS_RES *res = taos_stmt_use_result(statement->stmt);

	object_init_ex(return_value, TDengine_Resource_ce);
    TDengineResource *resource = zend_object_to_object_ptr(Z_OBJ_P(return_value), ResourceObject);
    resource->res = res;
    resource->connection = statement->connection;
    resource->sql = statement->sql;
    resource->statement = this_object_container(StatementObject);
    GC_ADDREF(&statement->connection->std);
    GC_ADDREF(Z_OBJ_P(ZEND_THIS));
}

PHP_METHOD(TDengine_Statement, setTableNameTags) {
    char *table_name;
    size_t table_name_len;
	zval *tags;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|a", &table_name, &table_name_len, &tags) == FAILURE) {
		RETURN_THROWS();
	}
    HashTable *tags_ht = Z_ARRVAL_P(tags);
    TDengineStatement *statement = this_object(StatementObject);
    check_stmt(statement);

    TAOS_BIND *binds = (TAOS_BIND*) ecalloc(zend_array_count(tags_ht), sizeof(TAOS_BIND));
    zval *row, *type, *value;
    size_t index;
    bool success = true;
	ZEND_HASH_FOREACH_NUM_KEY_VAL(tags_ht, index, row) {
        if (
            ((type = zend_hash_str_find(Z_ARRVAL_P(row), ZEND_STRL("type"))) && (value = zend_hash_str_find(Z_ARRVAL_P(row), ZEND_STRL("value"))))
            || ((type = zend_hash_index_find(Z_ARRVAL_P(row), 0)) && (value = zend_hash_index_find(Z_ARRVAL_P(row), 1)))
        )
        {
            if (!parse_taos_bind(&binds[index], (int) zval_get_long(type), value))
            {
                success = false;
                break;
            }
        }
        else
        {
            success = false;
            zend_throw_exception(spl_ce_InvalidArgumentException, "Invalid bind params", 0);
            break;
        }
	} ZEND_HASH_FOREACH_END();

    if (success)
    {
        int error = taos_stmt_set_tbname_tags(statement->stmt, table_name, binds);
        efree(binds);
        if (TSDB_CODE_SUCCESS != error)
        {
            throw_taos_exception_by_errno(error);
        }
    }
    else
    {
        efree(binds);
    }
}
