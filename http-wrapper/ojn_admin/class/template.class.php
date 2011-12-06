<?php
class ojnTemplate {
	private $titre_alt	= "";
	private $titre	= "openJabNab";
	private $soustitre	= "Configuration";
	private $Api;
	private $UInfos;

	public function __construct(&$api) {
		$this->Api=$api;
	}

	public function setUInfos($v) {
		$this->UInfos = $v;
	}

	private function translate($page)
	{
		global $translations;
		return preg_replace(array_map('toRegex', array_keys($translations)), $translations, $page);
	}

	public function display($buffer) {
		$template = file_get_contents(ROOT_SITE.'class/template.tpl.php');
		$Stats = $this->Api->getStats(false);

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
				$Stats['connected_bunnies']."/".$Stats['bunnies'],
				$Stats['enabled_plugins']."/".$Stats['plugins'],
				$Stats['ztamps'],
				$this->makeMenu(),
				$this->makeBunnyMenu()
			);

		$template = preg_replace($pattern, $replace, $template);
		if(preg_match("|<body>(.*)</body>|s", $template, $match))
		{
			$body = $this->translate($match[1]);
			$template = preg_replace("|<body>(.*)</body>|s", $body, $template);
		}
		return $template;
        }

	private function makeMenu() {
		$menu = '<a href="index.php">Accueil</a>';
		if(isset($_SESSION['token']))	{
            $menu .= ' | <a href="account.php">Account</a>';
			$menu .= ' | <a href="bunny.php">Lapin</a>';
			$menu .= ' | <a href="ztamp.php?z">Ztamps</a>';
			if($this->UInfos['isAdmin']) {
				$menu .= ' | <a href="server.php">Serveur</a>';
				$menu .= ' | <a href="api.php">Raw API call</a>';
			}
            $menu .= ' | <a href="index.php?logout">Logout ('.$this->UInfos['username'].')</a>';
		}
		$menu .= ' | <a href="help.php">Aide</a>';
		return $menu;
	}

	private function makeBunnyMenu()	{
		$menu = "";
		if($this->UInfos['token'] != '') {
			$online = $this->Api->getListOfConnectedBunnies(false);
			$bunnies = $this->Api->getListOfBunnies(false);
			if(!empty($bunnies))
				foreach($bunnies as $mac => $bunny)
					$menu .= '<li'.(isset($online[$mac]) ? ' class="online"' : '').'><a href="bunny.php?b='.$mac.'" alt="'.$mac.'" title="'.$mac.'">'.($bunny != "Bunny" ? $bunny : $mac).'</a></li>';
		}
		return !empty($menu) ? $menu : '<li>No Bunny</li>';
	}
}
function toRegex($str)
{
	return "|".$str."|";
}
?>
