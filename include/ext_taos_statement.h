#pragma once

#include "ext_tdengine.h"
#include "ext_taos_connection.h"

#define check_stmt(stmt) \
	if (!assert_stmt(stmt)) \
    { \
        zend_throw_exception_ex(TDengine_Exception_ce, 0, "Invalid stmt"); \
        RETURN_THROWS(); \
    }

typedef struct {
	TAOS_STMT *stmt;
    ConnectionObject *connection;
	const char *sql;
} TDengineStatement;

typedef struct {
    TDengineStatement *ptr;
    zend_object std;
} StatementObject;

inline int assert_stmt(TDengineStatement *stmt)
{
    return stmt->stmt ? 1 : 0;
}

inline void close_statement(TDengineStatement *stmt)
{
    taos_stmt_close(stmt->stmt);
    stmt->stmt = nullptr;
}

// TDengine\Statement
ZEND_METHOD(TDengine_Statement, getConnection);
ZEND_METHOD(TDengine_Statement, getSql);
ZEND_METHOD(TDengine_Statement, close);
ZEND_METHOD(TDengine_Statement, bindParams);
ZEND_METHOD(TDengine_Statement, execute);
ZEND_METHOD(TDengine_Statement, setTableNameTags);

const zend_function_entry class_TDengine_Statement_methods[] = {
	ZEND_ME(TDengine_Statement, getConnection, arginfo_class_TDengine_Statement_getConnection, ZEND_ACC_PUBLIC)
	ZEND_ME(TDengine_Statement, getSql, arginfo_class_TDengine_Statement_getSql, ZEND_ACC_PUBLIC)
	ZEND_ME(TDengine_Statement, close, arginfo_class_TDengine_Statement_close, ZEND_ACC_PUBLIC)
	ZEND_ME(TDengine_Statement, bindParams, arginfo_class_TDengine_Statement_bindParams, ZEND_ACC_PUBLIC)
	ZEND_ME(TDengine_Statement, execute, arginfo_class_TDengine_Statement_execute, ZEND_ACC_PUBLIC)
	ZEND_ME(TDengine_Statement, setTableNameTags, arginfo_class_TDengine_Statement_setTableNameTags, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

extern PHP_TDENGINE_API zend_class_entry *TDengine_Statement_ce;
extern PHP_TDENGINE_API zend_object_handlers tdengine_statement_handlers;

inline zend_object *php_tdengine_statement_create_object(zend_class_entry *ce) {
    StatementObject *obj = (StatementObject *) zend_object_alloc(sizeof(StatementObject), ce);
	obj->ptr = (TDengineStatement*) emalloc(sizeof(TDengineStatement));
	obj->ptr->stmt = nullptr;
	obj->ptr->connection = nullptr;
	obj->ptr->sql = nullptr;
	zend_object *zobj = &obj->std;
	zend_object_std_init(zobj, ce);
	object_properties_init(zobj, ce);
	zobj->handlers = &tdengine_statement_handlers;
	return zobj;
}

inline void php_tdengine_statement_free_object(zend_object *zobj) {
	StatementObject *obj = zend_object_to_object(zobj, StatementObject);

	if (obj && obj->ptr)
	{
		TDengineStatement *statement = obj->ptr;
		if (statement->stmt)
		{
			close_statement(statement);
		}
		if (statement->connection)
		{
			GC_DELREF(&statement->connection->std);
			statement->connection = nullptr;
		}
		obj->ptr->sql = nullptr;

		efree(statement);
		obj->ptr = nullptr;
	}

	zend_object_std_dtor(zobj);
}

inline zend_class_entry *register_class_TDengine_Statement(void)
{
	zend_class_entry ce;

	INIT_NS_CLASS_ENTRY(ce, "TDengine", "Statement", class_TDengine_Statement_methods);
	TDengine_Statement_ce = zend_register_internal_class_ex(&ce, nullptr);
	TDengine_Statement_ce->create_object = php_tdengine_statement_create_object;
	memcpy(&tdengine_statement_handlers, &std_object_handlers, sizeof(tdengine_statement_handlers));
	tdengine_statement_handlers.offset = XtOffsetOf(StatementObject, std);
	tdengine_statement_handlers.free_obj = php_tdengine_statement_free_object;

	return TDengine_Statement_ce;
}