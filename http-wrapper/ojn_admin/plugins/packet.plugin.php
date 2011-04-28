<?php
if(!empty($_POST['a'])) {
	if($_POST['a'] == "pack")
		$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/packet/sendPacket?data=".urlencode($_POST['textf'])."&".$ojnAPI->getToken());
	elseif($_POST['a'] == "msg")
		$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/packet/sendMessage?msg=".urlencode($_POST['textf'])."&".$ojnAPI->getToken());
	else
		$retour['error'] = "Incorrect Parameters";
	$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
	header("Location: bunny_plugin.php?p=packet");
}
?>
<form method="post">
<fieldset>
<legend>Actions</legend>
<input type="radio" name="a" value="pack" checked="true"/> Packet<br />
<input type="radio" name="a" value="msg" /> Message<br />
<input type="text" name="textf" />
<input type="submit" value="Envoyer" />
</fieldset>
</form>
