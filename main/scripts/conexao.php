<?php

    try {

        $conn = new PDO('###############################', '###############################' , '###############################');
        $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
        echo "CONEXÃO BEM SUCEDIDA!";

    } catch (PDOException $erro){

        echo "Erro de Conexão, Detalhes: " . $erro->getMessage();

    }
?>

