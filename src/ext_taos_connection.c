#include "ext_tdengine.h"
#include "ext_taos_connection.h"
#include "ext_taos_resource.h"
#include "Zend/zend_exceptions.h"

inline int is_connected(TDengineConnection *connection)
{
    return connection->connection ? 1 : 0;
}

inline void connection_close(TDengineConnection *connection)
{
    taos_close(connection->connection);
    connection->connection = NULL;
}

PHP_METHOD(TDengine_Connection, __construct) {
    TDengineConnection *connection = this_object(ConnectionObject);
    size_t host_len, user_len, pass_len, db_len;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|slsss!", &connection->host, &host_len, &connection->port, &connection->user, &user_len, &connection->pass, &pass_len, &connection->db, &db_len) == FAILURE) {
		RETURN_THROWS();
	}
    if (NULL == connection->host)
    {
        connection->host = "127.0.0.1";
    }
    if (NULL == connection->user)
    {
        connection->user = "root";
    }
    if (NULL == connection->pass)
    {
        connection->pass = "taosdata";
    }
    RETURN_TRUE;
}

PHP_METHOD(TDengine_Connection, connect) {
    TDengineConnection *connection = this_object(ConnectionObject);
    connection->connection = taos_connect(connection->host, connection->user, connection->pass, connection->db, connection->port);
    if (NULL == connection->connection)
    {
        throw_taos_exception(connection);
    }
}

PHP_METHOD(TDengine_Connection, close) {
    TDengineConnection *connection = this_object(ConnectionObject);
    check_connected(connection);
    connection_close(connection);
}

PHP_METHOD(TDengine_Connection, getHost) {
    TDengineConnection *connection = this_object(ConnectionObject);
    RETURN_STRING(connection->host);
}

PHP_METHOD(TDengine_Connection, getPort) {
    TDengineConnection *connection = this_object(ConnectionObject);
    RETURN_LONG(connection->port);
}

PHP_METHOD(TDengine_Connection, getUser) {
    TDengineConnection *connection = this_object(ConnectionObject);
    RETURN_STRING(connection->user);
}

PHP_METHOD(TDengine_Connection, getPass) {
    TDengineConnection *connection = this_object(ConnectionObject);
    RETURN_STRING(connection->pass);
}

PHP_METHOD(TDengine_Connection, getDb) {
    TDengineConnection *connection = this_object(ConnectionObject);
    if (connection->db)
    {
        RETURN_STRING(connection->db);
    }
    else
    {
        RETURN_NULL();
    }
}

PHP_METHOD(TDengine_Connection, getServerInfo) {
    TDengineConnection *connection = this_object(ConnectionObject);
    check_connected(connection);
    char *result = taos_get_server_info(connection->connection);
    if (result)
    {
        RETURN_STRING(result);
    }
    throw_taos_exception(connection);
}

PHP_METHOD(TDengine_Connection, selectDb) {
    char *db;
    size_t db_len;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &db, &db_len) == FAILURE) {
		RETURN_THROWS();
	}
    TDengineConnection *connection = this_object(ConnectionObject);
    check_connected(connection);
    int ret = taos_select_db(connection->connection, db);
    if(TSDB_CODE_SUCCESS != ret)
    {
        throw_taos_exception_by_errno(ret);
    }
}

PHP_METHOD(TDengine_Connection, query) {
    char *sql;
    size_t sql_len;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &sql, &sql_len) == FAILURE) {
		RETURN_THROWS();
	}
    TDengineConnection *connection = this_object(ConnectionObject);
    check_connected(connection);
    TAOS_RES *res = taos_query(connection->connection, sql);
    if (NULL == res)
    {
        throw_taos_exception(connection);
    }
	object_init_ex(return_value, TDengine_Resource_ce);
    TDengineResource *resource = zend_object_to_object_ptr(Z_OBJ_P(return_value), ResourceObject);
    resource->res = res;
    resource->connection = this_object_container(ConnectionObject);
    resource->sql = sql;
    GC_ADDREF(Z_OBJ_P(ZEND_THIS));
}
