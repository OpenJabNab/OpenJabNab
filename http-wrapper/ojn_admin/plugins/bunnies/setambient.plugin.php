<?php
if(!empty($_POST)) {
	if(isset($_POST['weathersel'])) {
		$s = 1;
		$v = (int)$_POST['weathersel'];
	}
	if(isset($_POST['stocksel'])) {
		$s = 2;
		$v = (int)$_POST['stocksel'];
	}
	if(isset($_POST['periphsel'])) {
		$s = 3;
		$v = (int)$_POST['periphsel'];
	}
	if(isset($_POST['leftearsel'])) {
		$s = 4;
		$v = (int)$_POST['leftearsel'];
	}
	if(isset($_POST['rightearsel'])) {
		$s = 5;
		$v = (int)$_POST['rightearsel'];
	}
	if(isset($_POST['emailsel'])) {
		$s = 6;
		$v = (int)$_POST['emailsel'];
	}
	if(isset($_POST['airsel'])) {
		$s = 7;
		$v = (int)$_POST['airsel'];
	}
	if(isset($_POST['nosesel'])) {
		$s = 8;
		$v = (int)$_POST['nosesel'];
	}
	if(isset($_POST['dissel'])) {
		$s = 0;
		$v = (int)$_POST['dissel'];
	}
	if(isset($s) && isset($v)) {
		$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/setambient/ambientPacket?service=".$s."&value=".$v."&".$ojnAPI->getToken());
		$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
		header("Location: bunny_plugin.php?p=setambient");
	}
}
?>


<fieldset>
<legend>Ambient Packets</legend>
<form method="post">
Weather:
<select name="weathersel">
	<option value="0">Sun</option>
	<option value="1">Cloudy</option>
	<option value="2">Smog</option>
	<option value="3">Rain</option>
	<option value="4">Snow</option>
	<option value="5">Storm</option>
</select>
<input type="submit" value="Send" />
</form>
<form method="post">
Stock Market:
<select name="stocksel">
	<option value="0">HighDown</option>
	<option value="1">MediumDown</option>
	<option value="2">LittleDown</option>
	<option value="3">Stable</option>
	<option value="4">LittleUp</option>
	<option value="5">MediumUp</option>
	<option value="6">HighUp</option>
</select>
<input type="submit" value="Send" />
</form>

</select>
</form>
<form method="post">
Periph:
<select name="periphsel">
	<option value="0">VeryLow</option>
	<option value="1">Low</option>
	<option value="2">LowAverage</option>
	<option value="3">Average</option>
	<option value="4">FastAverage</option>
	<option value="5">Fast</option>
	<option value="6">VeryFast</option>
</select>
<input type="submit" value="Send" />
</form>

<form method="post">
Left Ear:
<select name="leftearsel">
<?php for($i=0;$i<16;$i++): ?>
	<option value="<?php echo $i; ?>"><?php echo $i; ?></option>
<?php endfor; ?>
</select>
<input type="submit" value="Send" />
</form>

<form method="post">
Right Ear:
<select name="rightearsel">
<?php for($i=0;$i<16;$i++): ?>
	<option value="<?php echo $i; ?>"><?php echo $i; ?></option>
<?php endfor; ?>
</select>
<input type="submit" value="Send" />
</form>


<form method="post">
Email:
<select name="emailsel">
	<option value="0">Zero</option>
	<option value="1">One</option>
	<option value="2">Two</option>
	<option value="3">Three (or more)</option>
</select>
<input type="submit" value="Send" />
</form>

<form method="post">
AirQuality:
<select name="airsel">
	<option value="0">Good</option>
	<option value="5">Medium</option>
	<option value="10">Bad</option>
</select>
<input type="submit" value="Send" />
</form>

<form method="post">
Nose (Messages):
<select name="nosesel">
	<option value="0">None</option>
	<option value="1">Blink</option>
	<option value="2">DoubleBlink</option>
</select>
<input type="submit" value="Send" />
</form>

<form method="post">
Disable
<select name="dissel">
	<option value="1">Weather</option>
	<option value="2">StockMarket</option>
	<option value="3">Periph</option>
	<option value="6">Email</option>
	<option value="7">AirQuality</option>
</select>
<input type="submit" value="Send" />
</form>
</fieldset>
