<?php 
if(isset($_POST['color'])) {
	$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/colorbreathing/setColor?name=".$_POST['color']."&".$ojnAPI->getToken());
	$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
	header("Location: bunny_plugin.php?p=colorbreathing");
}
$colors = $ojnAPI->getApiList("bunny/".$_SESSION['bunny']."/colorbreathing/getColorList?".$ojnAPI->getToken());
$current = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/colorbreathing/getColor?".$ojnAPI->getToken());
$current = isset($current['ok']) ? $current['ok'] : 'violet';
?>
<form method="post">
<fieldset>
<legend>Couleur de la respiration</legend>
<select name="color">
<?php foreach($colors as $color){ ?>
<option value="<?php echo $color; ?>"<?php echo $color==$current ? " selected='selected'" : ""; ?>><?php echo $color; ?></option>
<?php } ?>
</select>
<input type="submit" value="Enregistrer">
</fieldset>
</form>
