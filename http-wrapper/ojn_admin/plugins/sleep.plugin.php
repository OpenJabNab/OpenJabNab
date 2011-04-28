<?php 
if(!empty($_POST['a'])) {
	if($_POST['a']=="sleep")
		$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/sleep/sleep?".$ojnAPI->getToken());
	elseif($_POST['a']=="wakeup")
		$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/sleep/wakeup?".$ojnAPI->getToken());
	elseif($_POST['a']=="setup")
		if(!empty($_POST['wakeL']) && !empty($_POST['sleepL']))
			$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/sleep/setup?wakeupList=".$_POST['wakeL']."&sleepList=".$_POST['sleepL']."&".$ojnAPI->getToken());
	else
		$retour['error'] = "Incorrect Parameters";
	$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
	header("Location: bunny_plugin.php?p=sleep");
}
$wakeup = "";
$sleep = "";
$lists = $ojnAPI->getApiList("bunny/".$_SESSION['bunny']."/sleep/getsetup?".$ojnAPI->getToken());
if(count($lists) == 14) {
	$lists = array_chunk($lists, 7);
	$wakeup = preg_replace("|(\d+:\d+):00|", "$1", implode(",", $lists[0]));
	$sleep = preg_replace("|(\d+:\d+):00|", "$1", implode(",", $lists[1]));
}
?>
<form method="post">
<fieldset>
<legend>Actions</legend>
<input type="radio" name="a" value="sleep" /> Sleep<br />
<input type="radio" name="a" value="wakeup" /> Wake Up<br />
<input type="radio" name="a" value="setup" /> Setup: <br />hh:mm,hh:mm,hh:mm.... 7 times for each list<br />
Wakeup List: <input type="text" name="wakeL" value="<?php echo $wakeup; ?>"/>
Sleep List: <input type="text" name="sleepL" value="<?php echo $sleep; ?>"/><br />
<input type="submit" value="Enregister">
</fieldset>
</form>
