--TEST--
test setOptions
--EXTENSIONS--
tdengine
--FILE--
<?php
declare(strict_types=1);

// 设置客户端选项
\TDengine\setOptions([
    \TDengine\TSDB_OPTION_LOCALE => 'en_US.UTF-8', // 区域
    \TDengine\TSDB_OPTION_CHARSET => 'UTF-8', // 字符集
    \TDengine\TSDB_OPTION_TIMEZONE => 'Asia/Shanghai', // 时区
    \TDengine\TSDB_OPTION_CONFIGDIR => '/etc/taos', // 配置目录
    \TDengine\TSDB_OPTION_SHELL_ACTIVITY_TIMER => 3, // shell 活动定时器
]);
?>
--EXPECT--
