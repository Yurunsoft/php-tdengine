<?php

namespace TDengine
{
    class Connection
    {
        public function __construct(string $host = '127.0.0.1', int $port = 6030, string $user = 'root', string $pass = 'taosdata', ?string $db = null) { }

        public function connect(): void{ }

        public function close(): void {}

        public function getHost(): string {}

        public function getPort(): int {}

        public function getUser(): string {}

        public function getPass(): string {}

        public function getDb(): ?string {}

        public function getServerInfo(): string {}

        public function selectDb(string $db): void {}

        public function query(string $sql): Resource {}

        public function prepare(string $sql): Statement {}
    }

    class Resource
    {
        public function getConnection(): Connection {}

        public function getStatement(): ?Statement {}

        public function getSql(): string {}

        public function getResultPrecision(): int {}

        public function fetch(): array {}

        public function fetchRow(): array {}

        public function getFieldCount(): int {}

        public function affectedRows(): int {}

        public function fetchFields(): array {}

        public function close(): void {}

    }

    class Statement
    {
        public function getConnection(): Connection {}

        public function getSql(): string {}

        public function close(): void {}

        public function bindParams(array $params): void {}

        public function execute(): Resource;
    }

    function setOptions(array $options): bool
    {

    }

    function getClientInfo(): string
    {

    }
}

namespace TDengine\Exception
{
    class TDengineException extends \Exception
    {
    }
}
