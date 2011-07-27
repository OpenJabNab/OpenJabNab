<?php
if(!empty($_POST['url'])) {
	$retour = $ojnAPI->getApiString("plugin/annuaire/setURL?url=".$_POST['url']."&".$ojnAPI->getToken());
	$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
	header("Location: server_plugin.php?p=annuaire");
}
$url = $ojnAPI->getApiString("plugin/annuaire/getURL?".$ojnAPI->getToken());
$url = isset($url['value']) ? $url['value'] : '';
?>

<form method="post">
<fieldset>
<legend>Set central directory URL</legend>
URL<input type="text" name="url" value="<?php echo $url; ?>" />
<input type="submit" value="Enregistrer">
</fieldset>
</form>