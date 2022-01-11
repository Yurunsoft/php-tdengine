--TEST--
test connect and close
--SKIPIF--
<?php require dirname(__DIR__) . '/include/skip/skip-no-swoole.php'; ?>
--EXTENSIONS--
tdengine
--FILE--
<?php
declare(strict_types=1);

use function Test\getDb;
use function Test\getHost;
use function Test\getPass;
use function Test\getPort;
use function Test\getUser;

include dirname(__DIR__) . '/include/include.php';

Co\run(function(){
    $connection = Test\getConnection();
    assert(getHost() === $connection->getHost());
    assert(getPort() === $connection->getPort());
    assert(getUser() === $connection->getUser());
    assert(getPass() === $connection->getPass());
    assert(getDb() === $connection->getDb());
    $connection->connect();
    assert(true === $connection->isConnected());
    $connection->close();
});
?>
--EXPECT--
