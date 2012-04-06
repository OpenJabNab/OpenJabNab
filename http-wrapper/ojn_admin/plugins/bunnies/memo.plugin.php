<?php
$days = array(
1 => "Monday",
2 => "Tuesday",
3 => "Wednesday",
4 => "Thursday",
5 => "Friday",
6 => "Saturday",
7 => "Sunday"
);
if(!empty($_POST['a'])) {
	if($_POST['a']=="dwebcast") {
		if(!empty($_POST['dwebcastT']) && !empty($_POST['dwebcastM'])) {
			$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/memo/adddaily?message=".urlencode($_POST['dwebcastM'])."&time=".$_POST['dwebcastT']."&".$ojnAPI->getToken());
			$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
			header("Location: bunny_plugin.php?p=memo");
		}
	}
	else if($_POST['a']=="webcast") {
		if(!empty($_POST['webcastT']) && !empty($_POST['webcastM']) && !empty($_POST['webcastD'])) {
			$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/memo/addwebcast?message=".urlencode($_POST['webcastM'])."&time=".$_POST['webcastT']."&day=".$_POST['webcastD']."&".$ojnAPI->getToken());
			$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
			header("Location: bunny_plugin.php?p=memo");
		}
	}
}
else if(!empty($_GET['rt']) && !empty($_GET['rd'])) {
	$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/memo/removewebcast?day=".$_GET['rd']."&time=".$_GET['rt']."&".$ojnAPI->getToken());
	$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
	header("Location: bunny_plugin.php?p=memo");
}
else if(!empty($_GET['rw'])) {
	$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/memo/removedaily?time=".$_GET['rw']."&".$ojnAPI->getToken());
	$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
	header("Location: bunny_plugin.php?p=memo");
}
$wDList = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/memo/getdailylist?".$ojnAPI->getToken());
$wList = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/memo/getwebcastslist?".$ojnAPI->getToken());

?>
<form method="post">
<fieldset>
<legend>Actions</legend>
<input type="radio" name="a" value="dwebcast" /> Add a daily webcast at (hh:mm) <input type="text" name="dwebcastT" maxlength="5" style="width:50px" />  : <input type="text" name="dwebcastM">
<br />
<input type="radio" name="a" value="webcast" /> Add a webcast at (hh:mm) <input type="text" name="webcastT" maxlength="5" style="width:50px" /> on <select name="webcastD"><?php foreach($days as $d => $day) { ?><option value="<?php echo $d ?>"><?php echo $day ?></option><?php } ?></select> : <input type="text" name="webcastM">
<br />
<input type="submit" value="Enregister">

<?php
if(isset($wList['list']->item)){
?>
<hr />
<center>
<table style="width: 80%">
	<tr>
		<th colspan="4">Webcast</th>
	</tr>
	<tr>
		<th>Day</th>
		<th>Time</th>
		<th>Name</th>
		<th>Actions</th>
	</tr>
<?php
	$i = 0;
	foreach($wList['list']->item as $item) {
	list($day, $time) = explode("|", $item->key);
?>
	<tr<?php echo $i++ % 2 ? " class='l2'" : "" ?>>
		<td><?php echo $days[$day] ?></td>
		<td><?php echo $time ?></td>
		<td><?php echo $item->value ?></td>
		<td width="15%"><a href="bunny_plugin.php?p=memo&rd=<?php echo $day ?>&rt=<?php echo $time ?>">Remove</a></td>
	</tr>
<?php  } ?>
</table>
<?php } 
if(isset($wDList['list']->item)){
?>
<hr />
<center>
<table style="width: 80%">
	<tr>
		<th colspan="3">Daily Webcast</th>
	</tr>
	<tr>
		<th>Time</th>
		<th>Name</th>
		<th>Actions</th>
	</tr>
<?php
	$i = 0;
	foreach($wDList['list']->item as $item) {
?>
	<tr<?php echo $i++ % 2 ? " class='l2'" : "" ?>>
		<td><?php echo urldecode($item->key) ?></td>
		<td><?php echo $item->value ?></td>
		<td width="15%"><a href="bunny_plugin.php?p=memo&rw=<?php echo $item->key ?>">Remove</a></td>
	</tr>
<?php  } ?>
</table>
<?php } ?>
</fieldset>
</form>
