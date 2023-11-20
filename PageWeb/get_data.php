<?php
date_default_timezone_set('Europe/Paris');
header('Content-Type: application/json');

$databasePath = __DIR__ . '/niveau_eau.db';
$db = new PDO("sqlite:$databasePath");

// Récupérer et valider les paramètres de date de la requête GET
$startDate = isset($_GET['start']) ? $_GET['start'] : null;
$endDate = isset($_GET['end']) ? $_GET['end'] : null;

// Préparation de la requête pour obtenir tous les enregistrements dans la plage de dates
$query = "SELECT * FROM NiveauxEau";
$parameters = [];

if ($startDate || $endDate) {
    $conditions = [];
    if ($startDate) {
        $conditions[] = "timestamp >= :startDate";
        $parameters[':startDate'] = $startDate;
    }
    if ($endDate) {
        $conditions[] = "timestamp <= :endDate";
        $parameters[':endDate'] = $endDate;
    }
    $query .= " WHERE " . implode(' AND ', $conditions);
}

$query .= " ORDER BY timestamp ASC";//" WHERE timestamp > '2023-11-20 00:00:00' ORDER BY timestamp ASC"
$stmtAllEntries = $db->prepare($query);
$stmtAllEntries->execute($parameters);
$allEntries = $stmtAllEntries->fetchAll(PDO::FETCH_ASSOC);

// Préparation de la requête pour obtenir le dernier enregistrement
$stmtLastEntry = $db->query("SELECT * FROM NiveauxEau ORDER BY id DESC LIMIT 1");
$lastEntry = $stmtLastEntry->fetch(PDO::FETCH_ASSOC);

$response = [
    'lastEntry' => false,
    'allEntries' => [],
    'startDate' => "$startDate",
    'endDate' => "$endDate"
];

// Si des données sont trouvées pour le dernier enregistrement, les ajouter à la réponse
if ($lastEntry) {
    $lastEntry['timestamp'] = (new DateTime($lastEntry['timestamp']))->format('d/m/Y H:i:s');
    $response['lastEntry'] = $lastEntry;
}

// Ajouter toutes les entrées filtrées par date à la réponse
foreach ($allEntries as $key => $entry) {
    $allEntries[$key]['timestamp'] = (new DateTime($entry['timestamp']))->format('d/m/Y H:i:s');
}
$response['allEntries'] = $allEntries;

echo json_encode($response);
?>