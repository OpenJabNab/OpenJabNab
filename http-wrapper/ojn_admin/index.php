<?php
if(!defined("ROOT_SITE"))
{
	echo "Please go to the <a href='install.php'>Installation page</a>";
	die();
}
if(isset($_GET['logout']))
{
	unset($_SESSION['connected']);
	unset($_SESSION['bunny']);
	unset($_SESSION['admin']);
	unset($_SESSION['token']);
}
if(count($_POST))
{
	if(isset($_POST['login']) && isset($_POST['password']))
	{
		$retour = ojnapi::loginAccount($_POST['login'], $_POST['password']);
		if(!preg_match("|^BAD_|", $retour))
		{
			$_SESSION['connected'] = true;
			ojnapi::setToken($retour);
		}
		else
		{
			unset($_SESSION['connected']);
			unset($_SESSION['bunny']);
			unset($_SESSION['admin']);
		}
	}
}
?>
<div class="three_cols">
      <h1 id="accueil">Accueil</h1>
      <p>Bienvenue sur la page de configuration de votre nabaztag sur <span class="titleTip" title="serveur open-source pour le nabaztag">openJabNab</span>. Vous avez la possibilit&eacute;
         d'activer ou de d&eacute;sactiver certains plugins, afin que votre lapin r&eacute;ponde pleinement
	 &agrave; vos besoins. De plus, il est possible de planifier l'ex&eacute;cution de t&acirc;ches r&eacute;currentes.</p> 
</div>

<div class="three_cols">
<?
if(isset($_SESSION['connected']) && $_SESSION['connected'] == true)
{
?>
<h1>Deconnection</h1>
Cliquez sur le lien suivant pour vous déconnecter : <a href="index.php?logout">deconnection</a>
<?
}
else
{
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
<?
}
?>
</div>

<div class="three_cols">
      <h1 id="tutorial">Nouveau compte</h1>
<p>Si vous voulez utiliser votre lapin, mais que vous n'avez pas de compte utilisateur, vous pouvez en créer un en cliquant sur le lien suivant : <a href="register.php">Cr&eacute;er un compte utilisateur</a>.</p>
</div>
