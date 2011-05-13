<?php
class ojnApi {
	private $Bunnies;								/* Registered Bunnies */
	private $ConnectedBunnies;				/* Connected Bunnies */
	private $Plugins;								/* All available plugins */
	private $ActivePlugins;						/* Active Plugins */
	private $BunnyPlugins;						/* Plugins for bunny */
	private $BunnyEnabledPlugins;			/* Plugins enabled for a bunny use */
	
	private $BunnyActivePlugins;				/* Plugins enabled on a bunny */
	
	public function __construct() {
	}

	public function getListOfZtamps($reload = false) {
		if(empty($this->Ztamps) || $reload)
			$this->Ztamps = $this->getApiMapped("ztamps/getListOfZtamps?".$this->getToken());
		return $this->Ztamps;
	}

	public function getListOfBunnies($reload) {
		if(empty($this->Bunnies) || $reload)
			$this->Bunnies = $this->getApiMapped("bunnies/getListOfBunnies?".$this->getToken());
		return $this->Bunnies;
	}

	public function getListOfConnectedBunnies($reload)	{
		if(empty($this->ConnectedBunnies) || $reload)
			$this->ConnectedBunnies = $this->getApiMapped("bunnies/getListOfConnectedBunnies?".$this->getToken());
		return $this->ConnectedBunnies;
	}

	public function getListOfActivePlugins($reload) {
		if(empty($this->ActivePlugins) || $reload)
			$this->ActivePlugins = $this->getApiList("plugins/getListOfEnabledPlugins?".$this->getToken());
		return $this->ActivePlugins;
	}

	public function getListOfPlugins($reload) {
		if(empty($this->Plugins) || $reload)
			$this->Plugins = $this->getApiMapped("plugins/getListOfPlugins?".$this->getToken());
		return $this->Plugins;
	}
	
	public function loginAccount($login, $pass) {
		$loginAccount = $this->getApiString("accounts/auth?login=".$login."&pass=".$pass);
		if(isset($loginAccount['error']))
			$loginAccount['value'] = $loginAccount['error'] == 'BAD_LOGIN' ? 'BAD_LOGIN' : 'BAD_ACCOUNT';
		return $loginAccount['value'];
	}

	public function getListOfBunnyPlugins($reload)	{
		if(empty($this->BunnyPlugins) || $reload)
			$this->BunnyPlugins = $this->getApiList("plugins/getListOfBunnyPlugins?".$this->getToken());
		return $this->BunnyPlugins;
	}

	public function getListOfBunnyActivePlugins($reload) {
		if(empty($this->BunnyEnabledPlugins) || $reload)
			$this->BunnyEnabledPlugins = $this->getApiList("plugins/getListOfBunnyEnabledPlugins?".$this->getToken());
		return $this->BunnyEnabledPlugins;
	}

	public function bunnyListOfPlugins($serial,$reload) {
		if(empty($this->BunnyActivePlugins) || $reload)
			$this->BunnyActivePlugins = $this->getApiList('bunny/'.$serial.'/getListOfActivePlugins?'.$this->getToken());
		return $this->BunnyActivePlugins;
	}

	public function ztampListOfPlugins($serial,$reload) {
		if(empty($this->ZtampActivePlugins) || $reload)
			$this->ZtampActivePlugins = $this->getApiList('ztamp/'.$serial.'/getListOfActivePlugins?'.$this->getToken());
		return $this->ZtampActivePlugins;
	}

	public function getApiList($url) {
		return $this->transformList($this->getApi($url));
	}

	private function getApiMapped($url)	{
		return $this->transformMappedList($this->getApi($url));
	}

	private function getApi($url) {
		return $this->loadXmlString(file_get_contents(ROOT_WWW_API.$url));
	}

	public function getApiString($url) {
		return (array)$this->getApi($url);
	}

	private function getMappedList($url) {
		return $this->transformMappedList($this->getApi($url));
	}

	private function loadXmlString($string) {
		return @simplexml_load_string($string);
	}

	public function setToken($token) {
		$_SESSION['token'] = $token;
	}

	public function getToken() {
		return isset($_SESSION['token']) ? 'token='.$_SESSION['token'] : '';
	}

	private function transformMappedList($mapped) {
		if(isset($mapped->list)) {
			$mapped = (array)$mapped->list->children();

			if(count($mapped)) {
				if(!is_array($mapped['item']))
					$mapped['item'] = array($mapped['item']);
				$mapped = $mapped['item'];
			}
			$temp = array();
			foreach($mapped as $item) {
				$item = (array)$item;
				$temp[$item['key']] = $item['value'];
			}
		} else
			$temp = false;
		return $temp;
	}

	private function transformValue($value) {
		if(isset($value->value))	{
			$value = (array)$value;
			$value = $value['value'];
		} else
			$value = false;
		return $value;
	}

	private function transformList($list) {
		$list = (array)$list;
		$list = (array)$list['list'];
		$temp = array();
		if(is_array($list['item'])) {
			foreach($list['item'] as $item)
				$temp[] = (string)$item;
		}
		else
			$temp = array($list['item']);
		return $temp;
	}
}
?>
