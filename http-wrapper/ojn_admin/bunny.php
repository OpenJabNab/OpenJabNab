<?php
require_once "include/common.php";
if(!isset($_SESSION['connected']))
	header('Location: index.php');
if(!empty($_GET['b'])) {
		$_SESSION['bunny'] = $_GET['b'];
		$bunnies = $ojnAPI->getListOfBunnies(false);
		$_SESSION['bunny_name'] = !empty($bunnies[$_GET['b']]) ? $bunnies[$_GET['b']] : '';	
		header("Location: bunny.php");
} elseif(isset($_GET['resetpwd'])) {
	$ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/resetPassword?".$ojnAPI->getToken());
	header('Location: bunny.php');
} elseif(!empty($_GET['single']) && !empty($_GET['double'])) {
	$ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/setSingleClickPlugin?name=".$_GET['single']."&".$ojnAPI->getToken());
	$ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/setDoubleClickPlugin?name=".$_GET['double']."&".$ojnAPI->getToken());
	header('Location: bunny.php');
}elseif((!empty($_GET['plug']) && !empty($_GET['stat'])) || (!empty($_POST['plug']) && !empty($_POST['stat']))) {
	$a = !empty($_GET['stat']) ? $_GET : $_POST;
	$function = $a['stat'] == 'register' ? 'register' : 'unregister';
	$ojnAPI->getApiString('bunny/'.$_SESSION['bunny'].'/'.$function.'Plugin?name='.$a['plug'].'&'.$ojnAPI->getToken());
	header('Location: bunny.php');
} else if(!empty($_GET['bunny_name'])) {
	$ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/setBunnyName?name=".$_GET['bunny_name']."&".$ojnAPI->getToken());
	$_SESSION['bunny_name'] = $_GET['bunny_name'];
	header('Location: bunny.php');
}
if(empty($_SESSION['bunny'])) {
?>
<h1>Choix du lapin &agrave; configurer</h1>
<ul>
<?php
	$bunnies = $ojnAPI->getListOfBunnies(false);
	foreach($bunnies as $bunny => $nom)	{
?>
	<li><?php echo $nom; ?> (<?php echo $bunny; ?>) <a href="bunny.php?b=<?php echo $bunny; ?>">>></a></li>		
<?php
	}
?>
</ul>
<?php 
} else {
?>
<h1 id="bunny">Configuration du lapin '<?php echo !empty($_SESSION['bunny_name']) ? $_SESSION['bunny_name'] : $_SESSION['bunny']; ?>'</h1>
<h2>Le lapin</h2>
<form>
<fieldset>
<?php 
$plugins = $ojnAPI->getListOfPlugins(false);
$bunnyPlugins = $ojnAPI->getListOfBunnyActivePlugins(false);
$actifs = $ojnAPI->bunnyListOfPlugins($_SESSION['bunny'],false);
$clicks = $ojnAPI->getApiList("bunny/".$_SESSION['bunny']."/getClickPlugins?".$ojnAPI->getToken());
?>
Nom : <input type="text" name="bunny_name" value="<?php echo $_SESSION['bunny_name']; ?>"> <input type="submit" value="Enregistrer">
</fieldset>
</form>
<form>
<fieldset>
<input name="resetpwd" type="submit" value="Remettre a zero le mot de passe">
</fieldset>
</form>
<form>
<fieldset>
Plugin simple click : <select name="single">
<option value="none">Aucun</option>
<?php foreach($actifs as $plugin) { ?>
<option value="<?=$plugin ?>" <?php echo ($plugin == $clicks[0] ? ' selected="selected"' : '') ?>><?php echo $plugin; ?></option>
<?php } ?>
</select>
Plugin double click : <select name="double">
<option value="none">Aucun</option>
<?php foreach($actifs as $plugin) { ?>
<option value="<?=$plugin ?>" <?php echo ($plugin == $clicks[1] ? ' selected="selected"' : '') ?>><?php echo $plugin; ?></option>
<?php } ?>
</select>
<input type="submit" value="Enregistrer">
</fieldset>
</form>
<h2>Plugins</h2>
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
		<td width="20%"><a href='bunny.php?stat=<?php echo in_array($plugin, $actifs) ? "unregister" : "register"; ?>&plug=<?php echo $plugin; ?>'><?php echo in_array($plugin, $actifs) ? "D&eacute;sa" : "A"; ?>ctiver le plugin</a></td>
		<td width="20%"><?php echo in_array($plugin, $actifs)?"<a href='bunny_plugin.php?p=$plugin'>Configurer / Utiliser</a>":""?></td>
	</tr>
<? } ?>
</table>
<?php }
require_once "include/append.php";
?>
