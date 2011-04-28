<?php
require_once "include/common.php";

if(isset($_GET['logout'])) {
	unset($_SESSION['connected']);
	unset($_SESSION['bunny']);
	unset($_SESSION['bunny_name']);
	unset($_SESSION['admin']);
	unset($_SESSION['token']);
	header("Location: index.php");
}
if(!empty($_POST['login']) && !empty($_POST['password'])) {
	$r = $ojnAPI->loginAccount($_POST['login'], $_POST['password']);
	if(!strpos($r,"AD_")) {
		$_SESSION['connected'] = true;
		$ojnAPI->setToken($r);
	} 
	header("Location: index.php");
}
?>
<div class="three_cols">
      <h1 id="accueil">Accueil</h1>
      <p>Bienvenue sur la page de configuration de votre nabaztag sur <span class="titleTip" title="serveur open-source pour le nabaztag">openJabNab</span>. Vous avez la possibilit&eacute;
         d'activer ou de d&eacute;sactiver certains plugins, afin que votre lapin r&eacute;ponde pleinement
	 &agrave; vos besoins. De plus, il est possible de planifier l'ex&eacute;cution de t&acirc;ches r&eacute;currentes.</p> 
</div>

<div class="three_cols">
<?php
if(isset($_SESSION['connected']) && $_SESSION['connected']) {
?>
<h1>D&eacute;connexion</h1>
Cliquez sur le lien suivant pour vous d&eacute;connecter : <a href="index.php?logout">D&eacute;connexion</a>
<?php
} else {
?>
      <h1 id="tutorial">Connection</h1>
      <form method="post">
	<dl>
	<dt>Login</dt>
	<dd><input type="text" name="login"></dd>
	<dt>Mot de passe</dt>
	<dd><input type="password" name="password"></dd>
	</dl>
	<input type="submit" value="Se connecter">
	</form>
<?php
}
?>
</div>

<div class="three_cols">
      <h1 id="tutorial">Nouveau compte</h1>
<p>Si vous voulez utiliser votre lapin, mais que vous n'avez pas de compte utilisateur, vous pouvez en cr&eacute;er un en cliquant sur le lien suivant : <a href="register.php">Cr&eacute;er un compte utilisateur</a>.</p>
</div>
<?php
require_once("include/append.php");
?>
