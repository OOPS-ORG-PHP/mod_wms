#!/usr/bin/php

mod_mms version <?=mmsversion()?>


<?
set_time_limit(0);

define (NOMAL, 0);
define (HOST_NOT_FOUND, 1);
define (SOCKET_CREATE_FAIL, 2);
define (CONNECT_FAIL, 3);
define (MALLOC_ERROR, 4);
define (FILE_NOT_FOUND, 401);
define (CORRUCPTED_MEDIA, 402);
define (FILE_WRONG, 403);
define (NOT_MEDIA, 404);
define (OTHER_ERROR, 405);

$addr[] = "192.168.0.1";
$addr[] = "192.168.0.2";
$addr[] = "192.168.0.3";
$debug = 0;
$timeout = 2;

$uri[] = "WMLoad.asf";
$uri[] = "WMLoad1.asf";
$uri[] = "WMLoad2.asf";
$uri[] = "WMLoad3.asf";
$uri[] = "WMLoad4.asf";

echo "MMSCHECKS FUNCTION TEST\n\n";
for ($j=0; $j<count($addr); $j++) {
  for ($i=0; $i<count($uri);$i++) {
    if ( ! $uri[$i] ) $uri[$i] = "/";
    if ( ! preg_match ("!^/!", $uri[$i]) ) $uri[$i] = "/" . $uri[$i];

    $ret = mmschecks ("{$addr[$j]}", $uri[$i], $opt, $timeout, $debug);

    echo "mms://{$addr[$j]}{$uri[$i]}$opt ... ";

    switch ($ret) {
      case NOMAL :
        echo "OK";
        break;
      case HOST_NOT_FOUND :
        echo "{$addr[$j]} not found";
        break;
      case SOCKET_CREATE_FAIL :
        echo "socket create failed";
        break;
      case CONNECT_FAIL :
        echo "connect failed";
        break;
      case MALLOC_ERROR :
        echo "memory allocation error";
        break;
      case ICONV_ERROR:
        echo "failed iconv convert";
        break;
      case NONBLOCK_ERROR:
        echo "failed transport non blocking mode";
        break;
      case BIND_ERROR:
        echo "failed to bind local port";
        break;
      case INVALID_URL:
        echo "{$addr[$j]} is invalid address";
        break;
      case CLOSE_PORT:
        echo "Maybe closed 1755 port on {$addr[$j]}";
        break;
      case FILE_NOT_FOUND :
        echo "file not found";
        break;
      case CORRUCPTED_MEDIA :
        echo "media corrucpted";
        break;
      case FILE_WRONG :
        echo "file wrong";
        break;
      case NOT_MEDIA :
        echo "is not media file";
        break;
      default :
        echo "Other error";
        break;
    }

    echo "\n";
    sleep(1);
  }
}
echo "\n";

echo "\n\n";
echo "** MMSCHECK FUNCTION TEST\n\n";
for ($j=0; $j<count($addr); $j++) {
  for ($i=0; $i<count($uri);$i++) {
    if ( ! $uri[$i] ) $uri[$i] = "/";
    if ( ! preg_match ("!^/!", $uri[$i]) ) $uri[$i] = "/" . $uri[$i];

    $url = "mms://{$addr[$j]}{$uri[$i]}{$opt}";

    $ret = mmschecks ($url, $timeout, $debug);

    echo "$url ... ";

    switch ($ret) {
      case NOMAL :
        echo "OK";
        break;
      case HOST_NOT_FOUND :
        echo "{$url} not found";
        break;
      case SOCKET_CREATE_FAIL :
        echo "socket create failed";
        break;
      case CONNECT_FAIL :
        echo "connect failed";
        break;
      case MALLOC_ERROR :
        echo "memory allocation error";
        break;
      case ICONV_ERROR:
        echo "failed iconv convert";
        break;
      case NONBLOCK_ERROR:
        echo "failed transport non blocking mode";
        break;
      case BIND_ERROR:
        echo "failed to bind local port";
        break;
      case INVALID_URL:
        echo "{$url} is invalid address";
        break;
      case CLOSE_PORT:
        echo "Maybe closed 1755 port on {$addr[$j]}";
        break;
      case FILE_NOT_FOUND :
        echo "file not found";
        break;
      case CORRUCPTED_MEDIA :
        echo "media corrucpted";
        break;
      case FILE_WRONG :
        echo "file wrong";
        break;
      case NOT_MEDIA :
        echo "is not media file";
        break;
      default :
        echo "Other error";
        break;
    }

    echo "\n";
    sleep(1);
  }
}
echo "\n";

?>
