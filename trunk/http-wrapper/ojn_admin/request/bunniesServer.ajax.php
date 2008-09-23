<center>
	<table style="width: 80%">
		<tr>
			<th>Nom</th>
			<th>Adresse MAC</th>
		</tr>
<?
	$i = 0;
	foreach(ojnapi::getListOfConnectedBunnies() as $mac => $bunny)
	{
		$mac_hidden = strtoupper($mac);
		$mac_hidden = substr($mac_hidden, 0, 6)."****".substr($mac_hidden, 10, 2);
?>
		<tr<?=$i++ % 2 == 1 ? " class='l2'" : "" ?>>
			<td><?=$bunny ?></td>
			<td><?=$mac_hidden ?></td>
		</tr>
<?
	}
?>
	</table>
