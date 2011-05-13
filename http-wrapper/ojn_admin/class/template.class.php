<?php
class ojnTemplate {
	private $titre_alt	= "";
	private $titre	= "openJabNab";
	private $soustitre	= "Configuration";
	private $Api;
	
	public function __construct($api) {
		$this->Api=$api;
	}

	public function display($buffer) {
		$template = file_get_contents(ROOT_SITE.'class/template.tpl.php');
		$ListOfConnectedBunnies = $this->Api->getListOfConnectedBunnies(false);
		$ListOfBunnies = $this->Api->getListOfBunnies(false);
		$ListOfPlugins = $this->Api->getListOfPlugins(false);
		$ListOfZtamps = $this->Api->getListOfZtamps(false);
		$ListOfActivePlugins = $this->Api->getListOfActivePlugins(false);
		
		if(empty($ListOfPlugins) && isset($_SESSION['connected']) 
		   && !isset($_POST['login']) && !strpos($_SERVER['REQUEST_URI'],"logout"))
			header('Location: index.php?logout');
		
		$pattern = array(
				"|<!!TITLE!!>|",
				"|<!!ALTTITLE!!>|",
				"|<!!SUBTITLE!!>|",
				"|<!!CONTENT!!>|",
				"|<!!LAPINS!!>|",
				"|<!!PLUGINS!!>|",
				"|<!!ZTAMPS!!>|",
				"|<!!MENU!!>|",
				"|<!!BUNNIES!!>|",
			);
		$replace = array(
				$this->titre,
				$this->titre_alt,
				$this->soustitre,
				$buffer,
				is_array($ListOfConnectedBunnies) ? count($ListOfConnectedBunnies)."/".count($ListOfBunnies) : '-',
				is_array($ListOfPlugins) ? count($ListOfActivePlugins)."/".count($ListOfPlugins) : '-',
				is_array($ListOfZtamps) ? count($ListOfZtamps) :  '-',
				$this->makeMenu(),
				$this->makeBunnyMenu(),
			);

		$template = preg_replace($pattern, $replace, $template);
		return $template;
        }

	private function makeMenu() {
		$menu = '<a href="index.php">Accueil</a>';
		if(isset($_SESSION['connected']))	{
			$menu .= ' | <a href="bunny.php">Lapin</a>';
			$menu .= ' | <a href="ztamp.php?z">Ztamps</a>';
			$menu .= ' | <a href="server.php">Serveur</a>';
		}
		return $menu;
	}

	private function makeBunnyMenu()	{
		$bunny = "";
		$online = $this->Api->getListOfConnectedBunnies(false);
		$bunnies = $this->Api->getListOfBunnies(false);
		if(!empty($bunnies))
			foreach($bunnies as $mac => $bunny)
				$menu .= '<li'.(isset($online[$mac]) ? ' class="online"' : '').'><a href="bunny.php?b='.$mac.'" alt="'.$mac.'" title="'.$mac.'">'.($bunny != "Bunny" ? $bunny : $mac).'</a></li>';
		else
			$menu .='<li>No Bunny</li>';
		return $menu;
	}
}
?>
