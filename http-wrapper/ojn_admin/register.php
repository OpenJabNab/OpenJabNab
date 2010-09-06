<?
$i = 0;
?>
<h1 id="inscription">Inscription</h1>
<div id="inscription-box">
<p>Vous devez rentrer le nom de votre compte, qui vous servira de login par la suite (il peut s'agir du nom du lapin), le nom de votre lapin, son <span class="titleTip" title="Adresse MAC">num&eacute;ro de s&eacute;rie</span> ainsi qu'un mot de passe pour acc&eacute;der &agrave; votre compte ulterieurement.</p>
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
		<td><input type="text" id="frm_serial"></td>
	</tr>
	<tr<?=$i++ % 2 == 1 ? " class='l2'" : "" ?>>
		<td>Nom du lapin :</td>
		<td><input type="text" id="frm_name"></td>
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
</div>
