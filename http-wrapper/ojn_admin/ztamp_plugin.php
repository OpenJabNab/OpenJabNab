<?php
require_once "include/common.php";
if(!isset($_SESSION['connected']))
	header('Location: index.php');
	$plugins = $ojnAPI->getListOfPlugins(false);
	if(!isset($plugins[$_GET['p']]))
		header('Location: ztamp.php');
?>
<h1 id="ztamp">Configuration du plugin '<?php echo $plugins[$_GET['p']] ?>' pour le Ztamp '<?php echo !empty($_SESSION['ztamp_name']) ? $_SESSION['ztamp_name'] :$_SESSION['ztamp'] ?>'</h1>
<br />
<?php
if(isset($_SESSION['message'])) {
	echo $_SESSION['message'];
	unset($_SESSION['message']);
}
$plugin = !empty($_GET['p']) ? $_GET['p'] : (isset($_POST['p']) ? $_POST['p'] : '');
if(file_exists('plugins/ztamps/'.$plugin.'.plugin.php'))
	include('plugins/ztamps/'.$plugin.'.plugin.php');
else
	echo "Aucune configuration pour ce plugin";
require_once "include/append.php";
?>
