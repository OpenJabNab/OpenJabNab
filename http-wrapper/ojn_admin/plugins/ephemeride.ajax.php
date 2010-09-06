<?
$retour = "";
if(isset($_GET['removewebcast']))
{
	$register = file_get_contents(ROOT_WWW_API."plugin/ephemeride/removewebcast?bunny=".$_SESSION['bunny']."&time=".$_GET['removewebcast']."&".ojnApi::getToken());
	$register = simplexml_load_string($register);
	$register = (array)$register;
	if(isset($register['error']))
		$retour = "Error : ".$register['error'];
	else
		$retour = $register['value'];
}
if(isset($_GET['addwebcast']))
{
	$register = file_get_contents(ROOT_WWW_API."plugin/ephemeride/addwebcast?bunny=".$_SESSION['bunny']."&time=".$_GET['addwebcast']."&".ojnApi::getToken());
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
        $("#setupPluginBunny").load("plugins/ephemeride.ajax.php?addwebcast="+encodeURIComponent($("#plugin_ephemeride_add").val()));
}
function sendDel() {
        $("#setupPluginBunny").load("plugins/ephemeride.ajax.php?removewebcast="+encodeURIComponent($("#plugin_ephemeride_del").val()));
}
</script>
<fieldset>
<legend>Plugin ephemeride</legend>
Ajouter heure de diffusion : <input type="text" id="plugin_ephemeride_add" style="width: 70%">
<input type="button" value="Ajouter" onclick="sendAdd()"><br />
Supprimer heure de diffusion : <input type="text" id="plugin_ephemeride_del" style="width: 70%">
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
