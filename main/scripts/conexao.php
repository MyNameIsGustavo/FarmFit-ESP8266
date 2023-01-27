<?php

    try {
        $HOST = "#######";
        $BANCO = "#######";
        $USUARIO = "#######";
        $SENHA = "#######";

        $PDO = new PDO("mysql:host=" . $HOST . ";dbname=" . $BANCO . ";charset=utf8", $USUARIO, $SENHA);

    } catch (PDOException $erro){

        echo "Erro de Conexão, Detalhes: " . $erro->getMessage();

    }
?>
