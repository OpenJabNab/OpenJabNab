<?
if(isset($_POST['color']))
{
	$retour = ojnApi::getApiString("bunny/".$_SESSION['bunny']."/colorbreathing/setColor?name=".$_POST['color']."&".ojnApi::getToken());
	if(isset($retour['ok']))
		$_SESSION['message'] = $retour['ok'];
	else
		$_SESSION['message'] = "Error : ".$retour['error'];
	session_write_close();
	header("Location: bunny_plugin.php?p=colorbreathing");
}
$colors = ojnApi::getApiList("bunny/".$_SESSION['bunny']."/colorbreathing/getColorList?".ojnApi::getToken());
$current = ojnApi::getApiString("bunny/".$_SESSION['bunny']."/colorbreathing/getColor?".ojnApi::getToken());
$current = isset($current['ok']) ? $current['ok'] : 'violet';
?>
<form method="post">
<?
if(isset($_SESSION['message']))
{
?>
<?=$_SESSION['message'] ?>
<?
$_SESSION['message'] = null;
unset($_SESSION['message']);
}
?>
<fieldset>
<legend>Couleur de la respiration</legend>
<select name="color">
<?
foreach($colors as $color)
{
?>
<option value="<?=$color ?>"<?=$color==$current ? " selected='selected'" : "" ?>><?=$color ?></option>
<?
}
?>
</select>
<input type="hidden" name="p" value="colorbreathing">
<input type="submit" value="Enregistrer">
</fieldset>
</form>
