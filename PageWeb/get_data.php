<?php
date_default_timezone_set('Europe/Paris');
header('Content-Type: application/json');

// Chemin vers la base de données SQLite
$databasePath = __DIR__ . '/niveau_eau.db';

// Connexion à la base de données SQLite avec PDO
$db = new PDO("sqlite:$databasePath");

// Récupérer et valider les paramètres de date de la requête GET
$startDate = isset($_GET['start']) ? new DateTime($_GET['start']) : null;
$endDate = isset($_GET['end']) ? new DateTime($_GET['end']) : null;

// Préparation de la requête pour obtenir le dernier enregistrement
$stmtLastEntry = $db->query("SELECT * FROM NiveauxEau ORDER BY id DESC LIMIT 1");
$lastEntry = $stmtLastEntry->fetch(PDO::FETCH_ASSOC);

// Préparation de la requête pour obtenir tous les enregistrements dans la plage de dates
$query = "SELECT * FROM NiveauxEau";
$queryConditions = [];
$parameters = [];

if ($startDate) {
    $queryConditions[] = "timestamp >= :startDate";
    $parameters[':startDate'] = $startDate->format('Y-m-d H:i:s');
}

if ($endDate) {
    $queryConditions[] = "timestamp <= :endDate";
    $parameters[':endDate'] = $endDate->format('Y-m-d H:i:s');
}

if (count($queryConditions) > 0) {
    $query .= " WHERE " . implode(' AND ', $queryConditions);
}

$query .= " ORDER BY timestamp ASC";
$stmtAllEntries = $db->prepare($query);
$stmtAllEntries->execute($parameters);
$allEntries = $stmtAllEntries->fetchAll(PDO::FETCH_ASSOC);

// Structure de la réponse
$response = [
    'lastEntry' => false,
    'allEntries' => []
];

// Si des données sont trouvées pour le dernier enregistrement, les formater et les ajouter à la réponse
if ($lastEntry) {
    $lastEntry['timestamp'] = (new DateTime($lastEntry['timestamp']))->format('d/m/Y H:i:s');
    $response['lastEntry'] = $lastEntry;
}

// Formater et ajouter toutes les entrées filtrées par date à la réponse
foreach ($allEntries as $key => $entry) {
    $allEntries[$key]['timestamp'] = (new DateTime($entry['timestamp']))->format('d/m/Y H:i:s');
}
$response['allEntries'] = $allEntries;

// Encodage des données en JSON et envoi de la réponse
echo json_encode($response);
?>