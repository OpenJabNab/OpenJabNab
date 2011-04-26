<?
if(isset($_POST['voix']))
{
	$retour = ojnApi::getApiString("bunny/".$_SESSION['bunny']."/setVoice?name=".$_POST['voix']."&".ojnApi::getToken());
	if(isset($retour['ok']))
		$_SESSION['message'] = $retour['ok'];
	else
		$_SESSION['message'] = "Error : ".$retour['error'];
	session_write_close();
	header("Location: bunny_plugin.php?p=clock");
}
$voixs = ojnApi::getApiList("bunny/".$_SESSION['bunny']."/clock/getVoiceList?".ojnApi::getToken());
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
<legend>Voix pour l'horloge</legend>
<select name="voix">
<?
foreach($voixs as $voix)
{
?>
<option value="<?=$voix ?>"><?=$voix ?></option>
<?
}
?>
</select>
<input type="hidden" name="p" value="clock">
<input type="submit" value="Enregistrer">
</fieldset>
</form>
