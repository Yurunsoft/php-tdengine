#ifndef PHP_EXT_TAOS_RESOURCE_H
# define PHP_EXT_TAOS_RESOURCE_H
#include "ext_taos_connection.h"
#include "ext_taos_statement.h"

typedef struct {
	TAOS_RES *res;
    ConnectionObject *connection;
    StatementObject *statement;
	const char *sql;
} TDengineResource;

typedef struct {
    TDengineResource *ptr;
    zend_object std;
} ResourceObject;

inline void close_resource(TDengineResource *resource)
{
	if (resource->res)
	{
		taos_stop_query(resource->res);
		taos_free_result(resource->res);
		resource->res = nullptr;
	}
}

bool fetch_row(zval *zrow, TDengineResource *resource, TAOS_FIELD *fields, int field_count);
void fetch(zval *zv, TDengineResource *resource);
void fetch_fields(zval *zresult, TDengineResource *resource);

// TDengine\Resource
ZEND_METHOD(TDengine_Resource, getConnection);
ZEND_METHOD(TDengine_Resource, getStatement);
ZEND_METHOD(TDengine_Resource, getSql);
ZEND_METHOD(TDengine_Resource, getResultPrecision);
ZEND_METHOD(TDengine_Resource, fetch);
ZEND_METHOD(TDengine_Resource, fetchRow);
ZEND_METHOD(TDengine_Resource, getFieldCount);
ZEND_METHOD(TDengine_Resource, affectedRows);
ZEND_METHOD(TDengine_Resource, fetchFields);
ZEND_METHOD(TDengine_Resource, close);

const zend_function_entry class_TDengine_Resource_methods[] = {
	ZEND_ME(TDengine_Resource, getConnection, arginfo_class_TDengine_Resource_getConnection, ZEND_ACC_PUBLIC)
	ZEND_ME(TDengine_Resource, getStatement, arginfo_class_TDengine_Resource_getStatement, ZEND_ACC_PUBLIC)
	ZEND_ME(TDengine_Resource, getSql, arginfo_class_TDengine_Resource_getSql, ZEND_ACC_PUBLIC)
	ZEND_ME(TDengine_Resource, getResultPrecision, arginfo_class_TDengine_Resource_getResultPrecision, ZEND_ACC_PUBLIC)
	ZEND_ME(TDengine_Resource, fetch, arginfo_class_TDengine_Resource_fetch, ZEND_ACC_PUBLIC)
	ZEND_ME(TDengine_Resource, fetchRow, arginfo_class_TDengine_Resource_fetchRow, ZEND_ACC_PUBLIC)
	ZEND_ME(TDengine_Resource, getFieldCount, arginfo_class_TDengine_Resource_getFieldCount, ZEND_ACC_PUBLIC)
	ZEND_ME(TDengine_Resource, affectedRows, arginfo_class_TDengine_Resource_affectedRows, ZEND_ACC_PUBLIC)
	ZEND_ME(TDengine_Resource, fetchFields, arginfo_class_TDengine_Resource_fetchFields, ZEND_ACC_PUBLIC)
	ZEND_ME(TDengine_Resource, close, arginfo_class_TDengine_Resource_close, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

extern PHP_TDENGINE_API zend_class_entry *TDengine_Resource_ce;
extern PHP_TDENGINE_API zend_object_handlers tdengine_resource_handlers;

inline zend_object *php_tdengine_resource_create_object(zend_class_entry *ce) {
    ResourceObject *obj = (ResourceObject *) zend_object_alloc(sizeof(ResourceObject), ce);
	obj->ptr = (TDengineResource*) emalloc(sizeof(TDengineResource));
	obj->ptr->res = nullptr;
	obj->ptr->connection = nullptr;
	obj->ptr->statement = nullptr;
	obj->ptr->sql = nullptr;
	zend_object *zobj = &obj->std;
	zend_object_std_init(zobj, ce);
	object_properties_init(zobj, ce);
	zobj->handlers = &tdengine_resource_handlers;
	return zobj;
}

inline void php_tdengine_resource_free_object(zend_object *zobj) {
	ResourceObject *obj = zend_object_to_object(zobj, ResourceObject);

	if (obj && obj->ptr)
	{
		TDengineResource *resource = obj->ptr;
		if (resource->res)
		{
			close_resource(resource);
		}
		if (resource->connection)
		{
			GC_DELREF(&resource->connection->std);
			resource->connection = nullptr;
		}
		if (resource->statement)
		{
			GC_DELREF(&resource->statement->std);
			resource->statement = nullptr;
		}
		resource->sql = nullptr;

		efree(resource);
		obj->ptr = nullptr;
	}

	zend_object_std_dtor(zobj);
}

inline zend_class_entry *register_class_TDengine_Resource(void)
{
	zend_class_entry ce;

	INIT_NS_CLASS_ENTRY(ce, "TDengine", "Resource", class_TDengine_Resource_methods);
	TDengine_Resource_ce = zend_register_internal_class_ex(&ce, nullptr);
	TDengine_Resource_ce->create_object = php_tdengine_resource_create_object;
	memcpy(&tdengine_resource_handlers, &std_object_handlers, sizeof(tdengine_resource_handlers));
	tdengine_resource_handlers.offset = XtOffsetOf(ResourceObject, std);
	tdengine_resource_handlers.free_obj = php_tdengine_resource_free_object;

	return TDengine_Resource_ce;
}

#endif	/* PHP_EXT_TAOS_RESOURCE_H */