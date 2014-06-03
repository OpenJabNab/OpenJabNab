<?php
if( !empty($_POST['Frequency']) && !empty($_POST['Email']) && !empty($_POST['Password']) )
{
$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/gmail/setConfig?Frequency=".$_POST['Frequency']."&Email=".$_POST['Email']."&Password=".$_POST['Password']."&".$ojnAPI->getToken());
$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
header("Location: bunny_plugin.php?p=gmail");
}

list($frequency, $email,$password) = $ojnAPI->getApiList("bunny/".$_SESSION['bunny']."/gmail/getConfig?".$ojnAPI->getToken());

?>

<form method="post">
<fieldset>
<legend>Configuration of your Gmail account</legend>
Frequency <i>(in minutes)</i>:<br>
<input type="text" name="Frequency" maxlength="30" style="width: 50px" value="<?php echo $frequency; ?>" /></td>
<br><br>
Email <i>(for example john.doe@gmail.com)</i>:<br>
<input type="text" name="Email" maxlength="30" style="width: 250px" value="<?php echo $email; ?>" /></td>
<br><br>
Password <i>(gmail account password)</i>:<br>
<input type="password" name="Password" maxlength="30" style="width: 200px" value="<?php echo $password; ?>" /></td>
<br>

<input type="submit" value="Save">
</fieldset>
</form>

