<?
if(isset($_POST['frequency']))
{
	$retour = ojnApi::getApiString("bunny/".$_SESSION['bunny']."/taichi/setFrequency?value=".$_POST['frequency']."&".ojnApi::getToken());
	if(isset($retour['ok']))
		$_SESSION['message'] = $retour['ok'];
	else
		$_SESSION['message'] = "Error : ".$retour['error'];
	session_write_close();
	header("Location: bunny_plugin.php?p=taichi");
}
$frequency = ojnApi::getApiString("bunny/".$_SESSION['bunny']."/taichi/getFrequency?".ojnApi::getToken());
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
<legend>Fréquence du Taïchi</legend>
<input type="text" value="<?=$frequency['value'] ?>" name="frequency">
<input type="hidden" name="p" value="taichi">
<input type="submit" value="Enregistrer">
</fieldset>
</form>
