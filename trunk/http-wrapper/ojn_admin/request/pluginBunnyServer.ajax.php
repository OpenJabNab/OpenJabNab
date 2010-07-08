<?
if(isset($_POST['plug']) && isset($_POST['stat']))
{
	file_get_contents(ROOT_WWW_API."plugins/".$_POST['stat']."Plugin?name=".$_POST['plug']."&".ojnapi::getToken());
	$actifs = ojnapi::getListOfActivePlugins(true);
}
else if(isset($_POST['plug']) && isset($_POST['reload']))
{
	file_get_contents(ROOT_WWW_API."plugins/reloadPlugin?name=".$_POST['plug']."&".ojnapi::getToken());
	$actifs = ojnapi::getListOfActivePlugins(true);
}
else
{
	$actifs = ojnapi::getListOfActivePlugins();
}
?>
<center>
	<table style="width: 80%">
		<tr>
			<th style="width: 40%">Plugin</th>
			<th colspan="2" style="width: 40%">Statut</th>
		</tr>
<?
	$i = 0;
	foreach(ojnapi::getListOfBunnyPlugins() as $name)
	{
?>
		<tr<?=$i++ % 2 == 1 ? " class='l2'" : "" ?>>
			<td><?=$name ?></td>
			<td><a href='javascript:reloadBunnyPlugin("<?=$name?>")'>Recharger</a></td>
			<td><?=in_array($name, $actifs) ? "<a href='javascript:changeBunnyPluginStatus(\"$name\", \"deactivate\")'>D&eacute;sactiver</a>" : "<a href='javascript:changeBunnyPluginStatus(\"$name\", \"activate\")'>Activer</a>" ?></td>
		</tr>
<?
	}
?>
	</table>
