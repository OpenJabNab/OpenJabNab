<?
if(defined("ROOT_SITE"))
{
	echo "Correctly configured";
	die();
}

$dir = realpath(dirname(__FILE__));
if(!isset($_POST['domain']))
{
	?>
	Please verify that the values are correct:
	<form method='post'>
		<label for='domain'>Domain name:</label>
		<input type='text' name='domain' value='<?=$_SERVER['HTTP_HOST']?>' />
		<input type='submit' value='OK' />
	</form>
	<?
}
else
{
	$htaccess  = 'php_value auto_prepend_file "'.$dir.'/include/autoprepend.php"'."\n";
	$htaccess .= 'php_value auto_append_file "'.$dir.'/include/autoappend.php"'."\n";
	file_put_contents(".htaccess", $htaccess);

	file_put_contents("include/autoprepend.php", preg_replace("|<HOSTNAME>|", $_POST['domain'], file_get_contents("include/autoprepend-default.php")));
	echo "Configuration done";
}

?>
