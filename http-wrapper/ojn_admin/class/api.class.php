<?
class ojnApi
{
	public function getGlobalAbout()
	{
		return "About";
	}

	public function getListOfConnectedBunnies()
	{
		$ListOfConnectedBunnies = self::getApiMapped("bunnies/getListOfConnectedBunnies?".ojnApi::getToken());
		return $ListOfConnectedBunnies;
	}

	static function getListOfActivePlugins()
	{
		return self::getApiList("plugins/getListOfEnabledPlugins?".ojnApi::getToken());
	}

	static function getListOfPlugins()
	{
		return self::getApiMapped("plugins/getListOfPlugins?".ojnApi::getToken());
	}
	
	static function loginAccount($login, $pass)
	{
		$loginAccount = self::getApiString("accounts/auth?login=".$login."&pass=".$pass);

		if(isset($loginAccount['error']))
			return $loginAccount['error'] == 'BAD_LOGIN' ? 'BAD_LOGIN' : 'BAD_ACCOUNT';
		
		return $loginAccount['value'];
	}

	static function getListOfBunnyPlugins($reload = false)
	{
		return self::getApiList("plugins/getListOfBunnyPlugins?".ojnApi::getToken());
	}

	static function getListOfBunnyActivePlugins($reload = false)
	{
		return self::getApiList("plugins/getListOfBunnyEnabledPlugins?".ojnApi::getToken());
	}

	static function bunnyListOfPlugins($serial)
	{
		return self::getApiList("bunny/$serial/getListOfActivePlugins?".ojnApi::getToken());
	}

	static function getApiList($url)
	{
		$mapped = file_get_contents(ROOT_WWW_API.$url);
		$mapped = self::loadXmlString($mapped);
		return self::transformList($mapped);
	}

	static function getApiMapped($url)
	{
		$mapped = file_get_contents(ROOT_WWW_API.$url);
		$mapped = self::loadXmlString($mapped);
		return self::transformMappedList($mapped);
	}

	static function getApiString($url)
	{
		$string = file_get_contents(ROOT_WWW_API.$url);
		$string = ojnApi::loadXmlString($string);
		return (array)$string;
	}

	static function getMappedList($url)
	{
		return $map = ojnApi::transformMappedList(ojnApi::loadXmlString(file_get_contents(ROOT_WWW_API.$url.ojnApi::getToken())));
	}

	static function loadXmlString($string)
	{
		return @simplexml_load_string($string);
	}

	static function setToken($token)
	{
		return $_SESSION['token'] = $token;
	}

	static function getToken()
	{
		return isset($_SESSION['token']) ? 'token='.$_SESSION['token'] : '';
	}

	static function transformMappedList($mapped)
	{
		if(isset($mapped->list))
		{
			$mapped = (array)$mapped->list->children();

			if(count($mapped))
			{
				if(!is_array($mapped['item']))
					$mapped['item'] = array($mapped['item']);
				$mapped = $mapped['item'];
			}
			$temp = array();
			foreach($mapped as $item)
			{
				$item = (array)$item;
				$temp[$item['key']] = $item['value'];
			}
			return $temp;
		}
		return false;
	}

	static function transformValue($value)
	{
		if(isset($value->value))
		{
			$value = (array)$value;
			return $value['value'];
		}
		return false;
	}

	static function transformList($list)
	{
		$list = (array)$list;
		$list = (array)$list['list'];
		$temp = array();
		if(is_array($list['item']))
		{
			foreach($list['item'] as $item)
				$temp[] = $item;
		}
		else
			$temp = array($list['item']);
		return $temp;
	}
}
?>
