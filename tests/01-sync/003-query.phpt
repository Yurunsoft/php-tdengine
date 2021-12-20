--TEST--
test query
--EXTENSIONS--
tdengine
--FILE--
<?php
declare(strict_types=1);

use Test\Assert;
use function Test\getDb;
use function Test\getHost;
use function Test\getPass;
use function Test\getPort;
use function Test\getUser;

include dirname(__DIR__) . '/include/include.php';
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
?>
--EXPECT--
