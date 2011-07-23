<?php
require_once('include/common.php');
if(isset($_SESSION['token']))
	header('Location: index.php');

if(!empty($_POST['name']) && !empty($_POST['login']) &&
        !empty($_POST['pwd']) && !empty($_POST['pwd2'])) {
    if((string)$_POST['pwd'] == (string)$_POST['pwd2']) {
        $retour = $ojnAPI->getApiString('accounts/registerNewAccount?login='.urlencode($_POST['login']).'&username='.urlencode($_POST['name']).'&pass='.$_POST['pwd']);
        $_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
    } else
        $_SESSION['message'] = 'Error : Passwords mismatch. Try again...';
    header("Location: register.php");
}
?>
<?php
if(isset($_SESSION['message']) && empty($_POST)) {
	if(strstr($_SESSION['message'],'Error : ')) {?>
	<div class="error_msg">
	<?php } else { ?>
	<div class="ok_msg">
	<?php }
	echo $_SESSION['message']; ?>
	</div>
	<?php unset($_SESSION['message']);
}
?>
<fieldset>
    <legend>Registration form</legend>
    <form method="post">
    <label>Name<input type="text" name="name" /></label><br />
    <label>Username<input type="text" name="login" /></label> <em>Will be used for login</em><br />
    <label>Password<input type="password" name="pwd" /></label><br />
    <label>Repeat Password<input type="password" name="pwd2" /></label><br />
    <input type="submit" value="Register" />
    </form>
</fieldset>
<?php
require_once('include/append.php');
?>
