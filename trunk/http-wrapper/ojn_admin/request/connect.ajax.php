<? 
if(isset($_POST['logout']))
{
	unset($_SESSION['connected']);
	unset($_SESSION['bunny']);
	unset($_SESSION['admin']);
	unset($_SESSION['token']);
}
elseif(isset($_POST['login']))
{
	$retour = ojnapi::loginAccount($_POST['login'], $_POST['pass']);
	if(!preg_match("|^BAD_|", $retour))
	{
		$infos = ojnapi::getAccountInfo($_POST['login']);
		$_SESSION['connected'] = true;
		$_SESSION['token'] = $retour;
		foreach($infos as $key => $value)
		{
			if($value == "false")
				$value = false;
			if($value == "true")
				$value = true;

			$_SESSION[strtolower($key)] = $value;
		}
		ojnApi::getListOfConnectedBunnies(true);
		ojnApi::getListOfPlugins(true);
		ojnApi::getListOfActivePlugins(true);
		ojnapi::getListOfSystemPlugins(true);
	}
	else
	{
		unset($_SESSION['connected']);
		unset($_SESSION['bunny']);
		unset($_SESSION['admin']);
	}
}

if(isset($_GET['mode'])) 
{
?>
Lapin : <input type="text" id="login" size="5" style="background: white; border: 1px solid rgb(117,144,174);"> Mot de passe : <input type="password" id="pass" size="5" style="background: white; border: 1px solid rgb(117,144,174);"><a href="javascript:doConnexion();"> &raquo; Connecter &laquo;</a>
<?
}
else
{
	if(isset($_SESSION['connected']))
	{
?>
<a href="javascript:doDisconnexion();">D&eacute;connexion &laquo;</a>

<?
	}
	else
	{
?>
<a href="javascript:loadConnexionBox();">Connexion &laquo;</a>
<?
	}
}
?>
