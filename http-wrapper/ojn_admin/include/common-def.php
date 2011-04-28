<?php
define('ROOT_SITE', realpath(dirname(__FILE__)."/../")."/");
define('ROOT_WWW_ADMIN', 'http://<HOSTNAME>/ojn_admin/');
define('ROOT_WWW_API', 'http://<HOSTNAME>/ojn_api/');

session_start('openJabNab');
require_once(ROOT_SITE.'class/api.class.php');
require_once(ROOT_SITE.'class/template.class.php');
$ojnAPI = new ojnApi();
$ojnTemplate = new ojnTemplate(&$ojnAPI);
ob_start(array($ojnTemplate,'display'));
?>
