--TEST--
Check if tdengine is loaded
--EXTENSIONS--
swoole
tdengine
--FILE--
<?php
echo 'The extension "tdengine" is available';
?>
--EXPECT--
The extension "tdengine" is available
