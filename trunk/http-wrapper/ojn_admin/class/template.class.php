<?
class ojnTemplate
{
	var $titre_alt	= "";
	var $titre	= "openJabNab";
	var $soustitre	= "Configuration";

        function ojnTemplate()
        {
		$this->titre_alt = ojnApi::getGlobalAbout();
                ob_start(array(&$this, "display"));
        }

        function display($buffer)
        {
                $template = file_get_contents(ROOT_SITE.'class/template.tpl.php');
		$ListOfConnectedBunnies = ojnApi::getListOfConnectedBunnies();
		$ListOfPlugins = ojnApi::getListOfPlugins();
		$ListOfActivePlugins = ojnApi::getListOfActivePlugins();

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
				$this->titre_alt,
				$this->soustitre,
				$buffer,
				is_array($ListOfConnectedBunnies) ? count($ListOfConnectedBunnies) : '-',
				is_array($ListOfPlugins) ? count($ListOfPlugins) : '-',
				is_array($ListOfActivePlugins) ? count($ListOfActivePlugins) : '-',
				$this->makeMenu(),
			);

		$template = preg_replace($pattern, $replace, $template);
		return $template;
        }

	function makeMenu()
	{
		$menu = '<a href="index.php">Accueil</a>';
		if(isset($_SESSION['connected']))
		{
			$menu .= ' | <a href="bunny.php">Lapin</a>';
			$menu .= ' | <a href="server.php">Serveur</a>';
		}
		return $menu;
	}
}
?>
