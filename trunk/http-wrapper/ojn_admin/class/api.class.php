<?
class ojnApi
{
	const mtimeMicro = 10;
	const mtimeMini = 60;
	const mtimeNormal = 600;
	const mtimeMax = 3600;

	static function registerNewAccount($login, $username, $pass)
	{
		$register = file_get_contents(ROOT_WWW_API."accounts/registerNewAccount?login=$login&pass=$pass&username=$username&".ojnApi::getToken());
		$register = ojnApi::loadXmlString($register);
		$register = (array)$register;
		if(isset($register['error']))
			return false;
		return true;
	}

	static function addBunny($login, $bunnyid)
	{
		$addBunny = file_get_contents(ROOT_WWW_API."accounts/addBunny?login=$login&bunnyid=$bunnyid&".ojnApi::getToken());
		$addBunny = ojnApi::loadXmlString($addBunny);
		$addBunny = (array)$addBunny;
		if(isset($addBunny['error']))
			return false;
		return true;
	}

	static function removeBunny($login, $bunnyid)
	{
		$removeBunny = file_get_contents(ROOT_WWW_API."accounts/removeBunny?login=$login&bunnyid=$bunnyid&".ojnApi::getToken());
		$removeBunny = ojnApi::loadXmlString($removeBunny);
		$removeBunny = (array)$removeBunny;
		if(isset($removeBunny['error']))
			return false;
		return true;
	}

	static function loginAccount($login, $pass)
	{
		$loginAccount = file_get_contents(ROOT_WWW_API."accounts/auth?login=$login&pass=$pass");
		$loginAccount = ojnApi::loadXmlString($loginAccount);
		$loginAccount = (array)$loginAccount;

		if(isset($loginAccount['error']))
			return $loginAccount['error'] == 'BAD_LOGIN' ? 'BAD_LOGIN' : 'BAD_ACCOUNT';
		
		return $loginAccount['value'];
	}

	static function getAccountInfo($login)
	{
		$infos = file_get_contents(ROOT_WWW_API."accounts/getAccountInfo?login=$login");
		$infos = ojnApi::transformMappedList(ojnApi::loadXmlString($infos));
		return $infos;
	}

	static function bunnyRegisterPlugin($serial, $plugin)
	{
		$register = file_get_contents(ROOT_WWW_API."bunny/$serial/registerPlugin?name=$plugin");
		$register = ojnApi::loadXmlString($register);
		$register = (array)$register;
		if(isset($register['error']))
			return false;
		return true;
	}

	static function bunnyUnregisterPlugin($serial, $plugin)
	{
		$register = file_get_contents(ROOT_WWW_API."bunny/$serial/unregisterPlugin?name=$plugin");
		$register = ojnApi::loadXmlString($register);
		$register = (array)$register;
		if(isset($register['error']))
			return false;
		return true;
	}

	static function bunnyListOfPlugins($serial)
	{
		$list = file_get_contents(ROOT_WWW_API."bunny/$serial/getListOfActivePlugins");
		$list = ojnApi::transformList(ojnApi::loadXmlString($list));
		return $list;
	}

	static function pluginRegisterCron($plugin, $serial, $interval, $offsetH, $offsetM, $callback = "")
	{
		$register = file_get_contents(ROOT_WWW_API."plugin/$plugin/registerCron?id=$plugin&callback=$callback&interval=$interval&offseth=$offsetH&offsetm=$offsetM");
		$register = ojnApi::loadXmlString($register);
		$register = (array)$register;
		if(isset($register['error']))
			return false;
		return true;
	}

	static function getGlobalAbout()
	{
		if(file_exists(ROOT_SITE.'cache/about.cache.php') && time() - filemtime(ROOT_SITE.'cache/about.cache.php') < ojnApi::mtimeMax)
		{
			require(ROOT_SITE.'cache/about.cache.php');
			return $GlobalAbout;
		}
		else
		{
			$GlobalAbout = file_get_contents(ROOT_WWW_API."global/about");
			$GlobalAbout = ojnApi::transformValue(ojnApi::loadXmlString($GlobalAbout));
			file_put_contents(ROOT_SITE.'cache/about.cache.php', '<?php'."\n".'$GlobalAbout = '.var_export($GlobalAbout, true)."\n".'?>');
			return $GlobalAbout;
		}

	}

	static function getListOfConnectedBunnies($reload = false)
	{
		if(file_exists(ROOT_SITE.'cache/bunnies.cache.php') && time() - filemtime(ROOT_SITE.'cache/bunnies.cache.php') < ojnApi::mtimeMini && !$reload)
		{
			require(ROOT_SITE.'cache/bunnies.cache.php');
			return $ListOfConnectedBunnies;
		}
		else
		{
			$ListOfConnectedBunnies = file_get_contents(ROOT_WWW_API."bunnies/getListOfConnectedBunnies?".ojnApi::getToken());
			$ListOfConnectedBunnies = ojnApi::transformMappedList(ojnApi::loadXmlString($ListOfConnectedBunnies));
			file_put_contents(ROOT_SITE.'cache/bunnies.cache.php', '<?php'."\n".'$ListOfConnectedBunnies = '.var_export($ListOfConnectedBunnies, true)."\n".'?>');
			return $ListOfConnectedBunnies;
		}

	}

	static function getListOfRequiredPlugins($reload = false)
	{
		if(file_exists(ROOT_SITE.'cache/plugins_required.cache.php') && time() - filemtime(ROOT_SITE.'cache/plugins_required.cache.php') < ojnApi::mtimeMax && !$reload)
		{
			require(ROOT_SITE.'cache/plugins_required.cache.php');
			return $plugins_required;
		}
		else
		{
			$plugins = file_get_contents(ROOT_WWW_API."plugins/getListOfRequiredPlugins?".ojnApi::getToken());
			$plugins = ojnApi::transformList(ojnApi::loadXmlString($plugins));
			file_put_contents(ROOT_SITE.'cache/plugins_required.cache.php', '<?php'."\n".'$plugins_required = '.var_export($plugins, true)."\n".'?>');
			return $plugins;
		}
	}

	static function getListOfSystemPlugins($reload = false)
	{
		if(file_exists(ROOT_SITE.'cache/plugins_system.cache.php') && time() - filemtime(ROOT_SITE.'cache/plugins_system.cache.php') < ojnApi::mtimeMax && !$reload)
		{
			require(ROOT_SITE.'cache/plugins_system.cache.php');
			return $plugins_system;
		}
		else
		{
			$plugins = file_get_contents(ROOT_WWW_API."plugins/getListOfSystemPlugins?".ojnApi::getToken());
			$plugins = ojnApi::transformList(ojnApi::loadXmlString($plugins));
			file_put_contents(ROOT_SITE.'cache/plugins_system.cache.php', '<?php'."\n".'$plugins_system = '.var_export($plugins, true)."\n".'?>');
			return $plugins;
		}
	}

	static function getListOfBunnyPlugins($reload = false)
	{
		if(file_exists(ROOT_SITE.'cache/plugins_bunny.cache.php') && time() - filemtime(ROOT_SITE.'cache/plugins_bunny.cache.php') < ojnApi::mtimeMini && !$reload)
		{
			require(ROOT_SITE.'cache/plugins_bunny.cache.php');
			return ${"plugins_bunny"};
		}
		else
		{
			$plugins = file_get_contents(ROOT_WWW_API."plugins/getListOfBunnyPlugins?".ojnApi::getToken());
			$plugins = ojnApi::transformList(ojnApi::loadXmlString($plugins));
			file_put_contents(ROOT_SITE.'cache/plugins_bunny.cache.php', '<?php'."\n".'$plugins_bunny = '.var_export($plugins, true)."\n".'?>');
			return $plugins;
		}
	}

	static function getListOfActivePlugins($reload = false)
	{
		if(file_exists(ROOT_SITE.'cache/plugins_active.cache.php') && time() - filemtime(ROOT_SITE.'cache/plugins_active.cache.php') < ojnApi::mtimeMini && !$reload)
		{
			require(ROOT_SITE.'cache/plugins_active.cache.php');
			return $plugins_active;
		}
		else
		{
			$plugins = file_get_contents(ROOT_WWW_API."plugins/getListOfEnabledPlugins?".ojnApi::getToken());
			$plugins = ojnApi::transformList(ojnApi::loadXmlString($plugins));
			file_put_contents(ROOT_SITE.'cache/plugins_active.cache.php', '<?php'."\n".'$plugins_active = '.var_export($plugins, true)."\n".'?>');
			return $plugins;
		}
	}

	static function getListOfPlugins($reload = false)
	{
		if(file_exists(ROOT_SITE.'cache/plugins.cache.php') && time() - filemtime(ROOT_SITE.'cache/plugins.cache.php') < ojnApi::mtimeMax && !$reload)
		{
			require(ROOT_SITE.'cache/plugins.cache.php');
			return $plugins;
		}
		else
		{
			$plugins = file_get_contents(ROOT_WWW_API."plugins/getListOfPlugins?".ojnApi::getToken());
			$plugins = ojnApi::transformMappedList(ojnApi::loadXmlString($plugins));
			file_put_contents(ROOT_SITE.'cache/plugins.cache.php', '<?php'."\n".'$plugins = '.var_export($plugins, true).";\n".'?>');
			return $plugins;
		}
	}
	
	static function getMappedList($url)
	{
		return $map = ojnApi::transformMappedList(ojnApi::loadXmlString(file_get_contents(ROOT_WWW_API.$url.ojnApi::getToken())));
	}

	static function loadXmlString($string)
	{
		return @simplexml_load_string($string);
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
ojnApi::getListOfConnectedBunnies();
ojnApi::getListOfPlugins();
ojnApi::getListOfActivePlugins();
ojnApi::getGlobalAbout();
ojnapi::getListOfSystemPlugins();
?>
