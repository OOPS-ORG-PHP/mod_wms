<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=EUC-KR">
<title>mod_mms check test page</title>
<style type="text/css">
a:link, a:visited, a:active { text-decoration: none; color:#555555; }
a:hover { text-decoration:underline; color:ORANGE; }
body, td { font-size:12px; font-family:굴림체; color:#555555; }
input { font-size:12px; font-family:굴림체; color:#555555; }
</style>
</head>
<body>
<pre>

mod_mms version <?=mmsversion()?>


<?
set_time_limit(0);

define (NOMAL, 0);
define (HOST_NOT_FOUND, 1);
define (SOCKET_CREATE_FAIL, 2);
define (CONNECT_FAIL, 3);
define (MALLOC_ERROR, 4);
define (ICONV_ERROR, 5);
define (NONBLOCK_ERROR, 6);
define (BIND_ERROR, 7);
define (INVALID_URL, 100);
define (CLOSE_PORT, 101);
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

echo "MMSCHECKS FUNCTION TEST\n\n";
for ($j=0; $j<count($addr); $j++) {
  for ($i=0; $i<count($uri);$i++) {
    # browser 의 buffer 출력을 막기 위하여 system 함수를 넣어줌
    system ("echo");

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

    sleep(1);
  }
}
echo "\n";

echo "\n\n";
echo "** MMSCHECK FUNCTION TEST\n\n";
for ($j=0; $j<count($addr); $j++) {
  for ($i=0; $i<count($uri);$i++) {
    # browser 의 buffer 출력을 막기 위하여 system 함수를 넣어줌
    system ("echo");

    if ( ! $uri[$i] ) $uri[$i] = "/";
    if ( ! preg_match ("!^/!", $uri[$i]) ) $uri[$i] = "/" . $uri[$i];

    $url = "mms://{$addr[$j]}{$uri[$i]}{$opt}";

    $ret = mmscheck ($url, $timeout, $debug) ."<br>\n";

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

    sleep(1);
  }
}
echo "\n";

?>
</pre>
</body>
</html>
