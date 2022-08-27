# php-tdengine

![GitHub Workflow Status (branch)](https://img.shields.io/github/workflow/status/yurunsoft/php-tdengine/ci/master)
[![Php Version](https://img.shields.io/badge/php-%3E=7.4-brightgreen.svg)](https://secure.php.net/)
[![imi License](https://img.shields.io/badge/license-AGPLv3-brightgreen.svg)](https://github.com/yurunsoft/php-tdengine/blob/master/LICENSE)

大数据引擎 [TDengine](https://github.com/taosdata/TDengine) 的 PHP 客户端扩展，还支持了 Swoole 协程化。可以运行在传统 PHP 环境和 Swoole 环境下。

编写了在三大操作系统下的编译及测试用例，保证代码的稳定性和可靠性。

## 环境要求

Windows、Linux、MacOS

PHP >= 7.4

TDengine >= 2.0（已支持 TDengine 3.0）

Swoole >= 4.8 (可选)

## 编译安装

编译前请先安装 [TDengine-client](https://www.taosdata.com/cn/getting-started/#%E9%80%9A%E8%BF%87%E5%AE%89%E8%A3%85%E5%8C%85%E5%AE%89%E8%A3%85) 或者自行编译 TDengine，详见 TDengine 文档：<https://www.taosdata.com/cn/documentation/connector>

**非 Swoole 环境：**

```shell
phpize && ./configure && make -j && make install
```

**手动指定 tdengine 目录：**

```shell
phpize && ./configure --with-tdengine-dir=/usr/local/Cellar/tdengine/2.4.0.0 && make -j && make install
```

> `--with-tdengine-dir=` 后跟上 tdengine 目录。
> 适用于默认找不到的情况，或者 MacOS 系统用户。

**Swoole 环境：**

```shell
phpize && ./configure --enable-swoole && make -j && make install
```

**启用扩展：**

方法一：在 `php.ini` 中加入 `extension=tdengine`

方法二：运行带参数 `php -dextension=tdengine test.php`

## PHP 代码编写

> 所有错误都会抛出异常: `TDengine\Exception\TDengineException`

**基本：**

```php
use TDengine\Connection;

// 获取扩展版本号
var_dump(\TDengine\EXTENSION_VERSION);

// 设置客户端选项
\TDengine\setOptions([
    \TDengine\TSDB_OPTION_LOCALE => 'en_US.UTF-8', // 区域
    \TDengine\TSDB_OPTION_CHARSET => 'UTF-8', // 字符集
    \TDengine\TSDB_OPTION_TIMEZONE => 'Asia/Shanghai', // 时区
    \TDengine\TSDB_OPTION_CONFIGDIR => '/etc/taos', // 配置目录
    \TDengine\TSDB_OPTION_SHELL_ACTIVITY_TIMER => 3, // shell 活动定时器
]);

// 获取客户端版本信息
var_dump(\TDengine\CLIENT_VERSION);
var_dump(\TDengine\getClientInfo());

// 以下值都是默认值，不改可以不传
$host = '127.0.0.1';
$port = 6030;
$user = 'root';
$pass = 'taosdata';
$db = null;

// 实例化
$connection = new Connection($host, $port, $user, $pass, $db);
// 连接
$connection->connect();
// 获取连接参数
$connection->getHost();
$connection->getPort();
$connection->getUser();
$connection->getPass();
$connection->getDb();
// 获取服务端信息
$connection->getServerInfo();
// 选择默认数据库
$connection->selectDb('db1');
// 关闭连接
$connection->close();
```

**查询：**

```php
// 查询
$resource = $connection->query($sql); // 支持查询和插入
// 获取结果集时间戳字段的精度，0 代表毫秒，1 代表微秒，2 代表纳秒
$resource->getResultPrecision();
// 获取所有数据
$resource->fetch();
// 获取一行数据
$resource->fetchRow();
// 获取字段数组
$resource->fetchFields();
// 获取列数
$resource->getFieldCount();
// 获取影响行数
$resource->affectedRows();
// 获取 SQL 语句
$resource->getSql();
// 获取连接对象
$resource->getConnection();
// 关闭资源（一般不需要手动关闭，变量销毁时会自动释放）
$resource->close();
```

**参数绑定：**

```php
// 查询
$stmt = $connection->prepare($sql); // 支持查询和插入，参数用?占位
// 设置表名和标签
$stmt->setTableNameTags('表名', [
    // 支持格式同参数绑定
    [TDengine\TSDB_DATA_TYPE_INT, 36],
]);
// 绑定参数方法1
$stmt->bindParams(
    // [字段类型, 值]
    [TDengine\TSDB_DATA_TYPE_TIMESTAMP, $time1],
    [TDengine\TSDB_DATA_TYPE_INT, 36],
    [TDengine\TSDB_DATA_TYPE_FLOAT, 44.0],
);
// 绑定参数方法2
$stmt->bindParams([
    // ['type' => 字段类型, 'value' => 值]
    ['type' => TDengine\TSDB_DATA_TYPE_TIMESTAMP, 'value' => $time2],
    ['type' => TDengine\TSDB_DATA_TYPE_INT, 'value' => 36],
    ['type' => TDengine\TSDB_DATA_TYPE_FLOAT, 'value' => 44.0],
]);
// 执行 SQL，返回 Resource，使用方法同 query() 返回值
$resource = $stmt->execute();
// 获取 SQL 语句
$stmt->getSql();
// 获取连接对象
$stmt->getConnection();
// 关闭（一般不需要手动关闭，变量销毁时会自动释放）
$stmt->close();
```

**字段类型：**

| 参数名称 | 说明 |
| ------------ | ------------ 
| `TDengine\TSDB_DATA_TYPE_NULL` | null |
| `TDengine\TSDB_DATA_TYPE_BOOL` | bool |
| `TDengine\TSDB_DATA_TYPE_TINYINT` | tinyint |
| `TDengine\TSDB_DATA_TYPE_SMALLINT` | smallint |
| `TDengine\TSDB_DATA_TYPE_INT` | int |
| `TDengine\TSDB_DATA_TYPE_BIGINT` | bigint |
| `TDengine\TSDB_DATA_TYPE_FLOAT` | float |
| `TDengine\TSDB_DATA_TYPE_DOUBLE` | double |
| `TDengine\TSDB_DATA_TYPE_BINARY` | binary |
| `TDengine\TSDB_DATA_TYPE_TIMESTAMP` | timestamp |
| `TDengine\TSDB_DATA_TYPE_NCHAR` | nchar |
| `TDengine\TSDB_DATA_TYPE_UTINYINT` | utinyint |
| `TDengine\TSDB_DATA_TYPE_USMALLINT` | usmallint |
| `TDengine\TSDB_DATA_TYPE_UINT` | uint |
| `TDengine\TSDB_DATA_TYPE_UBIGINT` | ubigint |
| `TDengine\TSDB_DATA_TYPE_JSON` | json |
| `TDengine\TSDB_DATA_TYPE_VARBINARY` | varbinary |
| `TDengine\TSDB_DATA_TYPE_DECIMAL` | decimal |
| `TDengine\TSDB_DATA_TYPE_BLOB` | blob |
| `TDengine\TSDB_DATA_TYPE_MEDIUMBLOB` | mediumblob |
| `TDengine\TSDB_DATA_TYPE_BINARY` | binary |
