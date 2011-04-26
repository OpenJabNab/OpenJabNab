<?
if(isset($_POST['text']))
{
	$retour = ojnApi::getApiString("bunny/".$_SESSION['bunny']."/tts/say?text=".urlencode($_POST['text'])."&".ojnApi::getToken());
	if(isset($retour['ok']))
		$_SESSION['message'] = $retour['ok'];
	else
		$_SESSION['message'] = "Error : ".$retour['error'];
	session_write_close();
	header("Location: bunny_plugin.php?p=tts");
}
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
<legend>Texte à envoyer</legend>
<input type="text" name="text" value="">
<input type="hidden" name="p" value="tts">
<input type="submit" value="Enregistrer">
</fieldset>
</form>
