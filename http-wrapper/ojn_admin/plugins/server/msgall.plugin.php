<?php
if(!empty($_POST['text'])) {
	$retour = $ojnAPI->getApiString("plugin/msgall/say?text=".urlencode($_POST['text'])."&".$ojnAPI->getToken());
	$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
	header("Location: server_plugin.php?p=msgall");
}
?>
<form method="post">
<fieldset>
<legend>Texte a envoyer &aacute; tous les lapins</legend>
<input type="text" name="text" value="">
<input type="submit" value="Enregistrer">
</fieldset>
</form>
