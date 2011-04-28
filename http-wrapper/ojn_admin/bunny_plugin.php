<?php
require_once "include/common.php";
if(!isset($_SESSION['connected']))
	header('Location: index.php');
	$plugins = $ojnAPI->getListOfPlugins();
	if(!isset($plugins[$_GET['p']]))
		header('Location: bunny.php');
?>
<h1 id="bunny">Configuration du plugin '<?php echo $plugins[$_GET['p']] ?>' pour le lapin '<?php echo !empty($_SESSION['bunny_name']) ? $_SESSION['bunny_name'] :$_SESSION['bunny'] ?>'</h1>
<br />
<?php
if(isset($_SESSION['message'])) {
	echo $_SESSION['message'];
	unset($_SESSION['message']);
}
$plugin = !empty($_GET['p']) ? $_GET['p'] : (isset($_POST['p']) ? $_POST['p'] : '');
if(file_exists('plugins/'.$plugin.'.plugin.php'))
	include('plugins/'.$plugin.'.plugin.php');
else
	echo "Aucune configuration pour ce plugin";
require_once "include/append.php";
?>
