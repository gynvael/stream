<?php
  header('X-XSS-Protection: 0');
  header("Content-Security-Policy: default-src 'none'; style-src 'self'; img-src data: http:");

?>
<html>
<body>
  <h2>My website</h2>

  <p>Message: <?php
  echo $_GET['msg'];
  ?></p>

  <form>
    <input id="name" />
    <input id="xsrf" value="9087621378abcdf" />
    <input type="submit" />
  </form>


</body>
</html>




