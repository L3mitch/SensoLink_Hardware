#include <WiFi.h>
#include <WebServer.h>

// Dados fictícios para simular sensores
float temperatura = 26.4;
float umidade = 58.2;

const char* ssid = "Kauan 42";
const char* password = "kau15092003";

WebServer server(80);

void handleJSON() {
  String json = "{";
  json += "\"status\":\"ok\",";
  json += "\"temperatura\":" + String(temperatura, 1) + ",";
  json += "\"umidade\":" + String(umidade, 1);
  json += "}";

  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado. IP: " + WiFi.localIP().toString());

  server.on("/dados", handleJSON);

  server.begin();
  Serial.println("Servidor HTTP iniciado");
}

void loop() {
  server.handleClient();
}
