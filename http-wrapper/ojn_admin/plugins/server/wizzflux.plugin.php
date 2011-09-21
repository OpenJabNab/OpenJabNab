<?php
if(!empty($_POST['list'])) {
	$retour = $ojnAPI->getApiString("plugin/wizzflux/setflux?list=".$_POST['list']."&".$ojnAPI->getToken());
	$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
	header("Location: server_plugin.php?p=wizzflux");
}
$list = $ojnAPI->getApiList("plugin/wizzflux/getflux?".$ojnAPI->getToken());
$list = implode(',', $list);
?>

<form method="post">
<fieldset>
<legend>Set flux list (comma separated, without spaces)</legend>
URL<input type="text" name="list" value="<?php echo $list; ?>" style="width:400px"/>
<input type="submit" value="Enregistrer">
</fieldset>
</form>
