<?php 
if(!empty($_POST['a'])) {
	if($_POST['a']=="sleep")
		$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/sleep/sleep?".$ojnAPI->getToken());
	elseif($_POST['a']=="wakeup")
		$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/sleep/wakeup?".$ojnAPI->getToken());
	elseif($_POST['a']=="setup") {
		for($i=0;$i<7;$i++)
			if(empty($_POST['w'.$i]) || empty($_POST['s'.$i])) $i=0xF;
		if($i<0xF) {
			$W = $_POST['w0'];
			$S = $_POST['s0'];
			for($i=1;$i<7;$i++) {
				$W .= ','.$_POST['w'.$i];
				$S .= ','.$_POST['s'.$i];
			}
			$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/sleep/setup?wakeupList=".$W."&sleepList=".$S."&".$ojnAPI->getToken());
		} else
			$retour['error'] = "Incorrect schedule";
	}
	else
		$retour['error'] = "Incorrect Parameters";
	$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
	header("Location: bunny_plugin.php?p=sleep");
}
$wakeup = "";
$sleep = "";
$lists = $ojnAPI->getApiList("bunny/".$_SESSION['bunny']."/sleep/getsetup?".$ojnAPI->getToken());
if(count($lists) == 14) {
	foreach($lists as $i=>$v)
		$lists[$i] = substr($v,0,-3);
} else
	for($i=0;$i<7;$i++) {
		$lists[$i] = '10:00';
		$lists[$i+7] = '18:00';
	}
?>
<form method="post">
<fieldset>
<legend>Actions</legend>
<input type="radio" name="a" value="sleep" /> Coucher<br />
<input type="radio" name="a" value="wakeup" /> Reveiller<br />
<input type="radio" name="a" value="setup" /> Configuration<br />
<table>
<tr>
	<th>Jour</th>
	<th>Lever</th>
	<th>Coucher</th>
</tr>
<?php
$Days = array('Lundi','Mardi','Mercredi','Jeudi','Vendredi','Samedi','Dimanche');
for($i=0;$i<7;$i++): ?>
<tr<?php echo $i % 2 ? " class='l2'" : "" ?>>
	<td><?php echo $Days[$i]; ?></td>
	<td><input type="text" name="w<?php echo $i; ?>" maxlength="5" style="width: 50px" value="<?php echo $lists[$i]; ?>" /></td>
	<td><input type="text" name="s<?php echo $i; ?>" maxlength="5" style="width: 50px" value="<?php echo $lists[$i+7]; ?>"/></td>
</tr>
<?php endfor; ?>
</table>
<input type="submit" value="Enregister">
</fieldset>
</form>
