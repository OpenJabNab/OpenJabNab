<?php
require_once "include/common.php";
if(!isset($_SESSION['token']))
	header('Location: index.php');
	$plugins = $ojnAPI->getListOfPlugins(false);
	if(!isset($plugins[$_GET['p']]))
		header('Location: bunny.php');
?>
<h1 id="bunny">Configuration du plugin '<?php echo $plugins[$_GET['p']] ?>' pour le lapin '<?php echo !empty($_SESSION['bunny_name']) ? $_SESSION['bunny_name'] :$_SESSION['bunny'] ?>'</h1>
<br />
<?php
if(isset($_SESSION['message'])) {
	if(strstr($_SESSION['message'],'Error : ')) {?>
	<div class="error_msg">
	<?php } else { ?>
	<div class="ok_msg">
	<?php }
	echo $_SESSION['message']; ?>
	</div>
	<?php unset($_SESSION['message']);
}
$plugin = !empty($_GET['p']) ? $_GET['p'] : (isset($_POST['p']) ? $_POST['p'] : '');
if(file_exists('plugins/bunnies/'.$plugin.'.plugin.php'))
	include('plugins/bunnies/'.$plugin.'.plugin.php');
else {
	$_SESSION['message']['error'] = "Aucune configuration pour ce plugin";
	header('Location: bunny.php');
}
require_once "include/append.php";
?>
