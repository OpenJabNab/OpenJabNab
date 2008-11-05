<?
$retour = "";
if(isset($_GET['sleep']))
{
	$register = file_get_contents(ROOT_WWW_API."plugin/sleep/".($_GET['sleep'] == 'sleep' ? 'goodNight' : 'hello')."?bunny=".$_SESSION['bunny']."&".ojnApi::getToken());
	$register = simplexml_load_string($register);
	$register = (array)$register;
	if(isset($register['error']))
		$retour = "Error : ".$register['error'];
	else
		$retour = $register['ok'];
}
?>
<script>
function sendSleep() {
        $("#setupPluginBunny").load("plugins/sleep.ajax.php?sleep="+$("#plugin_sleep_text").val());
}
</script>
<fieldset>
<legend>Advanced sleep & wake up</legend>
Sleep : <input type="text" id="plugin_sleep_text" style="width: 80%">
<input type="button" value="Action" onclick="sendSleep()">
<?
if(strlen($retour))
{
?>
<br /><i><?=$retour ?></i>
<?
}
?>
</fieldset>
