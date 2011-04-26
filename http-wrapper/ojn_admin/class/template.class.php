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
		$ListOfBunnies = ojnApi::getListOfBunnies();
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
				"|<!!BUNNIES!!>|",
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
				$this->makeBunnyMenu(),
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

	function makeBunnyMenu()
	{
		$bunny = "";
		$online = ojnApi::getListOfConnectedBunnies();
		foreach(ojnApi::getListOfBunnies() as $mac => $bunny)
		{
			$menu .= '<li'.(isset($online[$mac]) ? ' class="online"' : '').'><a href="bunny.php?b='.$mac.'" alt="'.$mac.'" title="'.$mac.'">'.($bunny != "Bunny" ? $bunny : $mac).'</a></li>';
		}
		return $menu;
	}
}
?>
