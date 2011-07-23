<?php
require_once "include/common.php";
if(!isset($_SESSION['token']))
	header('Location: index.php');
	$plugins = $ojnAPI->getListOfPlugins(false);
	if(!isset($plugins[$_GET['p']]))
		header('Location: ztamp.php');
?>
<h1 id="ztamp">Configuration du plugin '<?php echo $plugins[$_GET['p']] ?>' pour le Ztamp '<?php echo !empty($_SESSION['ztamp_name']) ? $_SESSION['ztamp_name'] :$_SESSION['ztamp'] ?>'</h1>
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
if(file_exists('plugins/ztamps/'.$plugin.'.plugin.php'))
	include('plugins/ztamps/'.$plugin.'.plugin.php');
else {
	$_SESSION['message']['error'] = "Aucune configuration pour ce plugin";
	header('Location: ztamp.php');
}
require_once "include/append.php";
?>
