<?php
require_once "include/common.php";
if(!isset($_SESSION['token']) || !$Infos['isAdmin'])
	header('Location: index.php');

$Plugins = $ojnAPI->getListOfPlugins(false);

if((!empty($_GET['plug']) && !empty($_GET['stat'])) || (!empty($_POST['plug']) && !empty($_POST['stat']))) {
	$a = !empty($_GET['stat']) ? $_GET : $_POST;
	if($a['stat'] == 'activate')
		$function='activate';
	else if($a['stat'] == 'deactivate')
		$function='deactivate';
	else
		$function='reload';
	if(isset($a['plug'],$Plugins))
		$_SESSION['message'] = $ojnAPI->getApiString('plugins/'.$function.'Plugin?name='.$a['plug'].'&'.$ojnAPI->getToken());
	else
		$_SESSION['message']['error'] = "No plugin with such name.";
	header('Location: server.php');
}

?>
<h1 id="config">Configuration du serveur</h1>
<p>
Voici la liste des plugins disponibles sur ce serveur. A partir de cette page, il est possible de les activer, les d&eacute;sactiver, ou encore les configurer pour ceux qui poss&egrave;dent des options &agrave; r&eacute;gler. Certains plugins ne peuvent pas &ecirc;tre d&eacute;sactiv&eacute;s car ils sont n&eacute;cessaires au bon fonctionnement du serveur.
</p>
<?php
if(isset($_SESSION['message']) && empty($_GET)) {
	if(isset($_SESSION['message']['ok'])) { ?>
	<div class="ok_msg">
	<?php	echo $_SESSION['message']['ok'];
	} else { ?>
	<div class="error_msg">
	<?php	echo $_SESSION['message']['error'];
	}
	if(empty($_GET))
		unset($_SESSION['message']);
	echo "</div>";
}
?>
<?php
		$Plugins = $ojnAPI->getListOfPlugins(false);
		$BPlugins = $ojnAPI->getListOfBunnyPlugins(false);
		$ZPlugins = $ojnAPI->getListOfZtampPlugins(false);
		$UPlugins = $BPlugins;
		/* Merge Bunny Plugins and ZTamp Plugins */
		if(!empty($ZPlugins))
			foreach($ZPlugins as $v)
				if(!in_array($v,$BPlugins))
					$BPlugins[] = $v;
		$APlugins = $ojnAPI->getListOfEnabledPlugins(false);
		$SPlugins = $ojnAPI->getApiList("plugins/getListOfSystemPlugins?".$ojnAPI->getToken());
		$RPlugins = $ojnAPI->getApiList("plugins/getListOfRequiredPlugins?".$ojnAPI->getToken());
?>
<center>
<table style="width: 80%">
	<tr>
		<th>Required Plugins</th>
		<th colspan="2">Actions</th>
	</tr>
<?php
	$i = 0;
	foreach($RPlugins as $p){
?>
	<tr<?php echo $i++ % 2 ? " class='l2'" : "" ?>>
		<td><?php echo $Plugins[$p]; ?></td>
		<td width="21%"><a href="server_plugin.php?p=<?php echo $p; ?>">Configurer</a></td>
		<td width="21%"><?php if($Plugins[$p][1]): ?><a href="?stat=reload&plug=<?php echo $p ?>">Recharger</a><?php endif; ?></td>
	</tr>
<?php } ?>
</table>

<table style="width: 80%">
	<tr>
		<th>System Plugins</th>
		<th colspan="3">Actions</th>
	</tr>
<?php
	$i = 0;
	foreach($SPlugins as $p){
?>
	<tr<?php echo $i++ % 2 ? " class='l2'" : "" ?>>
		<td><?php echo $Plugins[$p]; ?></td>
		<td width="14%"><a href="server_plugin.php?p=<?php echo $p; ?>">Configurer</a></td>
		<td <?php echo in_array($p,$APlugins) ? 'width="14%"' : 'colspan="2"'; ?>><a href="?stat=<?php echo in_array($p,$APlugins) ? "deactivate" : "activate"; ?>&plug=<?php echo $p ?>"><?php echo in_array($p,$APlugins) ? "D&eacute;sa" : "A"; ?>ctiver le plugin</a>
		<?php if(in_array($p,$APlugins)): ?><td width="14%"><a href="?stat=reload&plug=<?php echo $p ?>">Recharger</a></td><?php endif; ?>
	</tr>
<?php } ?>
</table>

<table style="width: 80%">
	<tr>
		<th>Bunnies &amp; Ztamps Plugins</th>
		<th colspan="2">Actions</th>
	</tr>
<?php
	$i = 0;
    if(!empty($UPlugins))
	foreach($UPlugins as $p){
?>
	<tr<?php echo $i++ % 2 ? " class='l2'" : "" ?>>
		<td><?php echo $Plugins[$p]; ?></td>
		<td <?php echo in_array($p,$APlugins) ? 'width="21%"' : 'colspan="2"'; ?> ><a href="?stat=<?php echo in_array($p,$APlugins) ? "deactivate" : "activate"; ?>&plug=<?php echo $p ?>"><?php echo in_array($p,$APlugins) ? "D&eacute;sa" : "A"; ?>ctiver le plugin</a>
		<?php if(in_array($p,$APlugins)): ?></td><td width="21%"><a href="?stat=reload&plug=<?php echo $p ?>">Recharger</a><?php endif; ?>
		</td>
	</tr>
<?php } ?>
</table>

</p>
<p id="tableBunnyPluginServer">
</p>
</center>
<h1 id="bunnies">Liste des lapins connect&eacute;s</h1>
<p>Voici la liste des lapins connect&eacute;s sur ce serveur.
</p>
<center>
<table style="width: 80%">
	<tr>
		<th>MAC</th>
		<th>Nom</th>
		<th>Statut</th>
		<th>Actions</th>
	</tr>
<?php
	$i = 0;
	$cbunnies = $ojnAPI->getApiMapped("bunnies/getListofAllConnectedBunnies?".$ojnAPI->getToken());
	$bunnies = $ojnAPI->getApiMapped("bunnies/getListofAllBunnies?".$ojnAPI->getToken());
    if(!empty($bunnies))
	foreach($bunnies as $mac=>$name){
?>
	<tr<?php echo $i++ % 2 ? " class='l2'" : "" ?>>
		<td width="20%"><?php echo $mac; ?></td>
		<td><?php echo $name; ?></td>
		<td width="20%"><?php	echo isset($cbunnies[$mac]) ? 'C': 'D&eacute;c' ?>onnect&eacute;</td>
		<td width="20%"><a href='bunny.php?b=<?php echo $mac; ?>'>Configurer</a>
	</tr>
<?php } ?>
</table>
</center>

<h1 id="ztamps">Liste des Ztamps</h1>
<p>Voici la liste des ztamps enregistr&eacute;s sur ce serveur.</p>
<center>
<table style="width: 80%">
	<tr>
		<th>ID</th>
		<th>Nom</th>
		<th>Actions</th>
	</tr>
<?php
	$i = 0;
	$Ztamps = $ojnAPI->getApiMapped("ztamps/getListOfAllZtamps?".$ojnAPI->getToken());
    if(!empty($Ztamps))
	foreach($Ztamps as $id=>$name){
?>
	<tr<?php echo $i++ % 2 ? " class='l2'" : "" ?>>
		<td width="20%"><?php echo $id; ?></td>
		<td><?php echo $name; ?></td>
		<td width="20%"><a href='ztamp.php?z=<?php echo $id; ?>'>Configurer</a>
	</tr>
<?php } ?>
</table>
</center>

<h1 id="userlist">Liste des Utilisateurs</h1>
<p>Voici la liste des comptes enregistr&eacute;s sur ce serveur.</p>
<center>
<table style="width: 80%">
	<tr>
		<th>Login</th>
		<th>Username</th>
		<th>Statut</th>
	</tr>
<?php
	$i = 0;
	$Users = $ojnAPI->getApiMapped("accounts/GetUserlist?".$ojnAPI->getToken());
	$Online = $ojnAPI->getApiList("accounts/GetConnectedUsers?".$ojnAPI->getToken());
	$Admins = $ojnAPI->getApiList("accounts/GetListOfAdmins?".$ojnAPI->getToken());
    if(!empty($Users))
	foreach($Users as $l=>$name){
?>
	<tr<?php echo $i++ % 2 ? " class='l2'" : "" ?>>
		<td width="20%" <?php echo in_array($l,$Admins) ? 'style="font-weight:bold;"' :''; ?>><?php echo $l; ?></td>
		<td><?php echo $name; ?></td>
		<td width="20%"><?php echo in_array($l,$Online) ? "C":"D&eacute;c"; ?>onnect&eacute;</td>
	</tr>
<?php } ?>
</table>
</center>

<?php
require_once 'include/append.php'
?>
