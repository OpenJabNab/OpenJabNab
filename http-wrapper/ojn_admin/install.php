<?php 
if(file_exists("include/common.php")){
	echo "Correctly configured";
	exit();
}

$dir = realpath(dirname(__FILE__));
if(!isset($_POST['domain'])) {
	?>
	The "include" folder must be writeable !
	Please verify that the values are correct:
	<form method='post'>
		<label for='domain'>Domain name:</label>
		<input type='text' name='domain' value='<?php echo $_SERVER['HTTP_HOST']?>' />
		<input type='submit' value='OK' />
	</form>
	<?php
} else {
	file_put_contents("include/common.php", str_replace("<HOSTNAME>", $_POST['domain'], file_get_contents("include/common-def.php")));
	echo "Configuration done. <a href=\"index.php\"> Start using OJN Admin.</a>";
}
?>
