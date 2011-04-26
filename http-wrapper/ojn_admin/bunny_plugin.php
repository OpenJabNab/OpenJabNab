<?
if(!isset($_SESSION['connected']))
	header('Location: index.php');

	$plugins = ojnapi::getListOfPlugins();
?>
<h1 id="bunny">Configuration du plugin '<?=$plugins[$_GET['p']] ?>' pour le lapin '<?=$_SESSION['bunny'] ?>'</h1>
<br />
<?
$plugin = isset($_GET['p']) ? $_GET['p'] : (isset($_POST['p']) ? $_POST['p'] : '');
if(file_exists('plugins/'.$plugin.'.plugin.php'))
	include('plugins/'.$plugin.'.plugin.php');
else
	echo "Aucune configuration pour ce plugin";

?>
