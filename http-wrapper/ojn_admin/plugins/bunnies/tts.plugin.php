<?php
if(!empty($_POST['text'])) {
	$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/tts/say?text=".urlencode($_POST['text'])."&".$ojnAPI->getToken());
	$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
	header("Location: bunny_plugin.php?p=tts");
}
?>
<form method="post">
<fieldset>
<legend>Texte a envoyer</legend>
<input type="text" name="text" value="">
<input type="submit" value="Enregistrer">
</fieldset>
</form>
