#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

// ==================== CONFIGURATION ====================
// Pour la SIMULATION WOKWI uniquement : réseau virtuel fourni par Wokwi
const char* ssid = "Wokwi-GUEST";
const char* password = "";              // Le réseau Wokwi-GUEST n'a pas de mot de passe

const int LED_PIN = 4;                  // Broche de la LED

WebServer server(80);                   // Serveur web sur le port 80

// ==================== PAGE HTML ====================
String getHTML(bool ledState) {
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>";
  html += "body { font-family: Arial; text-align: center; margin-top: 50px; background-color: #f0f0f0; }";
  html += "h1 { color: #333; }";
  html += ".button { display: inline-block; padding: 20px 40px; margin: 10px; font-size: 18px; ";
  html += "cursor: pointer; border: none; border-radius: 10px; color: white; }";
  html += ".on { background-color: #4CAF50; }";
  html += ".on:hover { background-color: #45a049; }";
  html += ".off { background-color: #f44336; }";
  html += ".off:hover { background-color: #da190b; }";
  html += ".status { margin-top: 20px; font-size: 20px; font-weight: bold; }";
  html += ".led-on { color: #4CAF50; }";
  html += ".led-off { color: #f44336; }";
  html += "</style></head><body>";

  html += "<h1>🔌 Contrôle LED ESP32</h1>";
  html += "<form action='/' method='GET'>";
  html += "<button type='submit' name='led' value='on' class='button on'>ALLUMER 💡</button>";
  html += "<button type='submit' name='led' value='off' class='button off'>ÉTEINDRE ⚫</button>";
  html += "</form>";

  if (ledState) {
    html += "<div class='status led-on'>LED est ALLUMÉE ✅</div>";
  } else {
    html += "<div class='status led-off'>LED est ÉTEINTE ❌</div>";
  }

  html += "</body></html>";
  return html;
}

// ==================== GESTIONNAIRES DE ROUTES ====================
void handleRoot() {
  server.sendHeader("Content-Type", "text/html; charset=UTF-8");
  String ledParam = server.arg("led");

  if (ledParam == "on") {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("LED ALLUMÉE");
  } else if (ledParam == "off") {
    digitalWrite(LED_PIN, LOW);
    Serial.println("LED ÉTEINTE");
  }

  bool ledState = digitalRead(LED_PIN);
  server.send(200, "text/html", getHTML(ledState));
}

void handleNotFound() {
  server.send(404, "text/plain", "404: Not found");
}

// ==================== SETUP ====================
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n\n=== Démarrage ESP32 (Wokwi) ===");

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  Serial.println("LED configurée sur GPIO" + String(LED_PIN));

  // Connexion au réseau WiFi virtuel Wokwi
  Serial.println("Connexion au WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnecté !");
  Serial.print("Adresse IP : ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("Serveur HTTP démarré sur le port 80");
  Serial.println("==============================\n");
}

// ==================== LOOP ====================
void loop() {
  server.handleClient();
  delay(2);
}