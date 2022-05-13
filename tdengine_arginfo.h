/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 9e8c61abd73451a7785702c10a4a8ff4759fbb32 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_TDengine_setOptions, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, options, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_TDengine_getClientInfo, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_TDengine_Connection___construct, 0, 0, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, host, IS_STRING, 0, "\'127.0.0.1\'")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, port, IS_LONG, 0, "6030")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, user, IS_STRING, 0, "\'root\'")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, pass, IS_STRING, 0, "\'taosdata\'")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, db, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_TDengine_Connection_connect, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_TDengine_Connection_isConnected, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_TDengine_Connection_close arginfo_class_TDengine_Connection_connect

#define arginfo_class_TDengine_Connection_getHost arginfo_TDengine_getClientInfo

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_TDengine_Connection_getPort, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_TDengine_Connection_getUser arginfo_TDengine_getClientInfo

#define arginfo_class_TDengine_Connection_getPass arginfo_TDengine_getClientInfo

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_TDengine_Connection_getDb, 0, 0, IS_STRING, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_TDengine_Connection_getServerInfo arginfo_TDengine_getClientInfo

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_TDengine_Connection_selectDb, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, db, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_TDengine_Connection_query, 0, 1, TDengine\\Resource, 0)
	ZEND_ARG_TYPE_INFO(0, sql, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_TDengine_Connection_prepare, 0, 1, TDengine\\Statement, 0)
	ZEND_ARG_TYPE_INFO(0, sql, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_TDengine_Resource_getConnection, 0, 0, TDengine\\Connection, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_TDengine_Resource_getStatement, 0, 0, TDengine\\Statement, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_TDengine_Resource_getSql arginfo_TDengine_getClientInfo

#define arginfo_class_TDengine_Resource_getResultPrecision arginfo_class_TDengine_Connection_getPort

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_TDengine_Resource_fetch, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_TDengine_Resource_fetchRow arginfo_class_TDengine_Resource_fetch

#define arginfo_class_TDengine_Resource_getFieldCount arginfo_class_TDengine_Connection_getPort

#define arginfo_class_TDengine_Resource_affectedRows arginfo_class_TDengine_Connection_getPort

#define arginfo_class_TDengine_Resource_fetchFields arginfo_class_TDengine_Resource_fetch

#define arginfo_class_TDengine_Resource_close arginfo_class_TDengine_Connection_connect

#define arginfo_class_TDengine_Statement_getConnection arginfo_class_TDengine_Resource_getConnection

#define arginfo_class_TDengine_Statement_getSql arginfo_TDengine_getClientInfo

#define arginfo_class_TDengine_Statement_close arginfo_class_TDengine_Connection_connect

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_TDengine_Statement_bindParams, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, params, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_TDengine_Statement_execute, 0, 0, TDengine\\Resource, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_TDengine_Statement_setTableNameTags, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, tableName, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, tags, IS_ARRAY, 0, "[]")
ZEND_END_ARG_INFO()
