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
function sendSleep(sleep) {
        $("#setupPluginBunny").load("plugins/sleep.ajax.php?sleep="+(sleep ? "sleep" : "wakeup"));
}
</script>
<fieldset>
<legend>Advanced sleep & wake up</legend>
<input type="button" value="Coucher le lapin" onclick="sendSleep(true)">&nbsp;
<input type="button" value="Reveiller le lapin" onclick="sendSleep(false)">
<?
if(strlen($retour))
{
?>
<br /><i><?=$retour ?></i>
<?
}
?>
</fieldset>
