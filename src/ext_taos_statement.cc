#include "ext_tdengine.h"
#include "ext_taos_statement.h"
#include "ext_taos_resource.h"

zend_class_entry *TDengine_Statement_ce;
zend_object_handlers tdengine_statement_handlers;
static int is_null = 1;

inline void parse_taos_bind(TAOS_BIND *bind, int data_type, zval *value)
{
    int16_t len;
    switch (data_type)
    {
        case TSDB_DATA_TYPE_NULL:
            bind->is_null = &is_null;
            break;
        case TSDB_DATA_TYPE_BOOL:
            bind->buffer = (int8_t*) emalloc(sizeof(int8_t));
            *((int8_t*) bind->buffer) = Z_TYPE_P(value) == IS_TRUE ? 1 : 0;
            bind->buffer_length = sizeof(int8_t);
            break;
        case TSDB_DATA_TYPE_TINYINT:
            bind->buffer = (int8_t*) emalloc(sizeof(int8_t));
            *((int8_t*) bind->buffer) = Z_LVAL_P(value);
            bind->buffer_length = sizeof(int8_t);
            break;
        case TSDB_DATA_TYPE_SMALLINT:
            bind->buffer = (int16_t*) emalloc(sizeof(int16_t));
            *((int16_t*) bind->buffer) = Z_LVAL_P(value);
            bind->buffer_length = sizeof(int16_t);
            break;
        case TSDB_DATA_TYPE_INT:
            bind->buffer = (int32_t*) emalloc(sizeof(int32_t));
            *((int32_t*) bind->buffer) = Z_LVAL_P(value);
            bind->buffer_length = sizeof(int32_t);
            break;
        case TSDB_DATA_TYPE_BIGINT:
            bind->buffer = (int64_t*) emalloc(sizeof(int64_t));
            *((int64_t*) bind->buffer) = Z_LVAL_P(value);
            bind->buffer_length = sizeof(int64_t);
            break;
        case TSDB_DATA_TYPE_FLOAT:
            bind->buffer = (float*) emalloc(sizeof(float));
            *((float*) bind->buffer) = Z_DVAL_P(value);
            bind->buffer_length = sizeof(float);
            break;
        case TSDB_DATA_TYPE_DOUBLE:
            bind->buffer = (double*) emalloc(sizeof(double));
            *((double*) bind->buffer) = Z_DVAL_P(value);
            bind->buffer_length = sizeof(double);
            break;
        case TSDB_DATA_TYPE_BINARY:
        case TSDB_DATA_TYPE_NCHAR:
            len = (int16_t) Z_STRLEN_P(value);
            bind->buffer = emalloc(sizeof(int16_t) + len);
	        memcpy(bind->buffer, &len, sizeof(int16_t));
	        memcpy((char *) bind->buffer + sizeof(int16_t), Z_STRVAL_P(value), len);
            break;
        case TSDB_DATA_TYPE_TIMESTAMP:
            bind->buffer = (uint64_t*) emalloc(sizeof(uint64_t));
            *((uint64_t*) bind->buffer) = Z_LVAL_P(value);
            bind->buffer_length = sizeof(uint64_t);
            break;
        case TSDB_DATA_TYPE_UTINYINT:
            bind->buffer = (uint8_t*) emalloc(sizeof(uint8_t));
            *((uint8_t*) bind->buffer) = Z_LVAL_P(value);
            bind->buffer_length = sizeof(uint8_t);
            break;
        case TSDB_DATA_TYPE_USMALLINT:
            bind->buffer = (uint16_t*) emalloc(sizeof(uint16_t));
            *((uint16_t*) bind->buffer) = Z_LVAL_P(value);
            bind->buffer_length = sizeof(uint16_t);
            break;
        case TSDB_DATA_TYPE_UINT:
            bind->buffer = (uint32_t*) emalloc(sizeof(uint32_t));
            *((uint32_t*) bind->buffer) = Z_LVAL_P(value);
            bind->buffer_length = sizeof(uint32_t);
            break;
        case TSDB_DATA_TYPE_UBIGINT:
            bind->buffer = (uint64_t*) emalloc(sizeof(uint64_t));
            *((uint64_t*) bind->buffer) = Z_LVAL_P(value);
            bind->buffer_length = sizeof(uint64_t);
            break;
        default:
            zend_throw_exception_ex(TDengine_Exception_ce, 0, "Invalid field type %d", data_type);
    }
    bind->buffer_type = data_type;
    bind->length = &bind->buffer_length;
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

PHP_METHOD(TDengine_Statement, bindParam) {
    int data_type;
	zval *value;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "lz", &data_type, &value) == FAILURE) {
		RETURN_THROWS();
	}
    TDengineStatement *statement = this_object(StatementObject);
    check_stmt(statement);
    close_statement(statement);

    TAOS_BIND *binds = (TAOS_BIND*) ecalloc(1, sizeof(TAOS_BIND));
    parse_taos_bind(&binds[0], data_type, value);

    int error = taos_stmt_bind_param(statement->stmt, binds);
    efree(binds);
    if (TSDB_CODE_SUCCESS != error)
    {
        throw_taos_exception_by_errno(error);
    }
}

PHP_METHOD(TDengine_Statement, execute) {
    TDengineStatement *statement = this_object(StatementObject);
    check_stmt(statement);
    close_statement(statement);

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
    if (NULL == res)
    {
        throw_taos_exception(statement->connection->ptr);
    }

	object_init_ex(return_value, TDengine_Resource_ce);
    TDengineResource *resource = zend_object_to_object_ptr(Z_OBJ_P(return_value), ResourceObject);
    resource->res = res;
    resource->connection = statement->connection;
    resource->sql = statement->sql;
    resource->statement = this_object_container(StatementObject);;
    GC_ADDREF(Z_OBJ_P(ZEND_THIS));
}
