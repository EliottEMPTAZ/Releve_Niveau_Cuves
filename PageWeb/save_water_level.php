<?php
date_default_timezone_set('Europe/Paris');
$databasePath = __DIR__ . '/niveau_eau.db';
$db = new PDO("sqlite:$databasePath");

// Récupère les données envoyées par l'Arduino
$niveau_cuve_1 = $_POST['niveau_cuve_1'] ?? null;
$niveau_cuve_2 = $_POST['niveau_cuve_2'] ?? null;
$timestamp = date('Y-m-d H:i:s');

// Vérifie que les données nécessaires sont présentes
if ($niveau_cuve_1 !== null && $niveau_cuve_2 !== null) {
    // Prépare la requête SQL pour insérer les données
    $stmt = $db->prepare("INSERT INTO NiveauxEau (timestamp, niveau_cuve_1, niveau_cuve_2) VALUES (:timestamp, :niveau_cuve_1, :niveau_cuve_2)");
    $stmt->bindParam(':timestamp', $timestamp);
    $stmt->bindParam(':niveau_cuve_1', $niveau_cuve_1);
    $stmt->bindParam(':niveau_cuve_2', $niveau_cuve_2);
    
    // Exécute la requête
    $stmt->execute();
    echo "Données insérées dans la base de données avec succès pour les deux cuves.";
} else {
    echo "Données manquantes pour l'insertion.";
}
?>