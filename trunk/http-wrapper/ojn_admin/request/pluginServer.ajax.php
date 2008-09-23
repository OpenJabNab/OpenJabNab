<?
if(isset($_POST['plug']) && isset($_POST['stat']))
{
	file_get_contents(ROOT_WWW_API."plugins/".$_POST['stat']."Plugin?name=".$_POST['plug']);
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
			<th style="width: 30%">Statut</th>
			<th>Lapins connect&eacute;s</th>
		</tr>
<?
	$system = array('boot', 'locate');
	$i = 0;
	foreach(ojnapi::getListOfPlugins() as $plugin => $name)
	{
?>
		<tr<?=$i++ % 2 == 1 ? " class='l2'" : "" ?>>
			<td><?=$name ?></td>
			<td><?=in_array($plugin, $system) ? "Actif (plugin n&eacute;cessaire)" : (isset($actifs[$plugin]) ? "<a href='javascript:changePluginStatus(\"$plugin\", \"deactivate\")'>D&eacute;sactiver</a>" : "<a href='javascript:changePluginStatus(\"$plugin\", \"activate\")'>Activer</a>") ?></td>
			<td><?=isset($actifs[$plugin]) ? $actifs[$plugin] : "N/A" ?></td>
		</tr>
<?
	}
?>
	</table>
