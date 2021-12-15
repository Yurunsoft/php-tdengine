#include "ext_tdengine.h"
#include "ext_taos_resource.h"
#include "Zend/zend_exceptions.h"

inline int assert_res(TDengineResource *resource)
{
    return resource->res ? 1 : 0;
}

void close_resource(TDengineResource *resource)
{
    taos_stop_query(resource->res);
    taos_free_result(resource->res);
    resource->res = NULL;
}

inline void fetch(zval *zv, TDengineResource *resource)
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

inline int fetch_row(zval *zrow, TDengineResource *resource, TAOS_FIELD *fields, int field_count)
{
    TAOS_ROW row = taos_fetch_row(resource->res);
    if (!row)
    {
        return 0;
    }
    int32_t len;
    char *string_value;
    array_init_size(zrow, field_count);

    for(int i = 0; i < field_count; ++i)
    {
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
                add_assoc_double(zrow, fields[i].name, *((float *)row[i]));
                break;
            case TSDB_DATA_TYPE_DOUBLE:
                add_assoc_double(zrow, fields[i].name, *((double *)row[i]));
                break;
            case TSDB_DATA_TYPE_BINARY:
            case TSDB_DATA_TYPE_NCHAR:
                len = *((int16_t *)row[i]);
                string_value = emalloc(len);
                memcpy(string_value, row[i], len);
                add_assoc_string(zrow, fields[i].name, string_value);
                efree(string_value);
                break;
            case TSDB_DATA_TYPE_TIMESTAMP:
                add_assoc_long(zrow, fields[i].name, *((uint64_t *)row[i]));
                break;
                break;
            case TSDB_DATA_TYPE_UTINYINT:
                add_assoc_long(zrow, fields[i].name, *((uint8_t *)row[i]));
                break;
            case TSDB_DATA_TYPE_USMALLINT:
                add_assoc_long(zrow, fields[i].name, *((uint16_t *)row[i]));
                break;
            case TSDB_DATA_TYPE_UINT:
                add_assoc_long(zrow, fields[i].name, *((uint32_t *)row[i]));
                break;
            case TSDB_DATA_TYPE_UBIGINT:
                add_assoc_long(zrow, fields[i].name, *((uint64_t *)row[i]));
                break;
            default:
                zend_throw_exception_ex(TDengine_Exception_ce, 0, "Invalid field type %d", fields[i].type);
        }
    }

    return 1;
}

inline void fetch_fields(zval *zresult, TDengineResource *resource)
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

PHP_METHOD(TDengine_Resource, getSql) {
    TDengineResource *resource = this_object(ResourceObject);

    RETURN_STRING(resource->sql ? resource->sql : "");
}

PHP_METHOD(TDengine_Resource, getResultPrecision) {
    TDengineResource *resource = this_object(ResourceObject);
    check_res(resource);

    RETURN_LONG(taos_result_precision(resource->res));
}

PHP_METHOD(TDengine_Resource, fetch) {
    TDengineResource *resource = this_object(ResourceObject);
    check_res(resource);
    fetch(return_value, resource);
}

PHP_METHOD(TDengine_Resource, fetchRow) {
    TDengineResource *resource = this_object(ResourceObject);
    check_res(resource);
    fetch_row(return_value, resource, taos_fetch_fields(resource->res), taos_num_fields(resource->res));
}

PHP_METHOD(TDengine_Resource, getFieldCount) {
    TDengineResource *resource = this_object(ResourceObject);
    check_res(resource);
    RETURN_LONG(taos_num_fields(resource->res));
}

PHP_METHOD(TDengine_Resource, affectedRows) {
    TDengineResource *resource = this_object(ResourceObject);
    check_res(resource);
    RETURN_LONG(taos_affected_rows(resource->res));
}

PHP_METHOD(TDengine_Resource, fetchFields) {
    TDengineResource *resource = this_object(ResourceObject);
    check_res(resource);
    fetch_fields(return_value, resource);
}

PHP_METHOD(TDengine_Resource, close) {
    TDengineResource *resource = this_object(ResourceObject);
    check_res(resource);
    close_resource(resource);
}
