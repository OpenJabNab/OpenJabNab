<?
if(isset($_POST['frequency']))
{
	$retour = ojnApi::getApiString("bunny/".$_SESSION['bunny']."/surprise/setFrequency?value=".$_POST['frequency']."&".ojnApi::getToken());
	if(isset($retour['ok']))
		$_SESSION['message'] = $retour['ok'];
	else
		$_SESSION['message'] = "Error : ".$retour['error'];
	session_write_close();
	header("Location: bunny_plugin.php?p=surprise");
}
if(isset($_POST['folder']))
{
	$retour = ojnApi::getApiString("bunny/".$_SESSION['bunny']."/surprise/setFolder?name=".$_POST['folder']."&".ojnApi::getToken());
	if(isset($retour['ok']))
		$_SESSION['message'] = $retour['ok'];
	else
		$_SESSION['message'] = "Error : ".$retour['error'];
	session_write_close();
	header("Location: bunny_plugin.php?p=surprise");
}
$folders = ojnApi::getApiList("bunny/".$_SESSION['bunny']."/surprise/getFolderList?".ojnApi::getToken());
?>
<form method="post">
<?
if(isset($_SESSION['message']))
{
?>
<?=$_SESSION['message'] ?>
<?
$_SESSION['message'] = null;
unset($_SESSION['message']);
}
?>
<fieldset>
<legend>Fréquence pour les surprises</legend>
<input type="text" name="frequency" value="<?=$frequency['value'] ?>">
<input type="hidden" name="p" value="surprise">
<input type="submit" value="Enregistrer">
</fieldset>
</form>
<form method="post">
<?
if(isset($_SESSION['message']))
{
?>
<?=$_SESSION['message'] ?>
<?
$_SESSION['message'] = null;
unset($_SESSION['message']);
}
?>
<fieldset>
<legend>Voix pour les surprises</legend>
<select name="folder">
<?
foreach($folders as $folder)
{
?>
<option value="<?=$folder ?>"><?=$folder ?></option>
<?
}
?>
</select>
<input type="hidden" name="p" value="surprise">
<input type="submit" value="Enregistrer">
</fieldset>
</form>
