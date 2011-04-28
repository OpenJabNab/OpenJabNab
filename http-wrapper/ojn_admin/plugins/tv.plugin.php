<?php 
if(!empty($_POST['p'])) {
	if(!empty($_POST['a']) && !empty($_POST['tvtime'])) {
		if($_POST['a']=="addwbc")
			$retour = ojnApi::getApiString("bunny/".$_SESSION['bunny']."/tv/addwebcast?time=".$_POST['tvtime']."&".ojnApi::getToken());
		elseif($_POST['a']=="rmwbc")
			$retour = ojnApi::getApiString("bunny/".$_SESSION['bunny']."/tv/removewebcast?time=".$_POST['tvtime']."&".ojnApi::getToken());
		if(isset($retour['ok']))
			$_SESSION['message'] = $retour['ok'];
		else
			$_SESSION['message'] = "Error : ".$retour['error'];
		session_write_close();
		header("Location: bunny_plugin.php?p=tv");
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
<input type="hidden" name="p" value="tv">
<input type="radio" name="a" value="addwbc" checked="true" /> Add Webcast<br />
<input type="radio" name="a" value="rmwbc" /> Remove Webcast<br /><br />
Time:<input type="text" name="tvtime" />
<input type="submit" value="Enregister">
</fieldset>
</form>
