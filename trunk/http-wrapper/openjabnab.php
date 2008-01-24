<?php
	$socket = fsockopen("127.0.0.1", 8080);
	if(!$socket)
		echo "Problem with OpenJabNab !";
	else
	{
	    fwrite($socket, $_SERVER['REQUEST_URI']);
	    while (!feof($socket)) {
	        echo fgets($socket, 128);
	    }
		fclose($socket);
	}
?>
