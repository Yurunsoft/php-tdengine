<?php

require_once __DIR__ . '/../iotdb/RestConnection.php';

use IoTDB\RestConnection;
use IoTDB\RestException;

function assertSameValue($expected, $actual, $label)
{
    if ($expected !== $actual) {
        throw new RuntimeException($label . ': expected ' . var_export($expected, true) . ', got ' . var_export($actual, true));
    }
}

function assertTrueValue($condition, $label)
{
    if (!$condition) {
        throw new RuntimeException($label);
    }
}

function createClient($port, array $options = [])
{
    return new RestConnection(array_merge([
        'endpoint' => 'http://127.0.0.1:' . $port,
        'username' => 'root',
        'password' => 'secret',
        'timeout' => 3,
        'rowLimit' => 100,
    ], $options));
}

$socket = stream_socket_server('tcp://127.0.0.1:0', $errorCode, $errorMessage);
if ($socket === false) {
    throw new RuntimeException('Unable to reserve a local port: ' . $errorMessage);
}
$address = stream_socket_get_name($socket, false);
fclose($socket);
if (!preg_match('/:(\d+)$/', $address, $matches)) {
    throw new RuntimeException('Unable to determine the local test port');
}
$port = (int) $matches[1];
$pipes = [];
$process = proc_open(
    [PHP_BINARY, '-S', '127.0.0.1:' . $port, __DIR__ . '/iotdb-rest-fixture.php'],
    [0 => ['pipe', 'r'], 1 => ['pipe', 'w'], 2 => ['pipe', 'w']],
    $pipes
);
if (!is_resource($process)) {
    throw new RuntimeException('Unable to start the local HTTP fixture');
}
stream_set_blocking($pipes[1], false);
stream_set_blocking($pipes[2], false);

try {
    $ready = false;
    for ($attempt = 0; $attempt < 100; ++$attempt) {
        $probe = @fsockopen('127.0.0.1', $port, $probeErrorCode, $probeErrorMessage, 0.1);
        if ($probe !== false) {
            fclose($probe);
            $ready = true;
            break;
        }
        usleep(50000);
    }
    assertTrueValue($ready, 'local HTTP fixture did not start');

    $treeV1 = createClient($port, ['apiVersion' => 1]);
    $query = $treeV1->query('SELECT s1 FROM root.sg.d1', 17);
    assertSameValue(42, $query['values'][0][0], 'Tree v1 query data');
    assertSameValue('/rest/v1/query', $query['receivedPath'], 'Tree v1 query route');
    assertSameValue(17, $query['received']['rowLimit'], 'Tree v1 row limit field');
    assertSameValue('Basic ' . base64_encode('root:secret'), $query['receivedAuthorization'], 'Basic authentication');

    $treeV1Insert = $treeV1->insertTablet([
        'device' => 'root.sg.d1',
        'timestamps' => [1],
        'measurements' => ['s1'],
        'dataTypes' => ['INT32'],
        'values' => [[42]],
        'isAligned' => false,
    ]);
    assertSameValue('/rest/v1/insertTablet', $treeV1Insert['receivedPath'], 'Tree v1 insert route');
    assertSameValue('root.sg.d1', $treeV1Insert['received']['deviceId'], 'Tree v1 device field');
    assertSameValue(['INT32'], $treeV1Insert['received']['dataTypes'], 'Tree v1 data type field');

    $treeV2 = createClient($port, ['apiVersion' => 2]);
    $treeV2Query = $treeV2->query('SELECT s1 FROM root.sg.d1', 19);
    assertSameValue('/rest/v2/query', $treeV2Query['receivedPath'], 'Tree v2 query route');
    assertSameValue(19, $treeV2Query['received']['row_limit'], 'Tree v2 row limit field');
    $nonQuery = $treeV2->nonQuery('INSERT INTO root.sg.d1(time,s1) VALUES(1,42)');
    assertSameValue('/rest/v2/nonQuery', $nonQuery['receivedPath'], 'Tree v2 non-query route');
    assertTrueValue(!array_key_exists('row_limit', $nonQuery['received']), 'non-query should not add a read limit');
    $treeV2Insert = $treeV2->insertTablet([
        'device' => 'root.sg.d1',
        'timestamps' => [1],
        'measurements' => ['s1'],
        'dataTypes' => ['INT32'],
        'values' => [[42]],
    ]);
    assertSameValue('/rest/v2/insertTablet', $treeV2Insert['receivedPath'], 'Tree v2 insert route');
    assertSameValue('root.sg.d1', $treeV2Insert['received']['device'], 'Tree v2 device field');
    assertSameValue(['INT32'], $treeV2Insert['received']['data_types'], 'Tree v2 data type field');

    $table = createClient($port, ['model' => 'table', 'database' => 'telemetry']);
    $tableQuery = $table->query('SELECT time, s1 FROM sensors');
    assertSameValue('/rest/table/v1/query', $tableQuery['receivedPath'], 'Table query route');
    assertSameValue('telemetry', $tableQuery['received']['database'], 'Table database selection');
    assertSameValue(100, $tableQuery['received']['row_limit'], 'Table row limit field');
    $tableNonQuery = $table->nonQuery('INSERT INTO sensors(time,s1) VALUES(1,42)');
    assertSameValue('/rest/table/v1/nonQuery', $tableNonQuery['receivedPath'], 'Table non-query route');
    assertSameValue('telemetry', $tableNonQuery['received']['database'], 'Table non-query database');
    $tableInsert = $table->insertTablet([
        'table' => 'sensors',
        'timestamps' => [1],
        'columnNames' => ['site', 's1'],
        'columnCategories' => ['TAG', 'FIELD'],
        'dataTypes' => ['STRING', 'INT32'],
        'values' => [['west', 42]],
    ]);
    assertSameValue('/rest/table/v1/insertTablet', $tableInsert['receivedPath'], 'Table insert route');
    assertSameValue(['TAG', 'FIELD'], $tableInsert['received']['column_categories'], 'Table categories mapping');
    assertSameValue('telemetry', $tableInsert['received']['database'], 'Table insert database');

    try {
        $treeV1->query('error-fixture');
        throw new RuntimeException('IoTDB statement error was not raised');
    } catch (RestException $exception) {
        assertSameValue(200, $exception->getHttpStatus(), 'IoTDB statement error HTTP status');
        assertSameValue(301, $exception->getServerCode(), 'IoTDB statement error code');
        assertSameValue('fixture statement error', $exception->getMessage(), 'IoTDB statement error message');
    }

    try {
        $treeV1->query('malformed-fixture');
        throw new RuntimeException('Malformed JSON was not rejected');
    } catch (RestException $exception) {
        assertTrueValue(strpos($exception->getMessage(), 'invalid JSON') !== false, 'malformed JSON error detail');
    }

    try {
        createClient($port, ['timeout' => 0.1])->query('slow-fixture');
        throw new RuntimeException('HTTP request timeout was not raised');
    } catch (RestException $exception) {
        assertTrueValue(strpos($exception->getMessage(), 'IoTDB REST request failed') !== false, 'HTTP timeout error detail');
    }

    try {
        createClient($port, ['model' => 'table', 'apiVersion' => 2]);
        throw new RuntimeException('Unsupported Table REST version was accepted');
    } catch (InvalidArgumentException $exception) {
        assertTrueValue(strpos($exception->getMessage(), 'Table model') !== false, 'unsupported Table REST version detail');
    }

    try {
        $treeV1->insertTablet([
            'device' => 'root.sg.d1',
            'timestamps' => [1, 2],
            'measurements' => ['s1'],
            'dataTypes' => ['INT32'],
            'values' => [[42]],
        ]);
        throw new RuntimeException('Mismatched tablet lengths were accepted');
    } catch (InvalidArgumentException $exception) {
        assertTrueValue(strpos($exception->getMessage(), 'same row count') !== false, 'tablet shape validation detail');
    }

    echo "IoTDB REST client tests passed.\n";
} finally {
    proc_terminate($process);
    fclose($pipes[0]);
    fclose($pipes[1]);
    fclose($pipes[2]);
    proc_close($process);
}
