<?php 
if(!empty($_POST['a'])) {
	if($_POST['a']=="preset") {
		if(!empty($_POST['presetN']) && !empty($_POST['presetU'])) {
			$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/webradio/addpreset?name=".$_POST['presetN']."&url=".urlencode($_POST['presetU'])."&".$ojnAPI->getToken());
			$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
			header("Location: bunny_plugin.php?p=webradio");
		}
	}
	else if($_POST['a']=="webcast") { 
		if(!empty($_POST['webcastT']) && !empty($_POST['webcastP'])) {
			$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/webradio/addwebcast?name=".$_POST['webcastP']."&time=".$_POST['webcastT']."&".$ojnAPI->getToken());
			$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
			header("Location: bunny_plugin.php?p=webradio");
		}
	}
	else if($_POST['a']=="play") { 
		if(!empty($_POST['playN'])) {
			$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/webradio/play?name=".$_POST['playN']."&".$ojnAPI->getToken());
			$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
			header("Location: bunny_plugin.php?p=webradio");
		}
	}
	else if($_POST['a']=="playurl") { 
		if(!empty($_POST['playU'])) {
			$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/webradio/playurl?url=".$_POST['playU']."&".$ojnAPI->getToken());
			$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
			header("Location: bunny_plugin.php?p=webradio");
		}
	}
}
else if(!empty($_GET['rp'])) {
	$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/webradio/removepreset?name=".$_GET['rp']."&".$ojnAPI->getToken());
	$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
	header("Location: bunny_plugin.php?p=webradio");
}
else if(!empty($_GET['d'])) {
	$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/webradio/setdefault?name=".$_GET['d']."&".$ojnAPI->getToken());
	$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
	header("Location: bunny_plugin.php?p=webradio");
}
else if(!empty($_GET['rw'])) {
	$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/webradio/removewebcast?time=".$_GET['rw']."&".$ojnAPI->getToken());
	$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
	header("Location: bunny_plugin.php?p=webradio");
}
$default = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/webradio/getdefault?".$ojnAPI->getToken());
$default = (string)($default['value']);
$pList = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/webradio/listpreset?".$ojnAPI->getToken());
$wList = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/webradio/listwebcast?".$ojnAPI->getToken());
?>
<form method="post">
<fieldset>
<legend>Actions</legend>
<input type="radio" name="a" value="play" /> Play a preset <input type="text" name="playN"><br />
<input type="radio" name="a" value="playurl" /> Play an url <input type="text" name="playU"><br />
<input type="radio" name="a" value="preset" /> Add a preset <input type="text" name="presetN">, for url <input type="text" name="presetU"><br />
<input type="radio" name="a" value="webcast" /> Add a webcast at (hh:mm) <input type="text" name="webcastT">, for preset <input type="text" name="webcastP"><br />
<input type="submit" value="Enregister">
<?php
if(isset($pList['list']->item))
{
?>
<hr />
<center>
<table style="width: 80%">
	<tr>
		<th colspan="4">Presets</th>
	</tr>
	<tr>
		<th>Name</th>
		<th>Url</th>
		<th colspan="2">Actions</th>
	</tr>
<?php
	$i = 0;
	foreach($pList['list']->item as $item) {
?>
	<tr<?php echo $i++ % 2 ? " class='l2'" : "" ?>>
		<td><?php echo $item->key ?></td>
		<td><?php echo $item->value ?></td>
		<td width="15%"><a href="bunny_plugin.php?p=webradio&rp=<?=$item->key ?>">Remove</a></td>
		<td width="15%"><?php if($default != $item->key) { ?><a href="bunny_plugin.php?p=webradio&d=<?=$item->key ?>">Set as default</a><?php } else { ?>Default preset<?php } ?></td>
	</tr>
<? } ?>
</table>
<?php
}
?>
<?php
if(isset($wList['list']->item))
{
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
		<td><?php echo $item->key ?></td>
		<td><?php echo $item->value ?></td>
		<td width="15%"><a href="bunny_plugin.php?p=webradio&rw=<?=$item->key ?>">Remove</a></td>
	</tr>
<? } ?>
</table>
<?php
}
?>
</fieldset>
</form>
