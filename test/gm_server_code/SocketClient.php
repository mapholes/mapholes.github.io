<?php
error_reporting(E_ALL);

$in = $_GET["geojson"];

$service_port =9010;
$address = "134.74.112.65";
$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
if ($socket === false)
{
    echo "socket_create() failed: reason: " . socket_strerror(socket_last_error()) . "\n";
}

$result = socket_connect($socket, $address, $service_port);
if ($result === false)
{
    echo "socket_connect() failed.\nReason: ($result) " . socket_strerror(socket_last_error($socket)) . "\n";
}

//echo $in;

socket_write($socket, $in, strlen($in));

while ($out = socket_read($socket, 2048)) {
    echo $out;
}
socket_close($socket);
?>
