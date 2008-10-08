<?
class ojnTemplate
{
	var $titre	= "openJabNab";
	var $soustitre	= "Configuration";

        function ojnTemplate()
        {
                ob_start(array(&$this, "display"));
        }

        function display($buffer)
        {
                $template = file_get_contents(ROOT_SITE.'class/template.tpl.php');

		$pattern = array(
				"|<!!TITLE!!>|",
				"|<!!ALTTITLE!!>|",
				"|<!!SUBTITLE!!>|",
				"|<!!CONTENT!!>|",
				"|<!!LAPINS!!>|",
				"|<!!PLUGINS!!>|",
				"|<!!ACTIF!!>|",
				"|<!!MENU!!>|",
			);
		$replace = array(
				$this->titre,
				ojnApi::getGlobalAbout(),
				$this->soustitre,
				$buffer,
				count(ojnApi::getListOfConnectedBunnies()),
				count(ojnApi::getListOfPlugins()),
				count(ojnApi::getListOfActivePlugins()),
				$this->makeMenu(),
			);

		$template = preg_replace($pattern, $replace, $template);
		return $template;
        }

	function makeMenu()
	{
		$menu = '<a href="index.php">Accueil</a>';
		if(isset($_SESSION['connected']))
			$menu .= ' | <a href="bunny.php">Lapin</a>';
		if(isset($_SESSION['connected']) && isset($_SESSION['admin']))
			$menu .= ' | <a href="server.php">Serveur</a>';
		return $menu;
	}
}
?>
