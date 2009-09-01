<?
$retour = "";
if(isset($_GET['removewebcast']))
{
	$register = file_get_contents(ROOT_WWW_API."plugin/tv/removewebcast?bunny=".$_SESSION['bunny']."&time=".$_GET['removewebcast']."&".ojnApi::getToken());
	$register = simplexml_load_string($register);
	$register = (array)$register;
	if(isset($register['error']))
		$retour = "Error : ".$register['error'];
	else
		$retour = $register['value'];
}
if(isset($_GET['addwebcast']))
{
	$register = file_get_contents(ROOT_WWW_API."plugin/tv/addwebcast?bunny=".$_SESSION['bunny']."&time=".$_GET['addwebcast']."&".ojnApi::getToken());
	$register = simplexml_load_string($register);
	$register = (array)$register;
	if(isset($register['error']))
		$retour = "Error : ".$register['error'];
	else
		$retour = $register['value'];
}
?>
<script>
function sendAdd() {
        $("#setupPluginBunny").load("plugins/tv.ajax.php?addwebcast="+encodeURIComponent($("#plugin_tv_add").val()));
}
function sendDel() {
        $("#setupPluginBunny").load("plugins/tv.ajax.php?removewebcast="+encodeURIComponent($("#plugin_tv_del").val()));
}
</script>
<fieldset>
<legend>Plugin tv</legend>
Ajouter heure de diffusion : <input type="text" id="plugin_tv_add" style="width: 70%">
<input type="button" value="Ajouter" onclick="sendAdd()"><br />
Supprimer heure de diffusion : <input type="text" id="plugin_tv_del" style="width: 70%">
<input type="button" value="Supprimer" onclick="sendDel()">
<?
if(strlen($retour))
{
?>
<br /><i><?=$retour ?></i>
<?
}
?>
</fieldset>
