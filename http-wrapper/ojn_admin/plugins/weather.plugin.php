<?php 
if(!empty($_POST['p'])) {
	if(!empty($_POST['a']) && !empty($_POST['city'])) {
		if($_POST['a']=="dcity")
			$retour = ojnApi::getApiString("bunny/".$_SESSION['bunny']."/weather/defaultcity?city=".$_POST['city']."&".ojnApi::getToken());
		elseif($_POST['a']=="addwbc")
			$retour = ojnApi::getApiString("bunny/".$_SESSION['bunny']."/weather/addwebcast?time=".$_POST['webc']."&city=".$_POST['city']."&".ojnApi::getToken());
		elseif($_POST['a']=="rmwbc")
				$retour = ojnApi::getApiString("bunny/".$_SESSION['bunny']."/weather/removewebcast?time=".$_POST['webc']."&city=".$_POST['city']."&".ojnApi::getToken());
		if(isset($retour['ok']))
			$_SESSION['message'] = $retour['ok'];
		else
			$_SESSION['message'] = "Error : ".$retour['error'];
		session_write_close();
		header("Location: bunny_plugin.php?p=weather");
	}
} 
$list = ojnApi::getApiList("bunny/".$_SESSION['bunny']."/weather/listwebcast?".ojnApi::getToken());
if(count($list[0])) {
	$wbc = preg_replace("|(\d+:\d+):00|", "$1", implode(",", $list));
} else
	$wbc = "Empty";
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
Current Webcasts list: <?php echo $wbc; ?><br />
<input type="hidden" name="p" value="weather">
<input type="radio" name="a" value="dcity" /> Set Default City<br />
<input type="radio" name="a" value="addwbc" /> Add Webcast<br />
<input type="radio" name="a" value="rmwbc" /> Remove Webcast<br />
City: <input type="text" name="city" /><br />
Webcast Time: <input type="text" name="webc" /><br />
<input type="submit" value="Enregister">
</fieldset>
</form>
