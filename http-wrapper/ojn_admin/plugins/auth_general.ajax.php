<?
if(isset($_GET['method']))
{
	file_get_contents(ROOT_WWW_API."plugin/auth/setAuthMethod?name=".$_GET['method']."&".ojnapi::getToken());
}
?>
<script>
function sendMethod() {
        $("#setupPlugin").load("plugins/auth_general.ajax.php?method="+encodeURIComponent($("input[name=method]:checked").val()));
}
</script>
<fieldset>
<legend>Plugin Authentification</legend>
Choix de la m&eacute;thode d'authentification : <br />
<?php
	foreach(ojnapi::getMappedList("plugin/auth/getListOfAuthMethods?") as $key => $value)
	{
		echo "<input type='radio' name='method' value='$key'".($value=="true"?"checked":"")."/>$key<br />";
	}
?>
<input type="button" value="OK" onclick="sendMethod()">
</fieldset>
