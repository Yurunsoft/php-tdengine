#ifndef PHP_EXT_TAOS_CONNECTION_H
# define PHP_EXT_TAOS_CONNECTION_H
#include "ext_tdengine.h"

#define check_connected(connection) \
	if (!is_connected(connection)) \
    { \
        zend_throw_exception_ex(TDengine_Exception_ce, 0, "Not connected"); \
        RETURN_THROWS(); \
    }

#define throw_taos_exception(_connection) \
	zend_throw_exception_ex(TDengine_Exception_ce, taos_errno(_connection->connection), "%s", taos_errstr(_connection->connection)); \
	RETURN_THROWS();

#define throw_taos_exception_by_errno(errno) \
	zend_throw_exception_ex(TDengine_Exception_ce, errno, "%s", tstrerror(errno)); \
	RETURN_THROWS();

typedef struct {
	char *host;
	int port;
	char *user;
	char *pass;
	char *db;
	TAOS *connection;
} TDengineConnection;

inline int is_connected(TDengineConnection *connection)
{
    return connection->connection ? 1 : 0;
}

inline void connection_close(TDengineConnection *connection)
{
    taos_close(connection->connection);
    connection->connection = nullptr;
}

// TDengine\Connection
ZEND_METHOD(TDengine_Connection, __construct);
ZEND_METHOD(TDengine_Connection, connect);
ZEND_METHOD(TDengine_Connection, close);
ZEND_METHOD(TDengine_Connection, getHost);
ZEND_METHOD(TDengine_Connection, getPort);
ZEND_METHOD(TDengine_Connection, getUser);
ZEND_METHOD(TDengine_Connection, getPass);
ZEND_METHOD(TDengine_Connection, getDb);
ZEND_METHOD(TDengine_Connection, getServerInfo);
ZEND_METHOD(TDengine_Connection, selectDb);
ZEND_METHOD(TDengine_Connection, query);
ZEND_METHOD(TDengine_Connection, prepare);

typedef struct {
    TDengineConnection *ptr;
    zend_object std;
} ConnectionObject;

const zend_function_entry class_TDengine_Connection_methods[] = {
	ZEND_ME(TDengine_Connection, __construct, arginfo_class_TDengine_Connection___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(TDengine_Connection, connect, arginfo_class_TDengine_Connection_connect, ZEND_ACC_PUBLIC)
	ZEND_ME(TDengine_Connection, close, arginfo_class_TDengine_Connection_close, ZEND_ACC_PUBLIC)
	ZEND_ME(TDengine_Connection, getHost, arginfo_class_TDengine_Connection_getHost, ZEND_ACC_PUBLIC)
	ZEND_ME(TDengine_Connection, getPort, arginfo_class_TDengine_Connection_getPort, ZEND_ACC_PUBLIC)
	ZEND_ME(TDengine_Connection, getUser, arginfo_class_TDengine_Connection_getUser, ZEND_ACC_PUBLIC)
	ZEND_ME(TDengine_Connection, getPass, arginfo_class_TDengine_Connection_getPass, ZEND_ACC_PUBLIC)
	ZEND_ME(TDengine_Connection, getDb, arginfo_class_TDengine_Connection_getDb, ZEND_ACC_PUBLIC)
	ZEND_ME(TDengine_Connection, getServerInfo, arginfo_class_TDengine_Connection_getServerInfo, ZEND_ACC_PUBLIC)
	ZEND_ME(TDengine_Connection, selectDb, arginfo_class_TDengine_Connection_selectDb, ZEND_ACC_PUBLIC)
	ZEND_ME(TDengine_Connection, query, arginfo_class_TDengine_Connection_query, ZEND_ACC_PUBLIC)
	ZEND_ME(TDengine_Connection, prepare, arginfo_class_TDengine_Connection_prepare, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

extern zend_class_entry *TDengine_Connection_ce;
extern zend_object_handlers tdengine_connection_handlers;

extern bool taos_inited;

inline zend_object *php_tdengine_connection_create_object(zend_class_entry *ce) {
	if (!taos_inited)
	{
		std::thread([]() {
			swoole_signal_block_all();
			taos_init();
		}).join();
		taos_inited = true;
	}
    ConnectionObject *obj = (ConnectionObject *) zend_object_alloc(sizeof(ConnectionObject), ce);
	TDengineConnection *connection = (TDengineConnection*) emalloc(sizeof(TDengineConnection));
	connection->host = nullptr;
    connection->port = 6030;
    connection->user = nullptr;
    connection->pass = nullptr;
    connection->db = nullptr;
    connection->connection = nullptr;
	zend_object *zobj = &obj->std;
	obj->ptr = connection;
	zend_object_std_init(zobj, ce);
	object_properties_init(zobj, ce);
	zobj->handlers = &tdengine_connection_handlers;
	return zobj;
}

inline void php_tdengine_connection_free_object(zend_object *zobj) {
	ConnectionObject *obj = zend_object_to_object(zobj, ConnectionObject);

	if (obj && obj->ptr)
	{
		TDengineConnection *connection = obj->ptr;
		if (connection && is_connected(connection))
		{
			connection_close(connection);
		}
	}

	zend_object_std_dtor(zobj);
}

inline zend_class_entry *register_class_TDengine_Connection(void)
{
	zend_class_entry ce;

	INIT_NS_CLASS_ENTRY(ce, "TDengine", "Connection", class_TDengine_Connection_methods);
	TDengine_Connection_ce = zend_register_internal_class_ex(&ce, nullptr);
	TDengine_Connection_ce->create_object = php_tdengine_connection_create_object;
	memcpy(&tdengine_connection_handlers, &std_object_handlers, sizeof(tdengine_connection_handlers));
	tdengine_connection_handlers.offset = XtOffsetOf(ConnectionObject, std);
	tdengine_connection_handlers.free_obj = php_tdengine_connection_free_object;

	return TDengine_Connection_ce;
}

// TDengine\Exception\TDengineException
extern zend_class_entry *TDengine_Exception_ce;

const zend_function_entry class_TDengine_Exception_methods[] = {
	ZEND_FE_END
};

inline zend_class_entry *register_class_TDengine_Exception(zend_class_entry *class_entry_Exception)
{
	zend_class_entry ce;

	INIT_NS_CLASS_ENTRY(ce, "TDengine\\Exception", "TDengineException", class_TDengine_Exception_methods);
	TDengine_Exception_ce = zend_register_internal_class_ex(&ce, class_entry_Exception);

	return TDengine_Exception_ce;
}

#endif	/* PHP_EXT_TAOS_CONNECTION_H */