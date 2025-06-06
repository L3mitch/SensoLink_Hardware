#include <WiFi.h>
#include <WebServer.h>

// Configurações de rede
const char* ssid = "Kauan 42";
const char* password = "kau15092003";

// GPIOs usados para o motor da cortina
#define PIN_ABRIR  26
#define PIN_FECHAR 27

String statusCortina = "parada";

WebServer server(80);

void handleRoot() {
  String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <title>Cortina</title>
      <meta name="viewport" content="width=device-width, initial-scale=1">
    </head>
    <body style="text-align: center; font-family: sans-serif;">
      <h1>Controle da Cortina</h1>
      <p>Status: <b>)rawliteral" + statusCortina + R"rawliteral(</b></p>
      <button onclick="location.href='/abrir'" style="padding: 10px 20px;">Abrir</button>
      <button onclick="location.href='/fechar'" style="padding: 10px 20px;">Fechar</button>
    </body>
    </html>
  )rawliteral";

  server.send(200, "text/html", html);
}

void handleAbrir() {
  // digitalWrite(PIN_FECHAR, LOW);
  // digitalWrite(PIN_ABRIR, HIGH);
  statusCortina = "abrindo";
}

void handleFechar() {
  // digitalWrite(PIN_ABRIR, LOW);
  // digitalWrite(PIN_FECHAR, HIGH);
  statusCortina = "fechando";
}

void handleStatus() {
  server.send(200, "text/plain", "Status atual: " + statusCortina);
}

void setup() {
  Serial.begin(115200);

  pinMode(PIN_ABRIR, OUTPUT);
  pinMode(PIN_FECHAR, OUTPUT);
  digitalWrite(PIN_ABRIR, LOW);
  digitalWrite(PIN_FECHAR, LOW);

  WiFi.begin(ssid, password);
  Serial.print("Conectando-se ao Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado. IP: " + WiFi.localIP().toString());

  server.on("/", handleRoot);
  server.on("/abrir", handleAbrir);
  server.on("/fechar", handleFechar);
  server.on("/status", handleStatus);

  server.begin();
  Serial.println("Servidor iniciado");
}

void loop() {
  server.handleClient();
}
