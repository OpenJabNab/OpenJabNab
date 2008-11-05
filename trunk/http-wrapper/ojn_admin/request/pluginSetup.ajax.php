<?
if(isset($_GET['plugin']))
{
	if(file_exists(ROOT_SITE."plugins/".$_GET['plugin'].".ajax.php"))
	{
		include(ROOT_SITE."plugins/".$_GET['plugin'].".ajax.php");
	}
	else
	{
?>
Ce plugin n'a pas encore de configuration sp&eacute;cifique.
<?
	}
}
?>
