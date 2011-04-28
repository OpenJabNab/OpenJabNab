<?php
if(!empty($_POST['frequency'])) {
	$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/taichi/setFrequency?value=".$_POST['frequency']."&".$ojnAPI->getToken());
	$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
	header("Location: bunny_plugin.php?p=taichi");
}
$frequency = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/taichi/getFrequency?".$ojnAPI->getToken());
$frequency = isset($frequency['value']) ? $frequency['value'] : '';
?>
<form method="post">
<fieldset>
<legend>Frequence du Taichi</legend>
<input type="text" value="<?php echo $frequency ?>" name="frequency">
<input type="submit" value="Enregistrer">
</fieldset>
</form>
