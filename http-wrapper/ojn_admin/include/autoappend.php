<?
if(!preg_match("|\.ajax\.php|", $_SERVER['REQUEST_URI']))
{
	ob_end_flush();
}
?>
