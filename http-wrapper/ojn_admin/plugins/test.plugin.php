<?php 
if(!empty($_POST['p'])) {
	if(!empty($_POST['a'])) {
		if($_POST['a']=="amb")
			$retour = ojnApi::getApiString("bunny/".$_SESSION['bunny']."/test/test?type=ambient&".ojnApi::getToken());
		elseif($_POST['a']=="chor")
			$retour = ojnApi::getApiString("bunny/".$_SESSION['bunny']."/test/test?type=chor&".ojnApi::getToken());
		if(isset($retour['ok']))
			$_SESSION['message'] = $retour['ok'];
		else
			$_SESSION['message'] = "Error : ".$retour['error'];
		session_write_close();
		header("Location: bunny_plugin.php?p=test");
	}
} 
?>
<form method="post">
<?php
if(isset($_SESSION['message'])) {
	echo $_SESSION['message'];
	$_SESSION['message'] = null;
	unset($_SESSION['message']);
}
?>
<fieldset>
<legend>Actions</legend>
<input type="hidden" name="p" value="sleep">
<input type="radio" name="a" value="amb" checked="true" /> Ambient Packet<br />
<input type="radio" name="a" value="chor" /> Choregraphie<br />
<input type="submit" value="Enregister">
</fieldset>
</form>
