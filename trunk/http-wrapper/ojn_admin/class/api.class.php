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
		$infos = simplexml_load_string($infos);
		$infos = (array)$infos->list->children();

		if(count($infos))
		{
			if(!is_array($infos['item']))
				$infos['item'] = array($infos['item']);
			$infos = $infos['item'];
		}
		$temp = array();
		foreach($infos as $bunny)
		{
			$bunny = (array)$bunny;
			$temp[$bunny['key']] = $bunny['value'];
		}

		return $temp;
	}

	static function getListOfConnectedBunnies()
	{
		if(file_exists(ROOT_SITE.'cache/bunnies.cache.php') && time() - filemtime(ROOT_SITE.'cache/bunnies.cache.php') < ojnApi::mtimeMini)
		{
			require(ROOT_SITE.'cache/bunnies.cache.php');
			return $bunnies;
		}
		else
		{
			$bunnies = file_get_contents(ROOT_WWW_API."bunnies/getListOfConnectedBunnies");

			$bunnies = simplexml_load_string($bunnies);
			$bunnies = (array)$bunnies->list->children();

			if(count($bunnies))
			{
				if(!is_array($bunnies['item']))
					$bunnies['item'] = array($bunnies['item']);
				$bunnies = $bunnies['item'];
			}
			$temp = array();
			foreach($bunnies as $bunny)
			{
				$bunny = (array)$bunny;
				$temp[$bunny['key']] = $bunny['value'];
			}
			file_put_contents(ROOT_SITE.'cache/bunnies.cache.php', '<?php'."\n".'$bunnies = '.var_export($temp, true)."\n".'?>');
			return $temp;
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

			$plugins = simplexml_load_string($plugins);
			$plugins = (array)$plugins->list->children();

			if(count($plugins))
			{
				if(!is_array($plugins['item']))
					$plugins['item'] = array($plugins['item']);
				$plugins = $plugins['item'];
			}
			$temp = array();
			foreach($plugins as $plugin)
			{
				$plugin = (array)$plugin;
				$temp[$plugin['key']] = $plugin['value'];
			}
			file_put_contents(ROOT_SITE.'cache/plugins_active.cache.php', '<?php'."\n".'$plugins_active = '.var_export($temp, true)."\n".'?>');
			return $temp;
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

			$plugins = simplexml_load_string($plugins);
			$plugins = (array)$plugins->list->children();

			if(count($plugins))
			{
				if(!is_array($plugins['item']))
					$plugins['item'] = array($plugins['item']);
				$plugins = $plugins['item'];
			}
			$temp = array();
			foreach($plugins as $plugin)
			{
				$plugin = (array)$plugin;
				$temp[$plugin['key']] = $plugin['value'];
			}
			file_put_contents(ROOT_SITE.'cache/plugins.cache.php', '<?php'."\n".'$plugins = '.var_export($temp, true).";\n".'?>');
			return $temp;
		}
	}
}
ojnApi::getListOfConnectedBunnies();
ojnApi::getListOfPlugins();
ojnApi::getListOfActivePlugins();
?>
