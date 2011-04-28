<?php 
if(!empty($_POST['a']) && !empty($_POST['etime'])) {
	if($_POST['a']=="addwbc")
		$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/ephemeride/addwebcast?time=".$_POST['etime']."&".$ojnAPI->getToken());
	elseif($_POST['a']=="rmwbc")
		$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/ephemeride/removewebcast?time=".$_POST['etime']."&".$ojnAPI->getToken());
	else
		$retour['error'] = "Incorrect Parameters";
	$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
	header("Location: bunny_plugin.php?p=ephemeride");
}
?>
<form method="post">
<fieldset>
<legend>Actions</legend>
<input type="radio" name="a" value="addwbc" checked="true" /> Add Webcast<br />
<input type="radio" name="a" value="rmwbc" /> Remove Webcast<br /><br />
Time:<input type="text" name="etime" />
<input type="submit" value="Enregister">
</fieldset>
</form>
