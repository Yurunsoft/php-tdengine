--TEST--
test prepare batch
--EXTENSIONS--
tdengine
--FILE--
<?php
declare(strict_types=1);

use Test\Assert;

include dirname(__DIR__) . '/include/include.php';
define('DB_NAME', 'php_tdengine_test');
$connection = Test\getConnection();
$connection->connect();

$sql = 'create database if not exists php_tdengine_test';
$resource = $connection->query($sql);
Assert::eq($resource->getSql(), $sql);

$connection->selectDb('php_tdengine_test');

$sql = 'CREATE STABLE if not exists meters (ts TIMESTAMP, voltage INT) TAGS (location BINARY(64), groupId INT)';
$resource = $connection->query($sql);
Assert::eq($resource->getSql(), $sql);

$time1 = (int) (microtime(true) * 1000);
$sql = 'INSERT INTO ? USING meters TAGS(?, ?) VALUES(?, ?)';
$stmt = $connection->prepare($sql);

$stmt->setTableNameTags('d1001', [
    [TDengine\TSDB_DATA_TYPE_BINARY, 'Beijing.Chaoyang'],
    [TDengine\TSDB_DATA_TYPE_INT, 1],
]);

$stmt->bindParams([
    [TDengine\TSDB_DATA_TYPE_TIMESTAMP, $time1],
    [TDengine\TSDB_DATA_TYPE_INT, 219],
]);

$time2 = (int) (microtime(true) * 1000);
if ($time1 === $time2)
{
    $time2 = $time1 + 1;
}

$stmt->bindParams([
    [TDengine\TSDB_DATA_TYPE_TIMESTAMP, $time2],
    [TDengine\TSDB_DATA_TYPE_INT, 218],
]);

$resource = $stmt->execute();
Assert::eq($resource->affectedRows(), 2);
Assert::eq($resource->getSql(), $sql);

$sql = 'select * from meters order by ts desc limit 2';
$resource = $connection->query($sql);
Assert::eq($resource->getSql(), $sql);
Assert::eq($resource->fetch(), [
    [
        'ts'       => $time2,
        'voltage'  => 218,
        'location' => 'Beijing.Chaoyang',
        'groupid'  => 1,
    ],
    [
        'ts'       => $time1,
        'voltage'  => 219,
        'location' => 'Beijing.Chaoyang',
        'groupid'  => 1,
    ],
]);

$resource = $connection->query($sql);
Assert::eq($resource->getSql(), $sql);
Assert::eq($resource->fetchRow(), [
    'ts'      => $time2,
    'voltage' => 218,
    'location' => 'Beijing.Chaoyang',
    'groupid'  => 1,
]);
?>
--EXPECT--
