<?php 
if(!empty($_POST['p'])) {
	if(!empty($_POST['a']) && !empty($_POST['tag'])) {
		if($_POST['a']=="addtag")
			$retour = ojnApi::getApiString("bunny/".$_SESSION['bunny']."/music/addrfid?tag=".$_POST['tag']."&music=".$_POST['mfile']."&".ojnApi::getToken());
		elseif($_POST['a']=="rmtag")
			$retour = ojnApi::getApiString("bunny/".$_SESSION['bunny']."/music/removerfid?tag=".$_POST['tag']."&".ojnApi::getToken());
		if(isset($retour['ok']))
			$_SESSION['message'] = $retour['ok'];
		else
			$_SESSION['message'] = "Error : ".$retour['error'];
		session_write_close();
		header("Location: bunny_plugin.php?p=music");
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
<input type="hidden" name="p" value="music">
<input type="radio" name="a" value="addtag" checked="true" /> Add Music: <input type="text" name="mfile" /><br />
<input type="radio" name="a" value="rmtag" /> Remove Tag<br /><br />
Tag:<input type="text" name="tag" /><br />
<input type="submit" value="Enregister">
</fieldset>
</form>
