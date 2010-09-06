<?
if(isset($_POST['plug']) && isset($_POST['stat']))
{
	$function = $_POST['stat'] == 'register' ? 'register' : 'unregister';
	file_get_contents(ROOT_WWW_API."bunny/".$_SESSION['bunny']."/".$function."Plugin?name=".$_POST['plug']."&".ojnapi::getToken());
	$actifs = ojnapi::bunnyListOfPlugins($_SESSION['bunny']);
}
?>
<center>
<table style="width: 80%">
	<tr>
		<th>Plugin</th>
		<th colspan="2">Actions</th>
	</tr>
<?
	$i = 0;
	$plugins = ojnapi::getListOfPlugins();
	$bunnyPlugins = ojnapi::getListOfBunnyActivePlugins(true);
	$actifs = ojnapi::bunnyListOfPlugins($_SESSION['bunny']);
	foreach($bunnyPlugins as $plugin)
	{
?>
	<tr<?=$i++ % 2 == 1 ? " class='l2'" : "" ?>>
		<td><?=$plugins[$plugin] ?></td>
		<td width="20%"><a href='javascript:registerBunnyForPlugin("<?=$plugin ?>", "<?=in_array($plugin, $actifs) ? "unregister" : "register" ?>")'><?=in_array($plugin, $actifs) ? "D&eacute;sa" : "A" ?>ctiver le plugin</a></td>
		<td width="20%"><?=in_array($plugin, $actifs)?"<a href='javascript:setupPluginForBunny(\"$plugin\")'>Configurer / Utiliser</a>":""?></td>
	</tr>
<?
	}
?>
</table>
