<?
$retour = "";
if(isset($_GET['text']))
{
	$register = file_get_contents(ROOT_WWW_API."plugin/tts/say?bunny=".$_SESSION['bunny']."&text=".urlencode($_GET['text'])."&".ojnApi::getToken());
	$register = simplexml_load_string($register);
	$register = (array)$register;
	if(isset($register['error']))
		$retour = "Error : ".$register['error'];
	else
		$retour = $register['ok'];
}
?>
<script>
function sendText() {
        $("#setupPluginBunny").load("plugins/tts.ajax.php?text="+encodeURIComponent($("#plugin_tts_text").val()));
}
</script>
<fieldset>
<legend>Plugin Text-to-Speech</legend>
Texte &agrave; dire : <input type="text" id="plugin_tts_text" style="width: 80%">
<input type="button" value="Parler" onclick="sendText()">
<?
if(strlen($retour))
{
?>
<br /><i><?=$retour ?></i>
<?
}
?>
</fieldset>
