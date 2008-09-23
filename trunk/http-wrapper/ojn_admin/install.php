<?
$dir = realpath(dirname(__FILE__));
$hostname = "<HOSTNAME>";
if($hostname == "<HOSTNAME>")
{
	echo 'The $hostname variable must be filled with your hostname'."\n";
}
else
{
	$htaccess  = 'php_value auto_prepend_file "'.$dir.'/include/autoprepend.php"'."\n";
	$htaccess .= 'php_value auto_append_file "'.$dir.'/include/autoappend.php"'."\n";
	file_put_contents(".htaccess", $htaccess);

	file_put_contents("include/autoprepend.php", preg_replace("|<HOSTNAME>|", "$hostname", file_get_contents("include/autoprepend.php")));
}
?>
