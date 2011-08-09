<?php
$Ztamps = $ojnAPI->GetListofZtamps(false);
$Files = $ojnAPI->getApiList("bunny/".$_SESSION['bunny']."/music/getfileslist?".$ojnAPI->getToken());
if(!empty($_POST['a']) && !empty($_POST['tag'])) {
	if($_POST['a']=="addtag" && !empty($_POST['mfile']) && in_array($_POST['mfile'],$Files))
		$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/music/addrfid?tag=".$_POST['tag']."&music=".$_POST['mfile']."&".$ojnAPI->getToken());
	elseif($_POST['a']=="rmtag")
		$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/music/removerfid?tag=".$_POST['tag']."&".$ojnAPI->getToken());
	else
		$retour['error'] = "Incorrect Parameters";
	$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
	header("Location: bunny_plugin.php?p=music");
}
?>
<form method="post">
<fieldset>
<legend>Actions</legend>
<input type="radio" name="a" value="addtag" checked="true" /> Play file
<select name="mfile">
<option value=""></option>
<?php foreach($Files as $f) { ?>
<option value="<?php echo $f; ?>"><?php echo ucfirst(substr($f,0,-4)); ?></option>
<?php } ?>
</select>
<input type="radio" name="a" value="rmtag" /> Remove association<br /><br />
for Ztamp: <select name="tag">
    <option value=""></option>
	<?php foreach($Ztamps as $k=>$v): ?>
	<option value="<?php echo $k; ?>"><?php echo $v; ?> (<?php echo $k; ?>)</option>
	<?php endforeach; ?>
	</select>
<input type="submit" value="Enregister">
</fieldset>
</form>
