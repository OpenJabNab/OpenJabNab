<?php
require_once "include/common.php";
if(!isset($_SESSION['token']))
	header('Location: index.php');
if(!empty($_GET['b'])) {
		$_SESSION['bunny'] = $_GET['b'];
		$bunnies = $ojnAPI->getListOfBunnies(false);
		$_SESSION['bunny_name'] = !empty($bunnies[$_GET['b']]) ? $bunnies[$_GET['b']] : '';
		header("Location: bunny.php");
} elseif(isset($_GET['resetpwd'])) {
	$_SESSION['message'] = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/resetPassword?".$ojnAPI->getToken());
	header('Location: bunny.php');
} elseif(isset($_GET['disconnect'])) {
	$_SESSION['message'] = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/disconnect?".$ojnAPI->getToken());
	header('Location: bunny.php');
} elseif(isset($_GET['resetown'])) {
	$_SESSION['message'] = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/resetOwner?".$ojnAPI->getToken());
	header('Location: bunny.php');
} elseif(!empty($_GET['single']) && !empty($_GET['double'])) {
	$msg = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/setSingleClickPlugin?name=".$_GET['single']."&".$ojnAPI->getToken());
	$msg1 = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/setDoubleClickPlugin?name=".$_GET['double']."&".$ojnAPI->getToken());
	$s = isset($msg['error']) ? $msg['error'] : $msg['ok']. '<br />';
	$s .= isset($msg1['error']) ? $msg1['error'] : $msg1['ok'];
	$msg = isset($msg['error']) || isset($msg1['error']) ? 'error' : 'ok';
		$_SESSION['message'] = array($msg=>$s);
	header('Location: bunny.php');
}elseif((!empty($_GET['plug']) && !empty($_GET['stat'])) || (!empty($_POST['plug']) && !empty($_POST['stat']))) {
	$a = !empty($_GET['stat']) ? $_GET : $_POST;
	$function = $a['stat'] == 'register' ? 'register' : 'unregister';
	$_SESSION['message'] = $ojnAPI->getApiString('bunny/'.$_SESSION['bunny'].'/'.$function.'Plugin?name='.$a['plug'].'&'.$ojnAPI->getToken());
	header('Location: bunny.php');
} else if(!empty($_GET['bunny_name'])) {
	$_SESSION['message'] = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/setBunnyName?name=".urlencode($_GET['bunny_name'])."&".$ojnAPI->getToken());
	$_SESSION['bunny_name'] = $_GET['bunny_name'];
	header('Location: bunny.php');
}

if(!empty($_GET['aVAPI'])) {
	$st = (string)$_GET['aVAPI'];
	if($st == "enable" || $st == "disable")
		$_SESSION['message'] = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/".$st."VAPI?".$ojnAPI->getToken());
	else
		$_SESSION['message'] = "Error: Error in choice";
	header("Location: bunny.php");
}

if(empty($_SESSION['bunny'])) {
?>
<h1>Choix du lapin &agrave; configurer</h1>
<ul>
<?php
	$bunnies = $ojnAPI->getListOfBunnies(false);
    if(!empty($bunnies))
	foreach($bunnies as $bunny => $nom)	{
?>
	<li><?php echo $nom; ?> (<?php echo $bunny; ?>) <a href="bunny.php?b=<?php echo $bunny; ?>">>></a></li>
<?php
	}
?>
</ul>
<?php
} else {
$Token = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/getVAPIToken?".$ojnAPI->getToken());
$Token = isset($Token['value']) ? $Token['value'] : '';
/* Status */
$Status = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/getVAPIStatus?".$ojnAPI->getToken());
$Status= (!empty($Status['value']) && $Status['value'] == "true") ? true : false;

?>
<h1 id="bunny">Configuration du lapin '<?php echo !empty($_SESSION['bunny_name']) ? $_SESSION['bunny_name'] : $_SESSION['bunny']; ?>'</h1>
<h2>Le lapin</h2>
<form method="get">
<fieldset>
<legend>Configuration</legend>
<?php
$plugins = $ojnAPI->getListOfPlugins(false);
$bunnyPlugins = $ojnAPI->getListOfBunnyEnabledPlugins(false);
$actifs = $ojnAPI->bunnyListOfPlugins($_SESSION['bunny'],false);
$clicks = $ojnAPI->getApiList("bunny/".$_SESSION['bunny']."/getClickPlugins?".$ojnAPI->getToken());
?>
Nom : <input type="text" name="bunny_name" value="<?php echo $_SESSION['bunny_name']; ?>"><input type="submit" value="Enregistrer">
</form><br /><br />
<form method="get">
Plugin simple click : <select name="single">
<option value="none">Aucun</option>
<?php foreach($actifs as $plugin) { ?>
<option value="<?php echo $plugin; ?>" <?php echo ($plugin == $clicks[0] ? ' selected="selected"' : '') ?>><?php echo $plugins[$plugin]; ?></option>
<?php } ?>
</select><br />
Plugin double click : <select name="double">
<option value="none">Aucun</option>
<?php foreach($actifs as $plugin) { ?>
<option value="<?php echo $plugin; ?>" <?php echo ($plugin == $clicks[1] ? ' selected="selected"' : '') ?>><?php echo $plugins[$plugin]; ?></option>
<?php } ?>
</select><br />
<input type="submit" value="Enregistrer">
</form><br />
VioletAPIToken: <?php echo $Token ; ?><br />
<form method="get">
VioletAPI: <input type="radio" name="aVAPI" value="enable" <?php echo $Status ? 'checked="checked"' : ''; ?>/> Activer
<input type="radio" name="aVAPI" value="disable" <?php echo !$Status ? 'checked="checked"' : ''; ?> /> D&eacute;sactiver
<input type="submit" value="Enregister">
</form>
<form method="get">
<input name="disconnect" type="submit" value="Deconnecter le lapin">
</form>
</fieldset>
<?php if($Infos['isAdmin']): ?>
<fieldset>
<legend>Debug features</legend>
<form method="get">
<input name="resetpwd" type="submit" value="Remettre a zero le mot de passe">
<input name="resetown" type="submit" value="Liberer le lapin de son maitre">
</form>
</fieldset>
<?php endif; ?>
<h2>Plugins</h2>
<?php
if(isset($_SESSION['message']) && empty($_GET)) {
	if(isset($_SESSION['message']['ok'])) { ?>
	<div class="ok_msg">
	<?php	echo $_SESSION['message']['ok'];
	} else { ?>
	<div class="error_msg">
	<?php	echo $_SESSION['message']['error'];
	}
	if(empty($_GET));
		unset($_SESSION['message']);
	echo "</div>";
}
?>
<center>
<table style="width: 80%">
	<tr>
		<th>Plugin</th>
		<th colspan="2">Actions</th>
	</tr>
<?php
	$i = 0;
	foreach($bunnyPlugins as $plugin){
?>
	<tr<?php echo $i++ % 2 ? " class='l2'" : "" ?>>
		<td><?php echo $plugins[$plugin]; ?></td>
		<td <?php echo in_array($plugin, $actifs) ? 'width="20%"' : 'colspan="2"'; ?>><a href='bunny.php?stat=<?php echo in_array($plugin, $actifs) ? "unregister" : "register"; ?>&plug=<?php echo $plugin; ?>'><?php echo in_array($plugin, $actifs) ? "D&eacute;sa" : "A"; ?>ctiver le plugin</a></td>
		<?php if(in_array($plugin, $actifs)): ?><td width="20%"><?php echo in_array($plugin, $actifs)?"<a href='bunny_plugin.php?p=$plugin'>Configurer / Utiliser</a>":""?></td><?php endif; ?>
	</tr>
<?php } ?>
</table>
<?php }
require_once "include/append.php";
?>
