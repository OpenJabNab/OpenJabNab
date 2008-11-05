<?
if(isset($_POST['plug']) && isset($_POST['stat']))
{
	$function = $_POST['stat'] == 'register' ? 'register' : 'Unregister';
	file_get_contents(ROOT_WWW_API."bunny/".$_SESSION['bunny']."/".$function."Plugin?name=".$_POST['plug']."&".ojnapi::getToken());
	$actifs = ojnapi::getListOfActivePlugins(true);
	$bunny = ojnApi::getListOfBunnyPlugins($_SESSION['bunny'], true);
}
else
{
	$actifs = ojnapi::getListOfActivePlugins();
	$bunny = ojnApi::getListOfBunnyPlugins($_SESSION['bunny']);
}
$plugins = ojnapi::getListOfPlugins();
$required = ojnapi::getListOfRequiredPlugins();
$system = ojnapi::getListOfSystemPlugins();
?>
<center>
<table style="width: 80%">
	<tr>
		<th>Plugin</th>
		<th colspan="2">Actions</th>
	</tr>
<?
	$i = 0;
	$actifs = ojnapi::getListOfActivePlugins();
	foreach($actifs as $plugin)
	{
		if(!in_array($plugin, $required) && !in_array($plugin, $system))
		{
?>
	<tr<?=$i++ % 2 == 1 ? " class='l2'" : "" ?>>
		<td><?=$plugins[$plugin] ?></td>
		<td width="20%"><a href='javascript:registerBunnyForPlugin("<?=$plugin ?>", "<?=in_array($plugin, $bunny) ? "unregister" : "register" ?>")'><?=in_array($plugin, $bunny) ? "Dec" : "C" ?>onnecter le lapin</a></td>
		<td width="20%"><a href='javascript:setupPluginForBunny("<?=$plugin ?>")'>Configurer / Utiliser</a></td>
	</tr>
<?
		}
	}
?>
</table>
