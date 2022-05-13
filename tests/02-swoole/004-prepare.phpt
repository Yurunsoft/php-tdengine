--TEST--
test prepare
--SKIPIF--
<?php require dirname(__DIR__) . '/include/skip/skip-no-swoole.php'; ?>
--EXTENSIONS--
tdengine
--FILE--
<?php
declare(strict_types=1);

use Test\Assert;

include dirname(__DIR__) . '/include/include.php';

Co\run(function(){
    define('DB_NAME', 'php_tdengine_test');
    $connection = Test\getConnection();
    $connection->connect();

    $sql = 'create database if not exists php_tdengine_test';
    $resource = $connection->query($sql);
    Assert::eq($resource->getSql(), $sql);

    $connection->selectDb('php_tdengine_test');

    $sql = 'create table if not exists test_query (ts timestamp, temperature int, humidity float)';
    $resource = $connection->query($sql);
    Assert::eq($resource->getSql(), $sql);

    $time1 = (int) (microtime(true) * 1000);
    $sql = 'insert into test_query values(?,?,?)';
    $stmt = $connection->prepare($sql);
    $stmt->bindParams([
        [TDengine\TSDB_DATA_TYPE_TIMESTAMP, $time1],
        [TDengine\TSDB_DATA_TYPE_INT, 36],
        [TDengine\TSDB_DATA_TYPE_FLOAT, 44.0],
    ]);
    $resource = $stmt->execute();
    Assert::eq($resource->affectedRows(), 1);
    Assert::eq($resource->getSql(), $sql);

    $time2 = (int) (microtime(true) * 1000);
    if ($time1 === $time2)
    {
        $time2 = $time1 + 1;
    }
    $sql = 'insert into test_query values(?,?,?)';
    $stmt = $connection->prepare($sql);
    $stmt->bindParams([
        ['type' => TDengine\TSDB_DATA_TYPE_TIMESTAMP, 'value' => $time2],
        ['type' => TDengine\TSDB_DATA_TYPE_INT, 'value' => 36],
        ['type' => TDengine\TSDB_DATA_TYPE_FLOAT, 'value' => 44.0],
    ]);
    $resource = $stmt->execute();
    Assert::eq($resource->affectedRows(), 1);
    Assert::eq($resource->getSql(), $sql);

    $sql = 'select * from test_query order by ts desc limit 2';
    $resource = $connection->query($sql);
    Assert::eq($resource->getSql(), $sql);
    Assert::eq($resource->fetch(), [
        [
            'ts'          => $time2,
            'temperature' => 36,
            'humidity'    => 44.0,
        ],
        [
            'ts'          => $time1,
            'temperature' => 36,
            'humidity'    => 44.0,
        ],
    ]);

    $resource = $connection->query($sql);
    Assert::eq($resource->getSql(), $sql);
    Assert::eq($resource->fetchRow(), [
        'ts'          => $time2,
        'temperature' => 36,
        'humidity'    => 44.0,
    ]);
});
?>
--EXPECT--
