<?php 
if(!empty($_POST['a']) && !empty($_POST['city'])) {
	if($_POST['a']=="dcity")
		$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/weather/defaultcity?city=".$_POST['city']."&".$ojnAPI->getToken());
	elseif($_POST['a']=="addwbc")
		$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/weather/addwebcast?time=".$_POST['webc']."&city=".$_POST['city']."&".$ojnAPI->getToken());
	elseif($_POST['a']=="rmwbc")
			$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/weather/removewebcast?time=".$_POST['webc']."&city=".$_POST['city']."&".$ojnAPI->getToken());
	else
		$retour['error'] = "Incorrect Parameters";
	$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
	header("Location: bunny_plugin.php?p=weather");
}
$list = $ojnAPI->getApiList("bunny/".$_SESSION['bunny']."/weather/listwebcast?".$ojnAPI->getToken());
if(count($list[0])) {
	$wbc = preg_replace("|(\d+:\d+):00|", "$1", implode(",", $list));
} else
	$wbc = "Empty";
?>
<form method="post">
<fieldset>
<legend>Actions</legend>
Current Webcasts list: <?php echo $wbc; ?><br />
<input type="radio" name="a" value="dcity" /> Set Default City<br />
<input type="radio" name="a" value="addwbc" /> Add Webcast<br />
<input type="radio" name="a" value="rmwbc" /> Remove Webcast<br />
City: <input type="text" name="city" /><br />
Webcast Time: <input type="text" name="webc" /><br />
<input type="submit" value="Enregister">
</fieldset>
</form>
