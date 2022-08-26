#include "ext_tdengine.h"
#include "ext_taos_resource.h"

extern "C" {
#include "ext/standard/php_math.h"
}

PHP_TDENGINE_API zend_class_entry *TDengine_Resource_ce;
PHP_TDENGINE_API zend_object_handlers tdengine_resource_handlers;

bool fetch_row(zval *zrow, TDengineResource *resource, TAOS_FIELD *fields, int field_count)
{
    array_init_size(zrow, field_count);
    TAOS_ROW row = nullptr;
#ifdef HAVE_SWOOLE
    if (Coroutine::get_current())
    {
        swoole::coroutine::async([&]() {
            row = taos_fetch_row(resource->res);
        });
    }
    else
    {
#endif
        row = taos_fetch_row(resource->res);
#ifdef HAVE_SWOOLE
    }
#endif
    if (!row)
    {
        return false;
    }
    int16_t len;
    char *string_value;

    for(int i = 0; i < field_count; ++i)
    {
        if (nullptr == row[i])
        {
            add_assoc_null(zrow, fields[i].name);
            continue;
        }
        switch(fields[i].type)
        {
            case TSDB_DATA_TYPE_NULL:
                add_assoc_null(zrow, fields[i].name);
                break;
            case TSDB_DATA_TYPE_BOOL:
                add_assoc_bool(zrow, fields[i].name, 0 != *((int8_t *)row[i]));
                break;
            case TSDB_DATA_TYPE_TINYINT:
                add_assoc_long(zrow, fields[i].name, *((int8_t *)row[i]));
                break;
            case TSDB_DATA_TYPE_SMALLINT:
                add_assoc_long(zrow, fields[i].name, *((int16_t *)row[i]));
                break;
            case TSDB_DATA_TYPE_INT:
                add_assoc_long(zrow, fields[i].name, *((int32_t *)row[i]));
                break;
            case TSDB_DATA_TYPE_BIGINT:
                add_assoc_long(zrow, fields[i].name, *((int64_t *)row[i]));
                break;
            case TSDB_DATA_TYPE_FLOAT:
                add_assoc_double(zrow, fields[i].name, _php_math_round((double) *((float *)row[i]), 7, PHP_ROUND_HALF_DOWN));
                break;
            case TSDB_DATA_TYPE_DOUBLE:
                add_assoc_double(zrow, fields[i].name, *((double *)row[i]));
                break;
            case TSDB_DATA_TYPE_BINARY:
            case TSDB_DATA_TYPE_NCHAR:
#ifdef TSDB_DATA_TYPE_JSON
            case TSDB_DATA_TYPE_JSON:
#endif
                len = ((int16_t *)((char*)row[i] - sizeof(int16_t)))[0];
                string_value = (char *) emalloc(len);
                memcpy(string_value, row[i], len);
                add_assoc_stringl(zrow, fields[i].name, string_value, len);
                efree(string_value);
                break;
            case TSDB_DATA_TYPE_TIMESTAMP:
                add_assoc_long(zrow, fields[i].name, *((int64_t *)row[i]));
                break;
#ifdef TSDB_DATA_TYPE_UTINYINT
            case TSDB_DATA_TYPE_UTINYINT:
                add_assoc_long(zrow, fields[i].name, *((uint8_t *)row[i]));
                break;
#endif
#ifdef TSDB_DATA_TYPE_USMALLINT
            case TSDB_DATA_TYPE_USMALLINT:
                add_assoc_long(zrow, fields[i].name, *((uint16_t *)row[i]));
                break;
#endif
#ifdef TSDB_DATA_TYPE_UINT
            case TSDB_DATA_TYPE_UINT:
                add_assoc_long(zrow, fields[i].name, *((uint32_t *)row[i]));
                break;
#endif
#ifdef TSDB_DATA_TYPE_UBIGINT
            case TSDB_DATA_TYPE_UBIGINT:
                add_assoc_long(zrow, fields[i].name, *((uint64_t *)row[i]));
                break;
#endif
            default:
                zend_throw_exception_ex(TDengine_Exception_ce, 0, "Invalid field type %d", fields[i].type);
                return false;
        }
    }

    return true;
}

void fetch(zval *zv, TDengineResource *resource)
{
    array_init(zv);
    int field_count = taos_num_fields(resource->res);
    TAOS_FIELD *fields = taos_fetch_fields(resource->res);
    while (true)
    {
        zval zrow;
        if (fetch_row(&zrow, resource, fields, field_count))
        {
            add_next_index_zval(zv, &zrow);
        }
        else
        {
            break;
        }
    }
}

void fetch_fields(zval *zresult, TDengineResource *resource)
{
    TAOS_FIELD *fields = taos_fetch_fields(resource->res);
    int field_count = taos_num_fields(resource->res);
    array_init_size(zresult, field_count);
    for(int i = 0; i < field_count; ++i)
    {
        zval zrow;
        array_init_size(&zrow, 3);
        add_assoc_string(&zrow, "name", fields[i].name);
        add_assoc_long(&zrow, "type", fields[i].type);
        add_assoc_long(&zrow, "bytes", fields[i].bytes);
        add_next_index_zval(zresult, &zrow);
    }
}

PHP_METHOD(TDengine_Resource, getConnection) {
    TDengineResource *resource = this_object(ResourceObject);

    GC_ADDREF(&resource->connection->std);
    RETURN_OBJ(&resource->connection->std);
}

PHP_METHOD(TDengine_Resource, getStatement) {
    TDengineResource *resource = this_object(ResourceObject);

    if (resource->statement)
    {
        GC_ADDREF(&resource->statement->std);
        RETURN_OBJ(&resource->statement->std);
    }
    else
    {
        RETURN_NULL();
    }
}

PHP_METHOD(TDengine_Resource, getSql) {
    TDengineResource *resource = this_object(ResourceObject);

    RETURN_STRING(resource->sql ? resource->sql : "");
}

PHP_METHOD(TDengine_Resource, getResultPrecision) {
    TDengineResource *resource = this_object(ResourceObject);

    RETURN_LONG(resource->res ? taos_result_precision(resource->res) : 0);
}

PHP_METHOD(TDengine_Resource, fetch) {
    TDengineResource *resource = this_object(ResourceObject);
    fetch(return_value, resource);
}

PHP_METHOD(TDengine_Resource, fetchRow) {
    TDengineResource *resource = this_object(ResourceObject);
    if (resource->res)
    {
        fetch_row(return_value, resource, taos_fetch_fields(resource->res), taos_num_fields(resource->res));
    }
    else
    {
        array_init_size(return_value, 0);
    }
}

PHP_METHOD(TDengine_Resource, getFieldCount) {
    TDengineResource *resource = this_object(ResourceObject);
    RETURN_LONG(resource->res ? taos_num_fields(resource->res) : 0);
}

PHP_METHOD(TDengine_Resource, affectedRows) {
    TDengineResource *resource = this_object(ResourceObject);
    if (resource->statement)
    {
        RETURN_LONG(taos_stmt_affected_rows(resource->statement->ptr->stmt));
    }
    RETURN_LONG(resource->res ? taos_affected_rows(resource->res) : 0);
}

PHP_METHOD(TDengine_Resource, fetchFields) {
    TDengineResource *resource = this_object(ResourceObject);
    if (resource->res)
    {
        fetch_fields(return_value, resource);
    }
    else
    {
        array_init_size(return_value, 0);
    }
}

PHP_METHOD(TDengine_Resource, close) {
    TDengineResource *resource = this_object(ResourceObject);
    close_resource(resource);
}
