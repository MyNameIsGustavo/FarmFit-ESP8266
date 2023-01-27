<?php

$dbname = '#########';
$dbuser = '######';  
$dbpass = '#############'; 
$dbhost = '##########################'; 

$connect = @mysqli_connect($dbhost,$dbuser,$dbpass,$dbname);

if(!$connect){
	echo "Error: " . mysqli_connect_error();
	exit();
}

echo "Connection Success!<br><br>";

$umidade_solo1 = $_GET['umidade_solo1'];
$umidade_solo2 = $_GET['umidade_solo2'];
$fluxo_agua = $_GET['fluxo_agua'];
$permissao_bomba = $_GET['permissao_bomba'];


$query = "INSERT INTO farm_dates (umidade_solo1, umidade_solo2, fluxo_agua, permissao_bomba) VALUES ('$umidade_solo1', '$umidade_solo2', '$fluxo_agua', '$permissao_bomba')";
$result = mysqli_query($connect,$query);

echo "Insertion Success!<br>";

?>
