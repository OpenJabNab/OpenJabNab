<?php
require_once "include/common.php";
if(!isset($_SESSION['connected']))
	header('Location: index.php');
if(isset($_GET['z']) && empty($_GET['z'])) {
	unset($_SESSION['ztamp']);
	unset($_SESSION['ztamp_name']);
	header("Location: ztamp.php");
}elseif(!empty($_GET['z'])) {
		$_SESSION['ztamp'] = $_GET['z'];
		$ztamps = $ojnAPI->getListOfZtamps(false);
		$_SESSION['ztamp_name'] = !empty($ztamps[$_GET['z']]) ? $ztamps[$_GET['z']] : '';	
		header("Location: ztamp.php");
}elseif((!empty($_GET['plug']) && !empty($_GET['stat'])) || (!empty($_POST['plug']) && !empty($_POST['stat']))) {
	$a = !empty($_GET['stat']) ? $_GET : $_POST;
	$function = $a['stat'] == 'register' ? 'register' : 'unregister';
	$ojnAPI->getApiString('ztamp/'.$_SESSION['ztamp'].'/'.$function.'Plugin?name='.$a['plug'].'&'.$ojnAPI->getToken());
	header('Location: ztamp.php');
} else if(!empty($_GET['ztamp_name'])) {
	$ojnAPI->getApiString("ztamp/".$_SESSION['ztamp']."/setZtampName?name=".$_GET['ztamp_name']."&".$ojnAPI->getToken());
	$_SESSION['ztamp_name'] = $_GET['ztamp_name'];
	header('Location: ztamp.php');
}
if(empty($_SESSION['ztamp'])) {
?>
<h1>Choix du ztamp &agrave; configurer</h1>
<ul>
<?php
	$ztamps = $ojnAPI->getListOfZtamps();
	foreach($ztamps as $ztamp => $nom)	{
?>
	<li><?php echo $nom; ?> (<?php echo $ztamp; ?>) <a href="ztamp.php?z=<?php echo $ztamp; ?>">>></a></li>		
<?php
	}
?>
</ul>
<?php 
} else {
?>
<h1 id="ztamp">Configuration du ztamp '<?php echo !empty($_SESSION['ztamp_name']) ? $_SESSION['ztamp_name'] : $_SESSION['ztamp']; ?>'</h1>
<h2>Le ztamp</h2>
<form>
<fieldset>
<?php 
$plugins = $ojnAPI->getListOfPlugins(false);
$ztampPlugins = $ojnAPI->getListOfBunnyActivePlugins(false);
$actifs = $ojnAPI->ztampListOfPlugins($_SESSION['ztamp'],false);
?>
Nom : <input type="text" name="ztamp_name" value="<?php echo $_SESSION['ztamp_name']; ?>"> <input type="submit" value="Enregistrer">
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
	foreach($ztampPlugins as $plugin){
?>
	<tr<?php echo $i++ % 2 ? " class='l2'" : "" ?>>
		<td><?php echo $plugins[$plugin]; ?></td>
		<td width="20%"><a href='ztamp.php?stat=<?php echo in_array($plugin, $actifs) ? "unregister" : "register"; ?>&plug=<?php echo $plugin; ?>'><?php echo in_array($plugin, $actifs) ? "D&eacute;sa" : "A"; ?>ctiver le plugin</a></td>
		<td width="20%"><?php echo in_array($plugin, $actifs)?"<a href='ztamp_plugin.php?p=$plugin'>Configurer / Utiliser</a>":""?></td>
	</tr>
<? } ?>
</table>
<?php }
require_once "include/append.php";
?>
