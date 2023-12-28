#include <WiFi.h>
#include <WiFiClient.h>
#include <HCSR04.h>

// Remplacer par vos SSID et mot de passe WiFi
const char* ssid     = "EMPTAZ #1";
const char* password = "familleemptaz";

// Votre serveur et chemin
const char* host = "172.20.10.3"; // Supprimez "http://" pour WiFiClient
const int httpPort = 8888;

// Définition des broches pour les deux capteurs ultrasoniques
#define TRIGGER_PIN1  32  // Broche Trigger pour le capteur 1
#define ECHO_PIN1     33  // Broche Echo pour le capteur 1
#define TRIGGER_PIN2  25  // Broche Trigger pour le capteur 2
#define ECHO_PIN2     26  // Broche Echo pour le capteur 2

//Définition du pin de la led
#define LED_PIN 14

//Definition des parametres de mesure
#define MAX_CUVE  24 // heuteur de la cuve pleine
#define ESPACE  7.5 // Distance entre le capteur et le niveau max de l'eau

// Création de deux objets pour les deux capteurs
UltraSonicDistanceSensor sonar1(TRIGGER_PIN1, ECHO_PIN1);
UltraSonicDistanceSensor sonar2(TRIGGER_PIN2, ECHO_PIN2);

// Création d'un client WiFi pour gérer la connexion serveur web.
WiFiClient client;

void setup(){
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);

  digitalWrite(LED_PIN, HIGH);//Avant connection Led allumer

  connectionWifi();
}

void loop() {

  //Verification de la connection
  if(WiFi.status() == WL_CONNECTED){
    digitalWrite(LED_PIN, LOW);
  } else {
    digitalWrite(LED_PIN, HIGH);
    connectionWifi(); //tenter la reconnection
  }

  delay(4500);

  // Mesure pour la cuve 1
  float distance1 = sonar1.measureDistanceCm();
  float niveau_cuve_1 = (1 - ((distance1 - ESPACE) / MAX_CUVE)) * 100;
  niveau_cuve_1 = constrain(niveau_cuve_1, 0, 100); // S'assure que le pourcentage reste entre 0 et 100.

  delay(100);
  // Mesure pour la cuve 2
  float distance2 = sonar2.measureDistanceCm();
  float niveau_cuve_2 = (1 - ((distance2 - ESPACE) / MAX_CUVE)) * 100;
  niveau_cuve_2 = constrain(niveau_cuve_2, 0, 100); // S'assure que le pourcentage reste entre 0 et 100.

  //Affichage
  affichage_serial_niveau(distance1, niveau_cuve_1, distance2, niveau_cuve_2);

  if (client.connect(host, httpPort)) { // Connecte le client au serveur
    // Crée la chaîne de requête POST
    String url = "http://172.20.10.3:8888/save_water_level.php";
    String postData = "niveau_cuve_1=" + String(niveau_cuve_1) + "&niveau_cuve_2=" + String(niveau_cuve_2);

    // Envoie la requête POST au serveur
    client.println("POST " + url + " HTTP/1.1");
    client.println("Host: " + String(host));
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Content-Length: " + String(postData.length()));
    client.println(); // Ligne vide avant le corps de la requête
    client.println(postData);
    //Serial.println(postData); //Afficher la requête
    
    // Attendre un moment pour que la réponse commence à arriver
    delay(500); 

    // Lire tous les en-têtes. Les en-têtes se terminent par une ligne vide.
    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        break;
      }
    }
    // Vérifiez s'il y a des données disponibles à lire
    while (client.available()) {
      String line = client.readStringUntil('\n'); // Lire chaque ligne de la réponse
      Serial.println(line); // Afficher la reponse du serveur
    }
  }
}

void connectionWifi(){
  Serial.print("[WiFi] Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  // Will try for about 10 seconds (20x 500ms)
  int tryDelay = 500;
  int numberOfTries = 20;

  // Wait for the WiFi event
  while (true) {
    switch(WiFi.status()) {
      case WL_NO_SSID_AVAIL:
        Serial.println("[WiFi] SSID not found");
        break;
      case WL_CONNECT_FAILED:
        Serial.print("[WiFi] Failed - WiFi not connected! Reason: ");
        return;
        break;
      case WL_CONNECTION_LOST:
        Serial.println("[WiFi] Connection was lost");
        break;
      case WL_SCAN_COMPLETED:
        Serial.println("[WiFi] Scan is completed");
        break;
      case WL_DISCONNECTED:
        Serial.println("[WiFi] WiFi is disconnected");
        break;
      case WL_CONNECTED:
        Serial.println("[WiFi] WiFi is connected!");
        Serial.print("[WiFi] IP address: ");
        Serial.println(WiFi.localIP());
        return;
        break;
      default:
        Serial.print("[WiFi] WiFi Status: ");
        Serial.println(WiFi.status());
        break;
    }
    delay(tryDelay);
    
    if (WiFi.status() == WL_CONNECTED) { //if Wifi connected
      Serial.println("[WiFi] WiFi is connected!");
      Serial.print("[WiFi] IP address: ");
      Serial.println(WiFi.localIP());
      for(int i=0; i<5; i++){
        digitalWrite(LED_PIN, LOW);
        delay(200);
        digitalWrite(LED_PIN, HIGH);
        delay(200);
        digitalWrite(LED_PIN, LOW);
      }
      break;
    } 
    else {
        digitalWrite(LED_PIN, HIGH);
    }

    if(numberOfTries <= 0){
      Serial.print("[WiFi] Failed to connect to WiFi!");
      // Use disconnect function to force stop trying to connect
      WiFi.disconnect();
      break;
    } else {
      numberOfTries--;
    }
  }
}

void affichage_serial_niveau(float d1,float n1,float d2,float n2){
  Serial.println("------------------");
  Serial.print("distance 1 : ");
  Serial.println(d1);
  Serial.print("distance 2 : ");
  Serial.println(d2);
  Serial.println("------------------");
  Serial.print("Niveau cuve 1 : ");
  Serial.print(n1);
  Serial.println(" %");
  Serial.print("Niveau cuve 2 : ");
  Serial.print(n2);
  Serial.println(" %");
  Serial.println("------------------");
}