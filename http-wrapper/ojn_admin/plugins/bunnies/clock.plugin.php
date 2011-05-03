<?php
if(isset($_POST['voix'])) {
	$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/clock/setVoice?name=".$_POST['voix']."&".$ojnAPI->getToken());
	$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
	header("Location: bunny_plugin.php?p=clock");
}
$voices = $ojnAPI->getApiList("bunny/".$_SESSION['bunny']."/clock/getVoiceList?".$ojnAPI->getToken());
$voice = $ojnAPI->getApiList("bunny/".$_SESSION['bunny']."/clock/getVoice?".$ojnAPI->getToken());
?>
<form method="post">
<fieldset>
<legend>Voix pour l'horloge</legend>
<select name="voix">
<?php foreach($voices as $voix) { ?>
<option value="<?php echo $voix ?>"<?php echo $voix == $voice ? ' selected="selected"' : ''; ?>><?php echo $voix; ?></option>
<?php } ?>
</select>
<input type="submit" value="Enregistrer">
</fieldset>
</form>
