<?php

$requestBody = file_get_contents('php://input');
$payload = json_decode($requestBody, true);
$path = isset($_SERVER['REQUEST_URI']) ? $_SERVER['REQUEST_URI'] : '';
$authorization = isset($_SERVER['HTTP_AUTHORIZATION']) ? $_SERVER['HTTP_AUTHORIZATION'] : '';

if ($authorization !== 'Basic ' . base64_encode('root:secret')) {
    http_response_code(401);
    header('Content-Type: application/json');
    echo json_encode(['code' => 801, 'message' => 'bad credentials']);
    return;
}

if (isset($payload['sql']) && strpos($payload['sql'], 'slow-fixture') !== false) {
    usleep(700000);
}

header('Content-Type: application/json; charset=utf-8');
if (isset($payload['sql']) && strpos($payload['sql'], 'malformed-fixture') !== false) {
    echo '{';
    return;
}

if (isset($payload['sql']) && strpos($payload['sql'], 'error-fixture') !== false) {
    echo json_encode(['code' => 301, 'message' => 'fixture statement error']);
    return;
}

$response = [
    'code' => 200,
    'message' => 'success',
    'receivedPath' => $path,
    'received' => $payload,
    'receivedAuthorization' => $authorization,
];
if (strpos($path, '/query') !== false) {
    $response['column_names'] = ['root.sg.d1.s1'];
    $response['values'] = [[42]];
}

echo json_encode($response, JSON_UNESCAPED_UNICODE | JSON_UNESCAPED_SLASHES);
