<?php 
if(!empty($_POST['a']) && !empty($_POST['tag'])) {
	if($_POST['a']=="addtag")
		$retour = ojnApi::getApiString("bunny/".$_SESSION['bunny']."/music/addrfid?tag=".$_POST['tag']."&music=".$_POST['mfile']."&".ojnApi::getToken());
	elseif($_POST['a']=="rmtag")
		$retour = ojnApi::getApiString("bunny/".$_SESSION['bunny']."/music/removerfid?tag=".$_POST['tag']."&".ojnApi::getToken());
	else
		$retour['error'] = "Incorrect Parameters";
	$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
	header("Location: bunny_plugin.php?p=music");
}
?>
<form method="post">
<fieldset>
<legend>Actions</legend>
<input type="radio" name="a" value="addtag" checked="true" /> Add Music: <input type="text" name="mfile" /><br />
<input type="radio" name="a" value="rmtag" /> Remove Tag<br /><br />
Tag:<input type="text" name="tag" /><br />
<input type="submit" value="Enregister">
</fieldset>
</form>
