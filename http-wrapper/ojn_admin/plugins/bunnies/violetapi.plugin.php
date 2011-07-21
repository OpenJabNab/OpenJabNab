<?php
if(!empty($_POST['a'])) {
	$st = (string)$_POST['a'];
	if($st == "enable" || $st == "disable") {
		$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/violetapi/".$st."?".$ojnAPI->getToken());
		$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
	} else
		$_SESSION['message'] = "Error in choice";
	header("Location: bunny_plugin.php?p=violetapi");
}

$Token = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/violetapi/getToken?".$ojnAPI->getToken());
$Token = isset($Token['value']) ? $Token['value'] : '';
/* Status */
$Status = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/violetapi/getStatus?".$ojnAPI->getToken());
$Status= (!empty($Status['value']) && $Status['value'] == "true") ? true : false;
?>
<form method="post">
<fieldset>
<legend>Actions</legend>
Token: <?php echo $Token ; ?><br />
<input type="radio" name="a" value="enable" <?php echo $Status ? 'checked="checked"' : ''; ?>/> Activer<br />
<input type="radio" name="a" value="disable" <?php echo !$Status ? 'checked="checked"' : ''; ?> /> D&eacute;sactiver<br />
<input type="submit" value="Enregister">
</fieldset>
</form>
