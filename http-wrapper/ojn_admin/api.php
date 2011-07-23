<?php
require_once "include/common.php";
if(!isset($_SESSION['token']) || !$Infos['isAdmin'])
	header('Location: index.php');
?>
<h1>DEVELOPMENT Feature. On this page, you can make a direct API call, your token will be added automatically</h1>
<br />
<form method="post">
<input type="text" style="width:80%" name="r" value="<?php echo !empty($_POST['r']) ? $_POST['r'] : '' ?>"/>
<input type="submit" value="Go !" />
</form>
<pre style="border: 1px solid grey ; width:80% background-color:grey">
<?php

function printr($a,$l=0) {
	if(is_array($a))
		foreach($a as $b=>$sa) {
			if(count($sa) < 2) {
				if(!is_array($sa))
					echo str_repeat('    ',$l).$b." => ".$sa."\n";
				else if(!is_numeric($b))
					echo str_repeat('    ',$l).$b."\n";
			}else
				echo str_repeat('    ',$l).$b."\n";
				printr($sa,$l+1);
		}
}

if(!empty($_POST['r'])) {
	$r = $ojnAPI->getApiXMLArray($_POST['r'].(strstr($_POST['r'],'?') ? '&': '?').$ojnAPI->getToken());
	printr($r);
} else
	echo "Make your request.";
?></pre>
<?php
require_once "include/append.php"
?>
