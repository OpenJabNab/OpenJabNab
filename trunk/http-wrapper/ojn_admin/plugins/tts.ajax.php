<?
$retour = "";
if(isset($_GET['text']))
{
	$register = file_get_contents(ROOT_WWW_API."plugin/tts/say?to=".$_SESSION['serial']."&text=".$_GET['text']);
	$register = simplexml_load_string($register);
	$register = (array)$register;
	if(isset($register['error']))
		$retour = "Error : ".$register['error'];
	$retour = $register['value'];
}
?>
<script>
function sendText() {
        $("#plugin_tts").load("plugins/tts.ajax.php?text="+$("#plugin_tts_text").val());
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
