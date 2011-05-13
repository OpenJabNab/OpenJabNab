<?php 
$Meths = $ojnAPI->getApiList("plugin/auth/getListOfAuthMethods?".$ojnAPI->getToken());

/* Trick as the API call return an array of XML elements - Needs to be fixed in the plugin's code */
foreach($Meths as $m=>$v)	$Meths[$m] = (array)$v;

if(!empty($_POST['a'])) {
	foreach($Meths as $v)
		if($_POST['a'] == $v["key"]) {
			$ok=true; break;
		}
	if(isset($ok))
		$retour = $ojnAPI->getApiString("plugin/auth/setAuthMethod?name=".$_POST['a']."&".$ojnAPI->getToken());
	else
		$retour['error'] = "Incorrect Parameters";
	$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
	header("Location: server_plugin.php?p=auth");
}
?>
<form method="post">
<fieldset>
<legend>Methodes</legend>
<?php foreach($Meths as $m): ?>
	<input type="radio" name="a" value="<?php echo $m['key']; ?>" <?php if($m['value']=='true') echo 'checked="true"' ; ?>/><?php echo ucfirst($m['key']); ?><br />
<?php endforeach; ?>
<input type="submit" value="Enregister">
</fieldset>
</form>