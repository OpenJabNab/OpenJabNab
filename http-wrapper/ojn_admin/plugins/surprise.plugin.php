<?php
if(!empty($_POST['frequency'])) {
	$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/surprise/setFrequency?value=".$_POST['frequency']."&".$ojnAPI->getToken());
	$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
	header("Location: bunny_plugin.php?p=surprise");
}
if(!empty($_POST['folder'])) {
	$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/surprise/setFolder?name=".$_POST['folder']."&".$ojnAPI->getToken());
	$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
	header("Location: bunny_plugin.php?p=surprise");
}
$folders = $ojnAPI->getApiList("bunny/".$_SESSION['bunny']."/surprise/getFolderList?".$ojnAPI->getToken());
$frequency = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/surprise/getFrequency?".$ojnAPI->getToken());
$frequency = isset($frequency['ok']) ? $frequency['ok'] : '';
?>
<form method="post">
<fieldset>
<legend>Frequence pour les surprises</legend>
<input type="text" name="frequency" value="<?php echo $frequency; ?>">
<input type="submit" value="Enregistrer">
</fieldset>
</form>
<form method="post">
<fieldset>
<legend>Voix pour les surprises</legend>
<select name="folder">
<? foreach($folders as $folder) { ?>
<option value="<?php echo $folder ?>"><?php echo $folder ?></option>
<? } ?>
</select>
<input type="submit" value="Enregistrer">
</fieldset>
</form>
