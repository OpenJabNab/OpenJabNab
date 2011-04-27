<?
if(!empty($_POST['p'])) {
	if(!empty($_POST['a'])) {
	if($_POST['a'] == "pack")
		$retour = ojnApi::getApiString("bunny/".$_SESSION['bunny']."/packet/sendPacket?data=".urlencode($_POST['textf'])."&".ojnApi::getToken());
	elseif($_POST['a'] == "msg")
		$retour = ojnApi::getApiString("bunny/".$_SESSION['bunny']."/packet/sendMessage?msg=".urlencode($_POST['textf'])."&".ojnApi::getToken());
	if(isset($retour['ok']))
		$_SESSION['message'] = $retour['ok'];
	else
		$_SESSION['message'] = "Error : ".$retour['error'];
		session_write_close();
		header("Location: bunny_plugin.php?p=packet");
	}
}
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
<legend>Actions</legend>
<input type="radio" name="a" value="pack" checked="true"/> Packet<br />
<input type="radio" name="a" value="msg" /> Message<br />
<input type="text" name="textf" />
<input type="hidden" name="p" value="packet" />
<input type="submit" value="Envoyer" />
</fieldset>
</form>
