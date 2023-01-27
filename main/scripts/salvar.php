<?php

    include('conexao.php');

    $umidade_solo1 = $_GET['umidade_solo1'];
    $umidade_solo2 = $_GET['umidade_solo2'];
    $fluxo_agua = $_GET['fluxo_agua'];
    $permissao_bomba = $_GET['permissao_bomba'];

    $sql = "INSERT INTO farm_dates (umidade_solo1, umidade_solo2, fluxo_agua, permissao_bomba) 
    VALUES (:umidade_solo1, :umidade_solo2, :fluxo_agua, :permissao_bomba)";

    $stmt = $PDO->prepare($sql);

    $stmt->bindParam(':umidade_solo1', $umidade_solo1);
    $stmt->bindParam(':umidade_solo2', $umidade_solo2);
    $stmt->bindParam(':fluxo_agua', $fluxo_agua);
    $stmt->bindParam(':permissao_bomba', $permissao_bomba);

    if($stmt->execute()){
        echo"Salvo_com_Sucesso";
    }else{
        echo"Erro_no_Salvamento";
    }
?>