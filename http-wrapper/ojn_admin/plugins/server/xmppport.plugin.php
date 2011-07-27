<?php
if(!empty($_POST['port'])) {
	$retour = $ojnAPI->getApiString("plugin/xmppport/setXmppPort?port=".$_POST['port']."&".$ojnAPI->getToken());
	$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
	header("Location: server_plugin.php?p=xmppport");
}
$Port = $ojnAPI->getApiString("plugin/xmppport/getXmppPort?".$ojnAPI->getToken());
$Port = isset($Port['value']) ? $Port['value'] : '5222';
?>

<form method="post">
<fieldset>
<legend>Set XMPP Port</legend>
New XMPP Port<input type="text" name="port" value="<?php echo $Port; ?>" />
<input type="submit" value="Enregistrer">
</fieldset>
</form>