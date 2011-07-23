<?php
class ojnApi {
	private $Bunnies;								/* Registered Bunnies */
	private $ConnectedBunnies;				/* Connected Bunnies */
	private $Plugins;								/* All available plugins */
	private $EnabledPlugins;						/* Active Plugins */
	private $BunnyPlugins;						/* Available plugins for a bunny */
	private $BunnyEnabledPlugins;			/* Enabled plugins for a bunny */
	private $ZtampPlugins;						/* Available plugins for a Ztamp */
	private $ZtampEnabledPlugins;			/* Enabled plugins for a Ztamp */

	private $BunnyActivePlugins;				/* Enabled plugins on a specific bunny */

	private $Ztamps;								/* Known Ztamps */
	private $ZtampActivePlugins;			/* Enabled plugins for a ztamp */

	private $Stats;

	public function __construct() {
	}

	public function getStats($reload = false) {
		if(empty($this->Stats) || $reload)
			$this->Stats = $this->getApiString("global/stats?".$this->getToken());
		return $this->Stats;
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

	public function getListOfEnabledPlugins($reload) {
		if(empty($this->EnabledPlugins) || $reload)
			$this->EnabledPlugins = $this->getApiList("plugins/getListOfEnabledPlugins?".$this->getToken());
		return $this->EnabledPlugins;
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

	public function getListOfBunnyEnabledPlugins($reload) {
		if(empty($this->BunnyEnabledPlugins) || $reload)
			$this->BunnyEnabledPlugins = $this->getApiList("plugins/getListOfBunnyEnabledPlugins?".$this->getToken());
		return $this->BunnyEnabledPlugins;
	}

	public function getListOfZtampPlugins($reload)	{
		if(empty($this->ZtampPlugins) || $reload)
			$this->ZtampPlugins = $this->getApiList("plugins/getListOfZtampPlugins?".$this->getToken());
		return $this->ZtampPlugins;
	}

	public function getListOfZtampEnabledPlugins($reload) {
		if(empty($this->ZtampEnabledPlugins) || $reload)
			$this->ZtampEnabledPlugins = $this->getApiList("plugins/getListOfZtampEnabledPlugins?".$this->getToken());
		return $this->ZtampEnabledPlugins;
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

	public function getApiMapped($url)	{
		return $this->transformMappedList($this->getApi($url));
	}

	public function getApiXMLArray($url) {
		return $this->XmlToArray($this->getApi($url));
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
		return simplexml_load_string($string);
	}

	public function setToken($token) {
		$this->GetApi('accounts/settoken?tk='.$token.'&'.$this->getToken());
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
			$value = (string)$value['value'];
		} else
			$value = false;
		return $value;
	}

	private function transformList($list) {
		$list = (array)$list;
        if(isset($list['list']))
    		$list = (array)$list['list'];
		$temp = array();
        if(isset($list['item'])) {
                if(is_array($list['item'])) {
                foreach($list['item'] as $item)
                    $temp[] = $item;
            } else
                $temp = array($list['item']);
        }
		return $temp;
	}

	private function XmlToArray($xml) {
		$name = $xml->getName();
		$nbc = count($xml->children());
		$val = str_replace(array('>','<'),array('&gt;','&lt;'),(string)$xml);
		if($nbc == 0)
			$a=array($name => $val);
		else {
			$t =array();
			foreach($xml->children() as $nme => $xmlchild) {
				$t[]=$this->XmlToArray($xmlchild);
			}
			$a = array($name=>($nbc == 1 ? $t[0] : $t));
		}
		return $a;
	}

}
?>
