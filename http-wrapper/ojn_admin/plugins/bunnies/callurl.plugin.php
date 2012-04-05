<?php
$Ztamps = $ojnAPI->GetListofZtamps(false);
if(!empty($_POST['a'])) {
	if($_POST['a']=="addurl") {
		if(!empty($_POST['addC'])) {
			$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/callurl/addurl?url=".urlencode($_POST['addC'])."&".$ojnAPI->getToken());
			$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
			header("Location: bunny_plugin.php?p=callurl");
		}
	}
	else if($_POST['a']=="webcast") {
		if(!empty($_POST['webcastT']) && !empty($_POST['webcastC'])) {
			$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/callurl/addwebcast?url=".urlencode($_POST['webcastC'])."&time=".$_POST['webcastT']."&".$ojnAPI->getToken());
			$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
			header("Location: bunny_plugin.php?p=callurl");
		}
	} else if($_POST['a'] == "rfidadd") {
		if(!empty($_POST['RfCity']) && !empty($_POST['Tag_Rfa']) && isset($Ztamps[$_POST['Tag_Rfa']])) {
			$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/callurl/addrfid?tag=".$_POST['Tag_Rfa']."&url=".urlencode($_POST['RfCity'])."&".$ojnAPI->getToken());
			$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
			header("Location: bunny_plugin.php?p=callurl");
		}
	} else if($_POST['a'] == "rfidd") {
		if(!empty($_POST['Tag_Rf']) && isset($Ztamps[$_POST['Tag_Rf']])) {
			$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/callurl/removerfid?tag=".$_POST['Tag_Rf']."&".$ojnAPI->getToken());
			$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
			header("Location: bunny_plugin.php?p=callurl");
		}
	}
}
else if(!empty($_GET['rp'])) {
	$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/callurl/removeurl?url=".urlencode($_GET['rp'])."&".$ojnAPI->getToken());
	$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
	header("Location: bunny_plugin.php?p=callurl");
}
else if(!empty($_GET['d'])) {
	$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/callurl/setdefaulturl?url=".urlencode($_GET['d'])."&".$ojnAPI->getToken());
	$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
	header("Location: bunny_plugin.php?p=callurl");
}
else if(!empty($_GET['rw'])) {
	$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/callurl/removewebcast?time=".$_GET['rw']."&".$ojnAPI->getToken());
	$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
	header("Location: bunny_plugin.php?p=callurl");
}
$default = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/callurl/getdefaulturl?".$ojnAPI->getToken());
$default = isset($default['value']) ? (string)($default['value']) : '';
$pList = $ojnAPI->getApiList("bunny/".$_SESSION['bunny']."/callurl/geturlslist?".$ojnAPI->getToken());
$wList = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/callurl/getwebcastslist?".$ojnAPI->getToken());

?>
<form method="post">
<fieldset>
<legend>Actions</legend>
<input type="radio" name="a" value="addurl" /> Add a url<input type="text" name="addC"><br />
<input type="radio" name="a" value="webcast" /> Add a webcast at (hh:mm) <input type="text" name="webcastT" maxlength="5" style="width:50px" /> for url <select name="webcastC">
	<option value=""></option>
	<?php if(!empty($pList))
	foreach($pList as $item) { ?>
		<option value="<?php echo urldecode($item) ?>"><?php echo urldecode($item); ?></option>
	<?php } ?>
</select><br />
<input type="radio" name="a" value="rfidadd" /> Add RFID action for <select name="RfCity">
	<option value=""></option>
	<?php  if(!empty($pList))
	foreach($pList as $item) { ?>
		<option value="<?php echo urldecode($item) ?>"><?php echo urldecode($item); ?></option>
	<?php } ?>
</select> on Ztamp: <select name="Tag_Rfa">
    <option value=""></option>
	<?php foreach($Ztamps as $k=>$v): ?>
	<option value="<?php echo $k; ?>"><?php echo $v; ?> (<?php echo $k; ?>)</option>
	<?php endforeach; ?>
	</select><br />
	<input type="radio" name="a" value="rfidd" /> Delete Ztamp association: <select name="Tag_Rf">
    <option value=""></option>
	<?php foreach($Ztamps as $k=>$v): ?>
	<option value="<?php echo $k; ?>"><?php echo $v; ?> (<?php echo $k; ?>)</option>
	<?php endforeach; ?>
	</select><br />
<input type="submit" value="Enregister">

<?php
if(!empty($pList)) {
?>
<hr />
<center>
<table style="width: 80%">
	<tr>
		<th colspan="4">Cities</th>
	</tr>
	<tr>
		<th>City</th>
		<th colspan="2">Actions</th>
	</tr>
<?php
	$i = 0;
	foreach($pList as $item) {
?>
	<tr<?php echo $i++ % 2 ? " class='l2'" : "" ?>>
		<td><?php echo urldecode($item) ?></td>
		<td width="15%"><a href="bunny_plugin.php?p=callurl&rp=<?php echo $item ?>">Remove</a></td>
		<td width="15%"><?php if($default != $item) { ?><a href="bunny_plugin.php?p=callurl&d=<?php echo $item ?>">Set as default</a><?php } else { ?>Default url<?php } ?></td>
	</tr>
<?php } ?>
</table>
<?php
}
if(isset($wList['list']->item)){
?>
<hr />
<center>
<table style="width: 80%">
	<tr>
		<th colspan="3">Webcast</th>
	</tr>
	<tr>
		<th>Time</th>
		<th>Name</th>
		<th>Actions</th>
	</tr>
<?php
	$i = 0;
	foreach($wList['list']->item as $item) {
?>
	<tr<?php echo $i++ % 2 ? " class='l2'" : "" ?>>
		<td><?php echo urldecode($item->key) ?></td>
		<td><?php echo $item->value ?></td>
		<td width="15%"><a href="bunny_plugin.php?p=callurl&rw=<?php echo $item->key ?>">Remove</a></td>
	</tr>
<?php  } ?>
</table>
<?php } ?>
</fieldset>
</form>
