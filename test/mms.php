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

$addr[] = "192.168.0.1";
$addr[] = "192.168.0.2";
$addr[] = "192.168.0.3";

$debug = $_GET['debug'] ? $_GET['debug'] : 0;
$timeout = $_GET['timeout'] ? $_GET['timeout'] : 2;
$sleep = $_GET['sleep'] ? $_GET['sleep'] : 0;

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
    echo mmscheck ($ret, $addr[$j]);

    if ( $sleep ) sleep($sleep);
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

    echo "$url ... " . mmscheck ($ret, $addr[$j]);

    if ( $sleep ) sleep($sleep);
  }
}
echo "\n";

?>
</pre>
</body>
</html>
