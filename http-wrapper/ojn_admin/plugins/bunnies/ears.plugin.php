<?php
if(!empty($_POST['friend'])) {
	$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/ears/setFriend?id=".$_POST['friend']."&".$ojnAPI->getToken());
	$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
	header("Location: bunny_plugin.php?p=ears");
}
$friend = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/ears/getFriend?".$ojnAPI->getToken());
$friend = isset($friend['value']) ? $friend['value'] : '';
?>
<form method="post">
<fieldset>
<legend>Friend's Bunny ID (001234567890)</legend>
<input type="text" value="<?php echo $friend ?>" name="friend">
<input type="submit" value="Enregistrer">
</fieldset>
</form>
