# Apache IoTDB REST client

`IoTDB\RestConnection` is a separate PHP userland HTTP client. It does not require or replace the native extension.

It uses Apache IoTDB's documented Basic-auth REST endpoints:

| Model | API | Query | Non-query and SQL writes | Structured tablet write |
| --- | --- | --- | --- | --- |
| Tree | REST v1 | `/rest/v1/query` | `/rest/v1/nonQuery` | `/rest/v1/insertTablet` |
| Tree | REST v2 | `/rest/v2/query` | `/rest/v2/nonQuery` | `/rest/v2/insertTablet` |
| Table | REST table v1 | `/rest/table/v1/query` | `/rest/table/v1/nonQuery` | `/rest/table/v1/insertTablet` |

The Table REST API is v1 only in this client. The server must have its REST service enabled and expose the relevant API for its IoTDB version. The default endpoint port is commonly `18080`, but the client requires the caller to set it explicitly.

The client supports PHP 7.4 or newer and uses the built-in JSON and HTTP stream APIs (`allow_url_fopen` must be enabled). HTTPS also requires the PHP OpenSSL extension and a trusted CA chain in PHP's certificate configuration.

## Use

```php
require_once __DIR__ . '/iotdb/RestConnection.php';

use IoTDB\RestConnection;

$client = new RestConnection([
    'endpoint' => 'https://iotdb.example:18080',
    'username' => 'root',
    'password' => getenv('IOTDB_PASSWORD'),
    'model' => 'tree',       // tree or table
    'apiVersion' => 2,       // Tree: 1 or 2; Table: 1 only
    'timeout' => 5,
    'rowLimit' => 500,
]);

$rows = $client->query('SELECT s1 FROM root.sg.d1');
$client->nonQuery('INSERT INTO root.sg.d1(time, s1) VALUES(1, 42)');

$client->insertTablet([
    'device' => 'root.sg.d1',
    'timestamps' => [1, 2],
    'measurements' => ['s1'],
    'dataTypes' => ['INT32'],
    'values' => [[42], [43]],
    'isAligned' => false,
]);
```

For Table model, set `model` to `table`, `apiVersion` to `1`, and provide `database` either at construction or with `useDatabase()`. A Table tablet batch takes `table`, `timestamps`, `columnNames`, `columnCategories` (`TAG`, `ATTRIBUTE`, `FIELD`), `dataTypes`, and row-major `values`; its database comes from the selected client database.

`query()` returns the decoded IoTDB JSON response unchanged. The Tree v1, Tree v2, and Table v1 result payloads differ, so callers should use the response field names for the selected endpoint. Query row limits are bounded to 1 through 1,000,000 in the client and may be capped further by server configuration. `nonQuery()` supports SQL inserts and DDL through the documented non-query endpoint. `insertTablet()` validates the batch shape and maps it to the documented version-specific endpoint. Both operations throw `IoTDB\RestException` for HTTP errors, malformed JSON, or an IoTDB status code other than 200. The exception exposes `getHttpStatus()`, `getServerCode()`, and `getResponse()`.

## Limits

- This client is synchronous and makes one HTTP POST per operation. It does not implement connection pooling, native sessions, automatic retries, or prepared statements; writes are never retried automatically.
- REST authentication in the documented OpenAPI is HTTP Basic authentication. Use HTTPS where credentials traverse an untrusted network. TLS peer and host verification stay enabled.
- The current IoTDB REST authorization filter decodes the Basic value and splits it on `:`; credentials containing a colon may be rejected by the server even though standard Basic authentication permits it.
- The client does not translate SQL dialects or map Tree paths to Table schemas. SQL is sent to IoTDB as supplied.
- IoTDB REST availability and individual routes depend on server version and configuration. An unsupported route is reported as a server/HTTP error; the client does not fall back to another API automatically.
- The tests use a local PHP built-in HTTP fixture to validate request paths, payload fields, authentication, error handling, and timeout behavior. They do not claim to replace an end-to-end test against an installed IoTDB server.

Run the focused client tests with:

```shell
php tests/iotdb-rest-test.php
```

These files use the repository's AGPL-3.0 license. The upstream API contracts are published in Apache IoTDB's [REST OpenAPI specifications](https://github.com/apache/iotdb/tree/master/external-service-impl/rest-openapi/src/main/openapi3).
