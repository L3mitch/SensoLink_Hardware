#include <WiFi.h>
#include <WebServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <time.h>

#define MAX_LEITURAS 50

int totalLeituras = 0;

unsigned long ultimaLeitura = 0;
const unsigned long intervaloLeitura = 10000;

struct Leitura
{
  float temperatura;
  float umidade;
  time_t timestamp;
};

Leitura historico[MAX_LEITURAS];

float temperatura = 25.1;
float umidade = 60.2;
String statusCortina = "parada";

WebServer server(80);

String formatarHora(time_t t)
{
  struct tm *info = localtime(&t);
  char buffer[25];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", info);
  return String(buffer);
}

void adicionarLeitura(float temp, float umi)
{
  if (totalLeituras < MAX_LEITURAS)
  {
    historico[totalLeituras].temperatura = temp;
    historico[totalLeituras].umidade = umi;
    historico[totalLeituras].timestamp = time(nullptr);
    totalLeituras++;
  }
  else
  {
    Serial.println("Histórico cheio, descartando leitura.");
  }
}

void enviarHistorico()
{
  DynamicJsonDocument doc(4096); // Ajuste se precisar

  JsonArray array = doc.createNestedArray("leituras");

  for (int i = 0; i < totalLeituras; i++)
  {
    JsonObject obj = array.createNestedObject();
    obj["temperatura"] = historico[i].temperatura;
    obj["umidade"] = historico[i].umidade;
    obj["hora"] = formatarHora(historico[i].timestamp);
  }

  String json;
  serializeJson(doc, json);
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", json);
}

void configurarWebServer()
{
  server.on("/historico", enviarHistorico);

  server.begin();
  Serial.println("Servidor HTTP iniciado");
}

void setup()
{
  Serial.begin(115200);

  configTime(-3 * 3600, 0, "pool.ntp.org", "time.nist.gov"); // UTC-3
  Serial.println("Sincronizando hora...");
  delay(1000);

  WiFiManager wm;

  // Opcional: resetar config segurando botão físico
  // wm.resetSettings();

  // Se não conseguir conectar, inicia o portal
  if (!wm.autoConnect("Cortina-Setup"))
  {
    Serial.println("Falha ao conectar. Reiniciando...");
    ESP.restart();
  }

  Serial.println("Conectado ao Wi-Fi!");
  Serial.println("IP: " + WiFi.localIP().toString());

  configurarWebServer();
}

void loop()
{
  server.handleClient();

  if (millis() - ultimaLeitura > intervaloLeitura)
  {
    ultimaLeitura = millis();

    // Aqui seria a leitura real do sensor
    float temp = 25.1 + random(-10, 10) * 0.1;
    float umi = 60.2 + random(-10, 10) * 0.1;

    adicionarLeitura(temp, umi);
  }
}
