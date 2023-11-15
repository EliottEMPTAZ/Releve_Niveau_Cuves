// Fonction pour charger les données à partir du serveur et mettre à jour l'affichage
function loadData(startDate, endDate) {
    let url = 'get_data.php';
    if (startDate && endDate) {
        url += `?start=${startDate}&end=${endDate}`;
    }

    fetch(url)
        .then(response => response.json())
        .then(data => {
            // Vérifier et mettre à jour les informations du dernier enregistrement
            updateLastEntryData(data.lastEntry);
            // Dessiner les graphiques
            if (data.allEntries && data.allEntries.length > 0) {
                drawChart('chart', data.allEntries);
            } else {
                document.getElementById('data').innerHTML = '<p>Aucune donnée disponible pour la période sélectionnée.</p>';
            }
        })
        .catch(error => console.error('Erreur lors de la récupération des données:', error));
}

// Mettre à jour les informations de la dernière entrée
function updateLastEntryData(lastEntry) {
    // Vérifiez que lastEntry est un objet et contient les propriétés attendues
    if (lastEntry && 'timestamp' in lastEntry && 'niveau_cuve_1' in lastEntry && 'niveau_cuve_2' in lastEntry) {
        document.getElementById('date').textContent = lastEntry.timestamp; // Mise à jour de la date du dernier relevé
        document.getElementById('niveau1').textContent = lastEntry.niveau_cuve_1 + ' %';
        document.getElementById('niveau2').textContent = lastEntry.niveau_cuve_2 + ' %';
    } else {
        document.getElementById('data').innerHTML = '<p>Aucune donnée disponible pour le dernier enregistrement.</p>';
    }
}


// Fonction pour dessiner un graphique avec Chart.js
function drawChart(canvasId, data) {
    const canvas = document.getElementById(canvasId);
    if (!canvas) {
        console.error(`Canvas with id ${canvasId} not found.`);
        return;
    }
    const ctx = canvas.getContext('2d');

    const labels = data.map(entry => entry.timestamp);
    const dataCuve1 = data.map(entry => parseFloat(entry.niveau_cuve_1));
    const dataCuve2 = data.map(entry => parseFloat(entry.niveau_cuve_2));
    const dataAverage = data.map((entry, index) => (dataCuve1[index] + dataCuve2[index]) / 2);

    // Destruction de l'ancien graphique s'il existe
    if (window.myChart) {
        window.myChart.destroy();
    }

    // Création du nouveau graphique
    window.myChart = new Chart(ctx, {
        type: 'line',
        data: {
            labels: labels,
            datasets: [
                {
                    label: 'Niveau Cuve 1',
                    data: dataCuve1,
                    fill: false,
                    borderColor: 'rgb(255, 205, 86)',
                    tension: 0.1
                },
                {
                    label: 'Niveau Cuve 2',
                    data: dataCuve2,
                    fill: false,
                    borderColor: 'rgb(75, 192, 192)',
                    tension: 0.1
                },
                {
                    label: 'Moyenne des deux cuves',
                    data: dataAverage,
                    fill: false,
                    borderColor: 'rgb(153, 102, 255)',
                    tension: 0.1
                }
            ]
        },
        options: {
            scales: {
                y: {
                    beginAtZero: true
                }
            },
            responsive: true,
            maintainAspectRatio: true
        }
    });
}

// Fonction appelée par le bouton pour mettre à jour le graphique
function updateChart() {
    const startDate = document.getElementById('startDate').value;
    const endDate = document.getElementById('endDate').value;
    loadData(startDate, endDate);
}

// Ajout des écouteurs d'événements pour le chargement de la page
document.addEventListener('DOMContentLoaded', (event) => {
    const currentDate = new Date();
    const threeMonthsAgo = new Date();
    threeMonthsAgo.setMonth(currentDate.getMonth() - 3);

    const formatDate = (date) => date.toISOString().split('T')[0];
    document.getElementById('startDate').value = formatDate(threeMonthsAgo);
    document.getElementById('endDate').value = formatDate(currentDate);

    // Assurez-vous que l'élément 'niveau1' existe avant de lancer cette fonction
    if (document.getElementById('niveau1') && document.getElementById('niveau2')) {
        loadData(formatDate(threeMonthsAgo), formatDate(currentDate));
    }
});
