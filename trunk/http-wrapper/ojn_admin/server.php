<?
if(!isset($_SESSION['connected']))
	header('Location: index.php');
?>
<script>
$(document).ready(function() {
   $("#tablePluginServer").load("request/pluginServer.ajax.php");
   $("#tableBunniesServer").load("request/bunniesServer.ajax.php");
});
</script>
<h1 id="config">Configuration du serveur</h1>
<p>
Voici la liste des plugins disponibles sur ce serveur. A partir de cette page, il est possible de les activer, les d&eacute;sactiver, ou encore les configurer pour ceux qui poss&egrave;dent des options &agrave; r&eacute;gler. Certains plugins ne peuvent pas &ecirc;tre d&eacute;sactiv&eacute;s car ils sont n&eacute;cessaires au bon fonctionnement du serveur.
</p>
<p id="tablePluginServer">
</p>
<h1 id="bunnies">Liste des lapins connect&eacute;s</h1>
<p>Voici la liste des lapins connect&eacute;s sur ce serveur.
</p>
<p id="tableBunniesServer">
</p>
