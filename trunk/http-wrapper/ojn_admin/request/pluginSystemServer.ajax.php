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
			<th style="width: 30%">Plugin</th>
			<th colspan="3" style="width: 70%">Statut</th>
		</tr>
<?
	$i = 0;
	$required = ojnapi::getListOfRequiredPlugins();
	foreach(ojnapi::getListOfSystemPlugins() as $name)
	{
?>
		<tr<?=$i++ % 2 == 1 ? " class='l2'" : "" ?>>
			<td><?=$name ?></td>
			<td><a href='javascript:reloadSystemPlugin("<?=$name?>")'>Recharger</a></td>
			<td><?=in_array($name, $required) ? "Actif (plugin n&eacute;cessaire)" : (in_array($name, $actifs) ? "<a href='javascript:changeSystemPluginStatus(\"$name\", \"deactivate\")'>D&eacute;sactiver</a>" : "<a href='javascript:changeSystemPluginStatus(\"$plugin\", \"activate\")'>Activer</a>") ?></td>
			<td><a href='javascript:setupPlugin("<?=$name?>")'>Configurer</a></td>
		</tr>
<?
	}
?>
	</table>
