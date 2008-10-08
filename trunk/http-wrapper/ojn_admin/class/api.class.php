<?
class ojnApi
{
	const mtimeMicro = 10;
	const mtimeMini = 60;
	const mtimeNormal = 600;
	const mtimeMax = 3600;

	static function registerNewAccount($login, $pass, $bunny, $serial)
	{
		$pass = md5($pass);
		$register = file_get_contents(ROOT_WWW_API."accounts/registerNewAccount?login=$login&hash=$pass&name=$bunny&bunny=$serial");
		$register = simplexml_load_string($register);
		$register = (array)$register;
		if(isset($register['error']))
			return false;
		return true;
	}

	static function loginAccount($login, $pass)
	{
		$pass = md5($pass);
		$loginAccount = file_get_contents(ROOT_WWW_API."accounts/loginAccount?login=$login&hash=$pass");
		$loginAccount = simplexml_load_string($loginAccount);
		$loginAccount = (array)$loginAccount;

		if(isset($loginAccount['error']))
			return $loginAccount['error'] == 'BAD_LOGIN' ? 'BAD_LOGIN' : 'BAD_ACCOUNT';
		
		return $loginAccount['value'];
	}

	static function getAccountInfo($login)
	{
		$infos = file_get_contents(ROOT_WWW_API."accounts/getAccountInfo?login=$login");
		$infos = ojnApi::transformMappedList(simplexml_load_string($infos));
		return $infos;
	}

	static function bunnyRegisterPlugin($serial, $plugin)
	{
		$register = file_get_contents(ROOT_WWW_API."bunny/$serial/registerPlugin?name=$plugin");
		$register = simplexml_load_string($register);
		$register = (array)$register;
		if(isset($register['error']))
			return false;
		return true;
	}

	static function bunnyUnregisterPlugin($serial, $plugin)
	{
		$register = file_get_contents(ROOT_WWW_API."bunny/$serial/unregisterPlugin?name=$plugin");
		$register = simplexml_load_string($register);
		$register = (array)$register;
		if(isset($register['error']))
			return false;
		return true;
	}

	static function bunnyListOfPlugins($serial)
	{
		$list = file_get_contents(ROOT_WWW_API."bunny/$serial/getListOfActivePlugins");
		$list = ojnApi::transformList(simplexml_load_string($list));
		return $list;
	}

	static function pluginRegisterCron($plugin, $serial, $interval, $offsetH, $offsetM, $callback = "")
	{
		$register = file_get_contents(ROOT_WWW_API."plugin/$plugin/registerCron?id=$plugin&callback=$callback&interval=$interval&offseth=$offsetH&offsetm=$offsetM");
		$register = simplexml_load_string($register);
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
			$GlobalAbout = ojnApi::transformValue(simplexml_load_string($GlobalAbout));
			file_put_contents(ROOT_SITE.'cache/about.cache.php', '<?php'."\n".'$GlobalAbout = '.var_export($GlobalAbout, true)."\n".'?>');
			return $GlobalAbout;
		}

	}

	static function getListOfConnectedBunnies()
	{
		if(file_exists(ROOT_SITE.'cache/bunnies.cache.php') && time() - filemtime(ROOT_SITE.'cache/bunnies.cache.php') < ojnApi::mtimeMini)
		{
			require(ROOT_SITE.'cache/bunnies.cache.php');
			return $ListOfConnectedBunnies;
		}
		else
		{
			$ListOfConnectedBunnies = file_get_contents(ROOT_WWW_API."bunnies/getListOfConnectedBunnies");
			$ListOfConnectedBunnies = ojnApi::transformMappedList(simplexml_load_string($ListOfConnectedBunnies));
			file_put_contents(ROOT_SITE.'cache/bunnies.cache.php', '<?php'."\n".'$ListOfConnectedBunnies = '.var_export($ListOfConnectedBunnies, true)."\n".'?>');
			return $ListOfConnectedBunnies;
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
			$plugins = file_get_contents(ROOT_WWW_API."plugins/getListOfEnabledPlugins");
			$plugins = ojnApi::transformMappedList(simplexml_load_string($plugins));
			file_put_contents(ROOT_SITE.'cache/plugins_active.cache.php', '<?php'."\n".'$plugins_active = '.var_export($plugins, true)."\n".'?>');
			return $plugins;
		}
	}

	static function getListOfPlugins()
	{
		if(file_exists(ROOT_SITE.'cache/plugins.cache.php') && time() - filemtime(ROOT_SITE.'cache/plugins.cache.php') < ojnApi::mtimeMax)
		{
			require(ROOT_SITE.'cache/plugins.cache.php');
			return $plugins;
		}
		else
		{
			$plugins = file_get_contents(ROOT_WWW_API."plugins/getListOfPlugins");
			$plugins = ojnApi::transformMappedList(simplexml_load_string($plugins));
			file_put_contents(ROOT_SITE.'cache/plugins.cache.php', '<?php'."\n".'$plugins = '.var_export($plugins, true).";\n".'?>');
			return $plugins;
		}
	}

	static function transformMappedList($mapped)
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

	static function transformValue($value)
	{
		$value = (array)$value;
		return $value['value'];
	}

	static function transformList($list)
	{
		$list = (array)$list;
		$list = (array)$list['list'];
		$temp = array();
		foreach($list['item'] as $item)
			$temp[] = $item;
		return $temp;
	}
}
ojnApi::getListOfConnectedBunnies();
ojnApi::getListOfPlugins();
ojnApi::getListOfActivePlugins();
ojnApi::getGlobalAbout();
?>
