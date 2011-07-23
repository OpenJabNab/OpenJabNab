<?php
require_once "include/common.php";

if(!isset($_SESSION['token']) || !$Infos['isAdmin'])
	header('Location: index.php');
$plugins = $ojnAPI->getListOfPlugins(false);
if(!isset($plugins[$_GET['p']]))
		header('Location: server.php');
?>
<h1 id="bunny">Configuration du plugin '<?php echo $plugins[$_GET['p']] ?></h1>
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
if(file_exists('plugins/server/'.$plugin.'.plugin.php'))
	include('plugins/server/'.$plugin.'.plugin.php');
else {
	$_SESSION['message']['error'] = "Aucune configuration pour ce plugin";
	header('Location: server.php');
}
require_once "include/append.php";
?>
