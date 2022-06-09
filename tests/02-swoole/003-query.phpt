--TEST--
test query
--SKIPIF--
<?php require dirname(__DIR__) . '/include/skip/skip-no-swoole.php'; ?>
--EXTENSIONS--
tdengine
--FILE--
<?php
declare(strict_types=1);

use Test\Assert;

include dirname(__DIR__) . '/include/include.php';
define('DB_NAME', 'php_tdengine_test');

Co\run(function(){
    $connection = Test\getConnection();
    $connection->connect();

    $sql = 'create database if not exists php_tdengine_test';
    $resource = $connection->query($sql);
    Assert::eq($resource->getSql(), $sql);

    $connection->selectDb('php_tdengine_test');

    $sql = 'create table if not exists test_query (ts timestamp, temperature int, humidity float)';
    $resource = $connection->query($sql);
    Assert::eq($resource->getSql(), $sql);

    $time = (int) (microtime(true) * 1000);
    $sql = sprintf('insert into test_query values(%s,%s,%s)', $time, 36, 44.0);
    $resource = $connection->query($sql);
    Assert::eq($resource->affectedRows(), 1);
    Assert::eq($resource->getSql(), $sql);

    $sql = 'select * from test_query order by ts desc limit 1';
    $resource = $connection->query($sql);
    Assert::eq($resource->getSql(), $sql);
    Assert::eq($resource->fetch(), [
        [
            'ts'          => $time,
            'temperature' => 36,
            'humidity'    => 44.0,
        ],
    ]);

    $resource = $connection->query($sql);
    Assert::eq($resource->getSql(), $sql);
    Assert::eq($resource->fetchRow(), [
        'ts'          => $time,
        'temperature' => 36,
        'humidity'    => 44.0,
    ]);

    // bug: https://github.com/Yurunsoft/php-tdengine/issues/6
    $sql = <<<'SQL'
    SELECT 
      AVG(temperature) as avg_temperature
    FROM 
      test_query 
    WHERE 
      ts >= NOW - 1d 
      and ts <= now INTERVAL(10s) FILL(PREV) 
    limit 
      1;
    SQL;
    $resource = $connection->query($sql);
    Assert::eq($resource->getSql(), $sql);
    $row = $resource->fetchRow();
    Assert::true(array_key_exists('avg_temperature', $row));
    Assert::null($row['avg_temperature']);
});
?>
--EXPECT--
