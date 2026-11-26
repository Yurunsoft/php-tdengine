<?php

namespace IoTDB;

class RestException extends \RuntimeException
{
    private $httpStatus;
    private $serverCode;
    private $response;

    public function __construct($message, $httpStatus = null, $serverCode = null, array $response = [])
    {
        parent::__construct($message, is_numeric($serverCode) ? (int) $serverCode : 0);
        $this->httpStatus = $httpStatus;
        $this->serverCode = $serverCode;
        $this->response = $response;
    }

    public function getHttpStatus()
    {
        return $this->httpStatus;
    }

    public function getServerCode()
    {
        return $this->serverCode;
    }

    public function getResponse()
    {
        return $this->response;
    }
}

/**
 * Minimal synchronous IoTDB REST client for Tree and Table SQL endpoints.
 *
 * This is intentionally separate from the native extension: IoTDB
 * uses HTTP REST and cannot be reached through libtaos.
 */
class RestConnection
{
    private $endpoint;
    private $model;
    private $apiVersion;
    private $username;
    private $password;
    private $timeout;
    private $rowLimit;
    private $database;

    public function __construct(array $config)
    {
        $endpoint = isset($config['endpoint']) ? $config['endpoint'] : null;
        if (!is_string($endpoint) || trim($endpoint) === '') {
            throw new \InvalidArgumentException('endpoint must be an absolute http or https URL');
        }

        $parts = parse_url($endpoint);
        if (
            !is_array($parts)
            || !isset($parts['scheme'], $parts['host'])
            || !in_array(strtolower($parts['scheme']), ['http', 'https'], true)
            || isset($parts['user'])
            || isset($parts['pass'])
            || isset($parts['query'])
            || isset($parts['fragment'])
        ) {
            throw new \InvalidArgumentException('endpoint must be an http or https URL without embedded credentials, query, or fragment');
        }

        $this->endpoint = rtrim($endpoint, '/');
        $this->model = isset($config['model']) ? strtolower($config['model']) : 'tree';
        if (!in_array($this->model, ['tree', 'table'], true)) {
            throw new \InvalidArgumentException("model must be 'tree' or 'table'");
        }

        $this->apiVersion = isset($config['apiVersion']) ? $config['apiVersion'] : 1;
        if (!is_int($this->apiVersion) || !in_array($this->apiVersion, [1, 2], true)) {
            throw new \InvalidArgumentException('apiVersion must be integer 1 or 2');
        }
        if ($this->model === 'table' && $this->apiVersion !== 1) {
            throw new \InvalidArgumentException('Table model is supported through REST table v1 only');
        }

        $this->username = isset($config['username']) ? $config['username'] : null;
        $this->password = isset($config['password']) ? $config['password'] : null;
        if (!is_string($this->username) || $this->username === '' || !is_string($this->password)) {
            throw new \InvalidArgumentException('username and password strings are required');
        }

        $this->timeout = isset($config['timeout']) ? $config['timeout'] : 10;
        if (!is_numeric($this->timeout) || !is_finite((float) $this->timeout) || $this->timeout <= 0 || $this->timeout > 300) {
            throw new \InvalidArgumentException('timeout must be greater than 0 and no more than 300 seconds');
        }
        $this->timeout = (float) $this->timeout;

        $this->rowLimit = isset($config['rowLimit']) ? $config['rowLimit'] : 1000;
        $this->validateRowLimit($this->rowLimit);

        $this->database = isset($config['database']) ? $config['database'] : null;
        if ($this->database !== null && !is_string($this->database)) {
            throw new \InvalidArgumentException('database must be a string when provided');
        }
    }

    public function useDatabase($database)
    {
        if (!is_string($database)) {
            throw new \InvalidArgumentException('database must be a string');
        }
        $this->database = $database;
    }

    public function getDatabase()
    {
        return $this->database;
    }

    /** Return the decoded IoTDB QueryDataSet payload. */
    public function query($sql, $rowLimit = null)
    {
        $payload = ['sql' => $this->validateSql($sql)];
        $limit = $rowLimit === null ? $this->rowLimit : $rowLimit;
        $this->validateRowLimit($limit);

        if ($this->model === 'table') {
            if ($this->database !== null) {
                $payload['database'] = $this->database;
            }
            $payload['row_limit'] = $limit;
        } elseif ($this->apiVersion === 1) {
            $payload['rowLimit'] = $limit;
        } else {
            $payload['row_limit'] = $limit;
        }

        return $this->post($this->route('query'), $payload);
    }

    /** Execute a non-query statement, including SQL INSERT/UPDATE/DDL. */
    public function nonQuery($sql)
    {
        $payload = ['sql' => $this->validateSql($sql)];
        if ($this->model === 'table' && $this->database !== null) {
            $payload['database'] = $this->database;
        }
        return $this->post($this->route('nonQuery'), $payload);
    }

    /**
     * Insert a columnar batch using the documented REST insertTablet endpoint.
     *
     * Tree batches use device, timestamps, measurements, dataTypes and values.
     * Table batches use table, timestamps, columnNames, columnCategories,
     * dataTypes and values.
     */
    public function insertTablet(array $batch)
    {
        $timestamps = isset($batch['timestamps']) ? $batch['timestamps'] : null;
        $values = isset($batch['values']) ? $batch['values'] : null;
        if (!is_array($timestamps) || !$this->isList($timestamps) || !is_array($values) || !$this->isList($values)) {
            throw new \InvalidArgumentException('timestamps and values must be sequential arrays');
        }

        if ($this->model === 'table') {
            $table = isset($batch['table']) ? $batch['table'] : null;
            $names = isset($batch['columnNames']) ? $batch['columnNames'] : null;
            $categories = isset($batch['columnCategories']) ? $batch['columnCategories'] : null;
            $types = isset($batch['dataTypes']) ? $batch['dataTypes'] : null;
            if (!is_string($table) || $table === '') {
                throw new \InvalidArgumentException('table batch requires a non-empty table name');
            }
            $this->validateColumnBatch($timestamps, $values, $names, $types);
            if (!is_array($categories) || !$this->isList($categories) || count($categories) !== count($names)) {
                throw new \InvalidArgumentException('columnCategories must match columnNames');
            }
            foreach ($categories as $category) {
                if (!is_string($category) || !in_array(strtoupper($category), ['TAG', 'ATTRIBUTE', 'FIELD'], true)) {
                    throw new \InvalidArgumentException('columnCategories values must be TAG, ATTRIBUTE, or FIELD');
                }
            }

            $payload = [
                'timestamps' => $timestamps,
                'column_names' => $names,
                'column_categories' => array_map('strtoupper', $categories),
                'data_types' => $types,
                'values' => $values,
                'table' => $table,
            ];
            if ($this->database !== null) {
                $payload['database'] = $this->database;
            }
            return $this->post('/rest/table/v1/insertTablet', $payload);
        }

        $device = isset($batch['device']) ? $batch['device'] : null;
        $measurements = isset($batch['measurements']) ? $batch['measurements'] : null;
        $types = isset($batch['dataTypes']) ? $batch['dataTypes'] : null;
        if (!is_string($device) || $device === '') {
            throw new \InvalidArgumentException('Tree batch requires a non-empty device path');
        }
        $this->validateColumnBatch($timestamps, $values, $measurements, $types);
        // The Tree REST API takes one vector per measurement. The public
        // batch format supplies one row per timestamp, as in the Table API.
        $vectors = [];
        foreach (array_keys($measurements) as $index) {
            $vectors[] = array_column($values, $index);
        }
        $aligned = isset($batch['isAligned']) ? $batch['isAligned'] : false;
        if (!is_bool($aligned)) {
            throw new \InvalidArgumentException('isAligned must be boolean');
        }

        if ($this->apiVersion === 1) {
            $payload = [
                'timestamps' => $timestamps,
                'measurements' => $measurements,
                'dataTypes' => $types,
                'values' => $vectors,
                'isAligned' => $aligned,
                'deviceId' => $device,
            ];
        } else {
            $payload = [
                'timestamps' => $timestamps,
                'measurements' => $measurements,
                'data_types' => $types,
                'values' => $vectors,
                'is_aligned' => $aligned,
                'device' => $device,
            ];
        }

        return $this->post($this->route('insertTablet'), $payload);
    }

    private function route($operation)
    {
        if ($this->model === 'table') {
            return '/rest/table/v1/' . $operation;
        }
        return '/rest/v' . $this->apiVersion . '/' . $operation;
    }

    private function post($path, array $payload)
    {
        try {
            $json = json_encode($payload, JSON_UNESCAPED_UNICODE | JSON_UNESCAPED_SLASHES | JSON_THROW_ON_ERROR);
        } catch (\JsonException $exception) {
            throw new RestException('Could not encode IoTDB REST request JSON: ' . $exception->getMessage());
        }

        $context = stream_context_create([
            'http' => [
                'method' => 'POST',
                'header' => implode("\r\n", [
                    'Accept: application/json',
                    'Content-Type: application/json; charset=utf-8',
                    'Authorization: Basic ' . base64_encode($this->username . ':' . $this->password),
                    'Content-Length: ' . strlen($json),
                    'Connection: close',
                ]),
                'content' => $json,
                'timeout' => $this->timeout,
                'ignore_errors' => true,
                'follow_location' => 0,
                'max_redirects' => 0,
            ],
            'ssl' => [
                'verify_peer' => true,
                'verify_peer_name' => true,
            ],
        ]);

        $url = $this->endpoint . $path;
        $body = @file_get_contents($url, false, $context);
        $responseHeaders = isset($http_response_header) ? $http_response_header : [];
        $httpStatus = $this->readHttpStatus($responseHeaders);
        if ($body === false) {
            $lastError = error_get_last();
            $message = $lastError && isset($lastError['message']) ? $lastError['message'] : 'network request failed';
            throw new RestException('IoTDB REST request failed: ' . $message, $httpStatus);
        }

        try {
            $decoded = json_decode($body, true, 512, JSON_THROW_ON_ERROR);
        } catch (\JsonException $exception) {
            throw new RestException('IoTDB REST returned invalid JSON: ' . $exception->getMessage(), $httpStatus);
        }
        if (!is_array($decoded)) {
            throw new RestException('IoTDB REST returned a non-object JSON response', $httpStatus);
        }

        $serverCode = isset($decoded['code']) ? $decoded['code'] : null;
        $serverMessage = isset($decoded['message']) && is_string($decoded['message']) ? $decoded['message'] : null;
        if ($httpStatus === null || $httpStatus < 200 || $httpStatus >= 300) {
            throw new RestException($serverMessage ?: 'IoTDB REST returned an HTTP error', $httpStatus, $serverCode, $decoded);
        }
        $dataset = $serverCode === null && substr($path, -6) === '/query'
            && isset($decoded['values']) && is_array($decoded['values'])
            && (isset($decoded['expressions']) || isset($decoded['columnNames']) || isset($decoded['column_names']));
        if (!$dataset && (!is_numeric($serverCode) || (int) $serverCode !== 200)) {
            throw new RestException($serverMessage ?: 'IoTDB REST statement failed', $httpStatus, $serverCode, $decoded);
        }

        return $decoded;
    }

    private function readHttpStatus(array $headers)
    {
        for ($index = count($headers) - 1; $index >= 0; --$index) {
            if (preg_match('/^HTTP\/\S+\s+(\d{3})\b/i', $headers[$index], $matches)) {
                return (int) $matches[1];
            }
        }
        return null;
    }

    private function validateSql($sql)
    {
        if (!is_string($sql) || trim($sql) === '') {
            throw new \InvalidArgumentException('sql must be a non-empty string');
        }
        return $sql;
    }

    private function validateRowLimit($rowLimit)
    {
        if (!is_int($rowLimit) || $rowLimit < 1 || $rowLimit > 1000000) {
            throw new \InvalidArgumentException('row limit must be an integer between 1 and 1000000');
        }
    }

    private function validateColumnBatch($timestamps, $values, $names, $types)
    {
        if (!is_array($names) || !$this->isList($names) || count($names) === 0) {
            throw new \InvalidArgumentException('column names must be a non-empty sequential array');
        }
        if (!is_array($types) || !$this->isList($types) || count($types) !== count($names)) {
            throw new \InvalidArgumentException('dataTypes must match the column names');
        }
        if (count($timestamps) !== count($values)) {
            throw new \InvalidArgumentException('values and timestamps must have the same row count');
        }
        foreach ($names as $name) {
            if (!is_string($name) || $name === '') {
                throw new \InvalidArgumentException('column names must be non-empty strings');
            }
        }
        foreach ($types as $type) {
            if (!is_string($type) || $type === '') {
                throw new \InvalidArgumentException('dataTypes must contain non-empty strings');
            }
        }
        foreach ($timestamps as $timestamp) {
            if (!is_int($timestamp)) {
                throw new \InvalidArgumentException('timestamps must contain integer values');
            }
        }
        foreach ($values as $row) {
            if (!is_array($row) || !$this->isList($row) || count($row) !== count($names)) {
                throw new \InvalidArgumentException('every value row must match the column count');
            }
            foreach ($row as $value) {
                if (!is_scalar($value) && $value !== null) {
                    throw new \InvalidArgumentException('tablet values must be JSON scalar values or null');
                }
            }
        }
    }

    private function isList(array $value)
    {
        return $value === [] || array_keys($value) === range(0, count($value) - 1);
    }
}
