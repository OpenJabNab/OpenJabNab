<?
if(!isset($_SESSION['connected']))
	header('Location: index.php');

if(!isset($_GET['bunny']) && (!isset($_SESSION['bunny']) || $_SESSION['bunny'] == null))
{
?>
<h1>Choix du lapin &agrave; configurer</h1>
<ul>
<?
	foreach(ojnapi::getListOfConnectedBunnies(true) as $bunny => $nom)
	{
?>
	<li><?=$nom ?> (<?=$bunny ?>) <a href="bunny.php?bunny=<?=$bunny ?>">>></a></li>		
<?
	}
?>
</ul>
<?
}
else
{
	if(isset($_GET['bunny']))
	{
		$_SESSION['bunny'] = $_GET['bunny'];
		ojnApi::getListOfBunnyPlugins($_SESSION['bunny'], true);
		header("Location: bunny.php");
	}

?>
<script>
$(document).ready(function() {
   $("#tablePluginBunny").load("request/pluginBunny.ajax.php");
});
</script>
<h1 id="bunny">Configuration du lapin '<?=$_SESSION['bunny'] ?>'</h1>
<p id="tablePluginBunny">
</p>
<p id="setupPluginBunny">
</p>
<?
}
?>
