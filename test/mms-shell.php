#!/usr/bin/php
<? dl("../modules/wms.so"); ?>
mod_wms version <?=wmsversion()?>


<?
set_time_limit(0);

$addr[] = "192.168.0.1";
$addr[] = "192.168.0.2";
$addr[] = "192.168.0.3";
$addr[] = "192.168.0.4";
$debug = 0;
$timeout = 2;
$sleep = 0;
$opt = "";

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
    echo wmsmsg ($ret, $addr[$j]) . "\n";

    if ( $sleep ) sleep($sleep);
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

    $ret = mmscheck ($url, $timeout, $debug);

    echo "$url ... " . wmsmsg ($ret, $addr[$j]) . "\n";
    if ( $sleep ) sleep($sleep);
  }
}
echo "\n";

echo "RTSPCHECKS FUNCTION TEST\n\n";

for ($j=0; $j<count($addr); $j++) {
  for ($i=0; $i<count($uri);$i++) {
    if ( ! $uri[$i] ) $uri[$i] = "/";
    if ( ! preg_match ("!^/!", $uri[$i]) ) $uri[$i] = "/" . $uri[$i];

    $ret = rtspchecks ("{$addr[$j]}", $uri[$i], $opt, $timeout, $debug);

    echo "rtsp://{$addr[$j]}{$uri[$i]}$opt ... ";
    echo wmsmsg ($ret, $addr[$j]) . " RC=>" . getrtspcode() . "\n";

    if ( $sleep ) sleep($sleep);
  }
}
echo "\n";

echo "\n\n";
echo "** RTSPCHECK FUNCTION TEST\n\n";
for ($j=0; $j<count($addr); $j++) {
  for ($i=0; $i<count($uri);$i++) {
    if ( ! $uri[$i] ) $uri[$i] = "/";
    if ( ! preg_match ("!^/!", $uri[$i]) ) $uri[$i] = "/" . $uri[$i];

    $url = "rtsp://{$addr[$j]}{$uri[$i]}{$opt}";

    $ret = rtspcheck ($url, $timeout, $debug);

    echo "$url ... " . wmsmsg ($ret, $addr[$j]) . " RC=>" . getrtspcode() . "\n";
    if ( $sleep ) sleep($sleep);
  }
}
echo "\n";


?>
