#include "ext_taos_connection.h"
#include "ext_taos_resource.h"

PHP_TDENGINE_API zend_class_entry *TDengine_Connection_ce;
PHP_TDENGINE_API zend_object_handlers tdengine_connection_handlers;
PHP_TDENGINE_API zend_class_entry *TDengine_Exception_ce;
bool taos_inited = false;

PHP_METHOD(TDengine_Connection, __construct) {
    TDengineConnection *connection = this_object(ConnectionObject);
    size_t host_len, user_len, pass_len, db_len;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|slsss!", &connection->host, &host_len, &connection->port, &connection->user, &user_len, &connection->pass, &pass_len, &connection->db, &db_len) == FAILURE) {
		RETURN_THROWS();
	}
    if (nullptr == connection->host)
    {
        connection->host = const_cast<char*>("127.0.0.1");
    }
    if (nullptr == connection->user)
    {
        connection->user = const_cast<char*>("root");
    }
    if (nullptr == connection->pass)
    {
        connection->pass = const_cast<char*>("taosdata");
    }
    RETURN_TRUE;
}

PHP_METHOD(TDengine_Connection, connect) {
    TDengineConnection *connection = this_object(ConnectionObject);
#ifdef HAVE_SWOOLE
    if (Coroutine::get_current())
    {
        int error = TSDB_CODE_SUCCESS;
        swoole::coroutine::async([&]() {
            connection->connection = taos_connect(connection->host, connection->user, connection->pass, connection->db, connection->port);
            if (!connection->connection)
            {
                error = taos_errno(nullptr);
            }
        });
        if (TSDB_CODE_SUCCESS != error)
        {
            throw_taos_exception_by_errno(error);
        }
    }
    else
    {
#endif
        connection->connection = taos_connect(connection->host, connection->user, connection->pass, connection->db, connection->port);
        if (nullptr == connection->connection)
        {
            throw_taos_exception_by_connection(connection);
        }
#ifdef HAVE_SWOOLE
    }
#endif
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
    const char *result = nullptr;
#ifdef HAVE_SWOOLE
    if (Coroutine::get_current())
    {
        int error = TSDB_CODE_SUCCESS;
        swoole::coroutine::async([&]() {
            result = taos_get_server_info(connection->connection);
            if (result)
            {
                RETURN_STRING(result);
            }
            else
            {
                error = taos_errno(nullptr);
            }
            if (TSDB_CODE_SUCCESS != error)
            {
                throw_taos_exception_by_errno(error);
            }
        });
    }
    else
    {
#endif
        result = taos_get_server_info(connection->connection);
        if (result)
        {
            RETURN_STRING(result);
        }
        throw_taos_exception_by_connection(connection);
#ifdef HAVE_SWOOLE
    }
#endif
}

PHP_METHOD(TDengine_Connection, selectDb) {
    char *db;
    size_t db_len;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &db, &db_len) == FAILURE) {
		RETURN_THROWS();
	}
    TDengineConnection *connection = this_object(ConnectionObject);
    check_connected(connection);

    int ret;
#ifdef HAVE_SWOOLE
    if (Coroutine::get_current())
    {
        swoole::coroutine::async([&]() {
            ret = taos_select_db(connection->connection, db);
        });
    }
    else
    {
#endif
        ret = taos_select_db(connection->connection, db);
#ifdef HAVE_SWOOLE
    }
#endif

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

    TAOS_RES *res = nullptr;
    int error;
#ifdef HAVE_SWOOLE
    if (Coroutine::get_current())
    {
        swoole::coroutine::async([&]() {
            res = taos_query(connection->connection, sql);
            error = taos_errno(res);
            if (TSDB_CODE_SUCCESS != error && res)
            {
                taos_free_result(res);
            }
        });
        if (TSDB_CODE_SUCCESS != error)
        {
            throw_taos_exception_by_errno(error);
        }
    }
    else
    {
#endif
        res = taos_query(connection->connection, sql);
        error = taos_errno(res);
        if (TSDB_CODE_SUCCESS != error)
        {
            if (res)
            {
                taos_free_result(res);
            }
            throw_taos_exception_by_errno(error);
        }
#ifdef HAVE_SWOOLE
    }
#endif

	object_init_ex(return_value, TDengine_Resource_ce);
    TDengineResource *resource = zend_object_to_object_ptr(Z_OBJ_P(return_value), ResourceObject);
    resource->res = res;
    resource->connection = this_object_container(ConnectionObject);
    resource->sql = sql;
    GC_ADDREF(Z_OBJ_P(ZEND_THIS));
}

PHP_METHOD(TDengine_Connection, prepare) {
    char *sql;
    unsigned long sql_len;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &sql, &sql_len) == FAILURE) {
		RETURN_THROWS();
	}
    TDengineConnection *connection = this_object(ConnectionObject);
    check_connected(connection);

    TAOS_STMT *stmt = taos_stmt_init(connection->connection);
    int error;
#ifdef HAVE_SWOOLE
    if (Coroutine::get_current())
    {
        swoole::coroutine::async([&]() {
            error = taos_stmt_prepare(stmt, sql, sql_len);
            if (TSDB_CODE_SUCCESS != error)
            {
                taos_stmt_close(stmt);
            }
        });
        if (TSDB_CODE_SUCCESS != error)
        {
            throw_taos_exception_by_errno(error);
        }
    }
    else
    {
#endif
        error = taos_stmt_prepare(stmt, sql, sql_len);
        if (TSDB_CODE_SUCCESS != error)
        {
            const char* message = taos_stmt_errstr(stmt);
            taos_stmt_close(stmt);
            throw_taos_exception(message, error);
        }
#ifdef HAVE_SWOOLE
    }
#endif
    object_init_ex(return_value, TDengine_Statement_ce);
    TDengineStatement *statement = zend_object_to_object_ptr(Z_OBJ_P(return_value), StatementObject);
    statement->stmt = stmt;
    statement->connection = this_object_container(ConnectionObject);
    statement->sql = sql;
    GC_ADDREF(Z_OBJ_P(ZEND_THIS));
}

PHP_METHOD(TDengine_Connection, isConnected) {
    TDengineConnection *connection = this_object(ConnectionObject);
    RETURN_BOOL(is_connected(connection));
}
