--TEST--
test1() Basic test
--EXTENSIONS--
tdengine
--FILE--
<?php
$ret = test1();

var_dump($ret);
?>
--EXPECT--
The extension tdengine is loaded and working!
NULL
