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
if(isset($_SESSION['connected']) && !strpos($_SERVER['REQUEST_URI'],"logout")) {
    $logout = 1;
    if(isset($_SESSION['token'])) {
        $r = $ojnAPI->getApiString('accounts/settoken?tk='.$_SESSION['token'].$ojnAPI->getToken());
        $logout = !isset($retour['ok']);
    }    
    if(!$logout)
        header('Location: index.php?logout');
}
?>
