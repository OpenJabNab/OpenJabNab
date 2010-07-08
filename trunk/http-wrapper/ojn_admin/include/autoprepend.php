<?
define('ROOT_SITE', realpath(dirname(__FILE__)."/../")."/");
define('ROOT_WWW_ADMIN', 'http://ojn.soete.org/ojn_admin/');
define('ROOT_WWW_API', 'http://ojn.soete.org/ojn_api/');

session_start('openJabNab');
require_once(ROOT_SITE.'class/api.class.php');
if(!preg_match("|\.ajax\.php|", $_SERVER['REQUEST_URI']))
{
	require_once(ROOT_SITE.'class/template.class.php');
	$ojnTemplate = new ojnTemplate();
}
?>
