<?php 
$days = array("", "Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi", "Samedi", "Dimanche");
if(!empty($_POST['a']) && !empty($_POST['cinematime']) && !empty($_POST['cinemaday']) ) {
	if($_POST['a']=="addwbc")
		$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/cinema/addwebcast?time=".$_POST['cinematime']."&day=".$_POST['cinemaday']."&".$ojnAPI->getToken());
	else
		$retour['error'] = "Incorrect Parameters";
	$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
	header("Location: bunny_plugin.php?p=cinema");
}
else if(!empty($_GET['rmwbc'])) {
	list($day, $time) = split("\|", $_GET['rmwbc']);
	$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/cinema/removewebcast?time=".$time."&day=".$day."&".$ojnAPI->getToken());
	$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
	header("Location: bunny_plugin.php?p=cinema");
} 
?>
<form method="post">
<fieldset>
<legend>Actions</legend>
<input type="radio" name="a" value="addwbc" checked="true" /> Add Webcast<br />
<input type="radio" name="a" value="rmwbc" /> Remove Webcast<br /><br />
Time:<input type="text" name="cinematime" /> Day :<select name="cinemaday">
<option value="1">Lundi</option>
<option value="2">Mardi</option>
<option value="3">Mercredi</option>
<option value="4">Jeudi</option>
<option value="5">Vendredi</option>
<option value="6">Samedi</option>
<option value="7">Dimanche</option>
</select>
<input type="submit" value="Enregister">

<?php
$webcasts = $ojnAPI->getApiList("bunny/".$_SESSION['bunny']."/cinema/getwebcastslist?".$ojnAPI->getToken());
if($webcasts){
?>
<hr />
<center>
<table style="width: 80%">
	<tr>
		<th colspan="3">Webcast</th>
	</tr>
	<tr>
		<th>Time</th>
		<th>Day</th>
		<th>Actions</th>
	</tr>
<?php
	$i = 0;
	foreach($webcasts as $item) {
	list($day, $time) = split("\|", $item);
?>
	<tr<?php echo $i++ % 2 ? " class='l2'" : "" ?>>
		<td><?php echo $time ?></td>
		<td><?php echo $days[$day] ?></td>
		<td width="15%"><a href="bunny_plugin.php?p=cinema&rmwbc=<?php echo $item ?>">Remove</a></td>
	</tr>
<?php  } ?>
</table>
<?php } ?>

</fieldset>
</form>
