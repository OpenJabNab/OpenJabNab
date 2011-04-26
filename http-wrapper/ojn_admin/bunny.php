<?
if(!isset($_SESSION['connected']))
	header('Location: index.php');

if(isset($_GET['reset']))
{
	$retour = ojnApi::getApiString("bunny/".$_SESSION['bunny']."/resetPassword?".ojnApi::getToken());
	header('Location: bunny.php');
}
if(isset($_GET['single']) && isset($_GET['double']))
{
	$retour = ojnApi::getApiString("bunny/".$_SESSION['bunny']."/setSingleClickPlugin?name=".$_GET['single']."&".ojnApi::getToken());
	$retour = ojnApi::getApiString("bunny/".$_SESSION['bunny']."/setDoubleClickPlugin?name=".$_GET['double']."&".ojnApi::getToken());
	header('Location: bunny.php');
}
if(isset($_POST['plug']) && isset($_POST['stat']))
{
	$function = $_POST['stat'] == 'register' ? 'register' : 'unregister';
	file_get_contents(ROOT_WWW_API."bunny/".$_SESSION['bunny']."/".$function."Plugin?name=".$_POST['plug']."&".ojnapi::getToken());
	$actifs = ojnapi::bunnyListOfPlugins($_SESSION['bunny']);
}

if(isset($_GET['bunny_name']))
{
	$retour = ojnApi::getApiString("bunny/".$_SESSION['bunny']."/setBunnyName?name=".$_GET['bunny_name']."&".ojnApi::getToken());
	header('Location: bunny.php');
}
if(!isset($_GET['b']) && (!isset($_SESSION['bunny']) || $_SESSION['bunny'] == null))
{
?>
<h1>Choix du lapin &agrave; configurer</h1>
<ul>
<?
	foreach(ojnapi::getListOfConnectedBunnies(true) as $bunny => $nom)
	{
?>
	<li><?=$nom ?> (<?=$bunny ?>) <a href="bunny.php?b=<?=$bunny ?>">>></a></li>		
<?
	}
?>
</ul>
<?
}
else
{
	if(isset($_GET['b']))
	{
		$_SESSION['bunny'] = $_GET['b'];
		ojnApi::getListOfBunnyPlugins($_SESSION['bunny'], true);
		header("Location: bunny.php");
	}
	if(isset($_GET['register']))
	{
        	file_get_contents(ROOT_WWW_API."bunny/".$_SESSION['bunny']."/registerPlugin?name=".$_GET['register']."&".ojnapi::getToken());

		header("Location: bunny.php");
	}
	if(isset($_GET['unregister']))
	{
        	file_get_contents(ROOT_WWW_API."bunny/".$_SESSION['bunny']."/unregisterPlugin?name=".$_GET['unregister']."&".ojnapi::getToken());

		header("Location: bunny.php");
	}

?>
<h1 id="bunny">Configuration du lapin '<?=$_SESSION['bunny'] ?>'</h1>
<h2>Le lapin</h2>
<form>
<fieldset>
<?
$lapins = ojnApi::getListOfConnectedBunnies();
$plugins = ojnapi::getListOfPlugins();
$bunnyPlugins = ojnapi::getListOfBunnyActivePlugins(true);
$actifs = ojnapi::bunnyListOfPlugins($_SESSION['bunny']);
$clicks = ojnapi::getApiList("bunny/".$_SESSION['bunny']."/getClickPlugins?".ojnApi::getToken());
?>
Nom : <input type="text" name="bunny_name" value="<?=$lapins[$_SESSION['bunny']] ?>"><br />
<input type="submit" value="Enregistrer">
</fieldset>
</form>
<form>
<fieldset>
<input name="reset" type="submit" value="Remettre à zéro le mot de passe">
</fieldset>
</form>
<form>
<fieldset>
<?
$lapins = ojnApi::getListOfConnectedBunnies();
$plugins = ojnapi::getListOfPlugins();
$bunnyPlugins = ojnapi::getListOfBunnyActivePlugins(true);
$actifs = ojnapi::bunnyListOfPlugins($_SESSION['bunny']);
?>
Plugin simple click : <select name="single">
<option value="none">Aucun</option>
<?
foreach($actifs as $plugin)
{
?>
<option value="<?=$plugin ?>" <?=($plugin == $clicks[0] ? ' selected="selected"' : '') ?>><?=$plugin ?></option>
<?
}
?>
</select><br />
Plugin double click : <select name="double">
<option value="none">Aucun</option>
<?
foreach($actifs as $plugin)
{
?>
<option value="<?=$plugin ?>" <?=($plugin == $clicks[1] ? ' selected="selected"' : '') ?>><?=$plugin ?></option>
<?
}
?>
</select><br />
<input type="submit" value="Enregistrer">
</fieldset>
</form>
<h2>Plugins</h2>
<center>
<table style="width: 80%">
	<tr>
		<th>Plugin</th>
		<th colspan="2">Actions</th>
	</tr>
<?
	$i = 0;
	foreach($bunnyPlugins as $plugin)
	{
?>
	<tr<?=$i++ % 2 == 1 ? " class='l2'" : "" ?>>
		<td><?=$plugins[$plugin] ?></td>
		<td width="20%"><a href='bunny.php?<?=in_array($plugin, $actifs) ? "unregister" : "register" ?>=<?=$plugin ?>'><?=in_array($plugin, $actifs) ? "D&eacute;sa" : "A" ?>ctiver le plugin</a></td>
		<td width="20%"><?=in_array($plugin, $actifs)?"<a href='bunny_plugin.php?p=$plugin'>Configurer / Utiliser</a>":""?></td>
	</tr>
<?
	}
?>
</table>
<?
}
?>
