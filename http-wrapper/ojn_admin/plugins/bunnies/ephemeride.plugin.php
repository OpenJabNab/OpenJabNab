<?php 
if(!empty($_POST['a']) && !empty($_POST['etime'])) {
	if($_POST['a']=="addwbc")
		$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/ephemeride/addwebcast?time=".$_POST['etime']."&".$ojnAPI->getToken());
	else
		$retour['error'] = "Incorrect Parameters";
	$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
	header("Location: bunny_plugin.php?p=ephemeride");
}
else if(!empty($_GET['rmwbc'])) {
		        $retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/ephemeride/removewebcast?time=".$_GET['rmwbc']."&".$ojnAPI->getToken());
		        $_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
			        header("Location: bunny_plugin.php?p=ephemeride");
}
?>
<form method="post">
<fieldset>
<legend>Actions</legend>
<input type="radio" name="a" value="addwbc" checked="true" /> Add Webcast<br />
Time:<input type="text" name="etime" />
<input type="submit" value="Enregister">

<?php
$webcasts = $ojnAPI->getApiList("bunny/".$_SESSION['bunny']."/ephemeride/getwebcastslist?".$ojnAPI->getToken());
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
                <th>Actions</th>
        </tr>
<?php
	        $i = 0;
		        foreach($webcasts as $item) {
?>
        <tr<?php echo $i++ % 2 ? " class='l2'" : "" ?>>
                <td><?php echo $item ?></td>
                <td width="15%"><a href="bunny_plugin.php?p=ephemeride&rmwbc=<?php echo $item ?>">Remove</a></td>
        </tr>
<?php  } ?>
</table>
<?php } ?>


</fieldset>
</form>
