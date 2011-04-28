<?php 
if(!empty($_POST['a'])) {
	if($_POST['a']=="amb")
		$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/test/test?type=ambient&".$ojnAPI->getToken());
	elseif($_POST['a']=="chor")
		$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/test/test?type=chor&".$ojnAPI->getToken());
	else
		$retour['error'] = "Incorrect Parameters";
	$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
	header("Location: bunny_plugin.php?p=test");
}
?>
<form method="post">
<fieldset>
<legend>Actions</legend>
<input type="radio" name="a" value="amb" checked="true" /> Ambient Packet<br />
<input type="radio" name="a" value="chor" /> Choregraphie<br />
<input type="submit" value="Enregister">
</fieldset>
</form>
