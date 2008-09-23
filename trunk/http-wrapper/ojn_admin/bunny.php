<?
if(!isset($_SESSION['connected']))
	header('Location: index.php');
?>
<h1 id="bunny">Configuration du lapin '<?=$_SESSION['bunny'] ?>'</h1>
<?
	$actifs = ojnapi::getListOfActivePlugins();
	foreach($actifs as $plugin => $bunnies)
	{
		if(file_exists("plugins/".$plugin.".ajax.php"))
		{
?>
<div id="plugin_<?=$plugin ?>">
</div>
<br />
<script>$("#plugin_<?=$plugin ?>").load("plugins/<?=$plugin ?>.ajax.php");</script>
<?
		}
	}
?>
