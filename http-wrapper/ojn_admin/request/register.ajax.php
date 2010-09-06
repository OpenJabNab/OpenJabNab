<? 
if(isset($_POST['login']) && isset($_POST['pass']) && isset($_POST['bunny']) && isset($_POST['serial']))
{
	$register = ojnapi::registerNewAccount($_POST['login'], $_POST['pass'], $_POST['bunny'], $_POST['serial']);
	if($register)
	{
?>
<p>F&eacute;licitations, votre compte '<?=$_POST['login'] ?>' est desormais actif, et vous permet de configurer votre lapin <?=$_POST['bunny'] ?> (num&eacute;ro de s&eacute;rie : <?=$_POST['serial'] ?>). Vous pouvez maintenant cliquer sur le bouton situ&eacute; en haut &agrave; droite de l'application pour vous connecter.</p>
<?
	}
	else
	{
?>
<p>Le compte '<?=$_POST['login'] ?>' existe d&eacute;j&agrave;, veuillez choisir un autre login :</p>
<table>
	<tr>
		<th colspan="2">Nouveau compte</th>
	</tr>
	<tr<?=$i++ % 2 == 1 ? " class='l2'" : "" ?>>
		<td>Nom du compte :</td>
		<td><input type="text" id="frm_login"></td>
	</tr>
	<tr<?=$i++ % 2 == 1 ? " class='l2'" : "" ?>>
		<td>Num&eacute;ro de s&eacute;rie du lapin :</td>
		<td><input type="text" id="frm_serial" value="<?=$_POST['serial'] ?>"></td>
	</tr>
	<tr<?=$i++ % 2 == 1 ? " class='l2'" : "" ?>>
		<td>Nom du lapin :</td>
		<td><input type="text" id="frm_name" value="<?=$_POST['bunny'] ?>"></td>
	</tr>
	<tr<?=$i++ % 2 == 1 ? " class='l2'" : "" ?>>
		<td>Mot de passe :</td>
		<td><input type="password" id="frm_pass1"></td>
	</tr>
	<tr<?=$i++ % 2 == 1 ? " class='l2'" : "" ?>>
		<td>Confirmation du mot de passe :</td>
		<td><input type="password" id="frm_pass2"></td>
	</tr>
</table>
<input type="button" value="Cr&eacute;er le compte" onclick="doRegister()">
<?
	}
}
?>
