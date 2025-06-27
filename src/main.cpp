#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <DHTesp.h>
#include <time.h>


#define DHT_PIN 21

DHTesp dht;

// ==== CONFIGURAÇÕES DO USUÁRIO ====
const char *ssid = "Teste HTTP";
const char *password = "123teste";

const char *FIREBASE_HOST = "sensolink-bed71";
const char *SENSOR_ID = "3w1n9oYT74HPBgiCbCWb";
const char *FIREBASE_API_KEY = "AIzaSyDxnHt3H5eT2cAmQah7ztVfRyeDGqFUgNo";
// ==================================

// Constantes para construir URL Firebase
String firebaseHost = "https://firestore.googleapis.com/v1/projects/" + String(FIREBASE_HOST) + "/databases/(default)/documents/sensors/" + String(SENSOR_ID) + "/readings?key=" + FIREBASE_API_KEY;

// Protótipos de funções
String getTimestampString();
void sendToFirestore(float temperatura, String timestampISO);

void setup()
{
   dht.setup(DHT_PIN, DHTesp::DHT22);

   Serial.begin(115200);
   delay(1000);

   // Conecta ao Wi-Fi
   WiFi.begin(ssid, password);
   Serial.print("Conectando ao Wi-Fi");
   while (WiFi.status() != WL_CONNECTED)
   {
      delay(500);
      Serial.print(".");
   }
   Serial.println("\nWi-Fi conectado!");

   // Configura NTP (UTC)
   configTime(0, 0, "pool.ntp.org", "time.nist.gov");

   // Aguarda sincronização
   struct tm timeinfo;
   Serial.print("Sincronizando horário");
   while (!getLocalTime(&timeinfo))
   {
      Serial.print(".");
      delay(1000);
   }
   Serial.println("\nHorário sincronizado com NTP!");
}

void loop()
{
   TempAndHumidity dados = dht.getTempAndHumidity();
   float temperatura = dados.temperature;

   String timestampISO = getTimestampString();

   Serial.println("Temperatura: " + String(temperatura));
   Serial.println("Timestamp: " + timestampISO);

   sendToFirestore(temperatura, timestampISO);

   delay(1000); // Espera 1 segundo
}

String getTimestampString()
{
   struct tm timeinfo;
   if (!getLocalTime(&timeinfo))
   {
      return "";
   }

   char iso8601[25];
   strftime(iso8601, sizeof(iso8601), "%Y-%m-%dT%H:%M:%SZ", &timeinfo);
   return String(iso8601);
}

void sendToFirestore(float temperatura, String timestampISO)
{
   HTTPClient http;
   http.begin(firebaseHost);
   http.addHeader("Content-Type", "application/json");

   // Monta JSON
   StaticJsonDocument<256> doc;
   doc["fields"]["temperatura"]["doubleValue"] = temperatura;
   doc["fields"]["timestamp"]["timestampValue"] = timestampISO;

   String json;
   serializeJson(doc, json);

   // Envia
   int responseCode = http.POST(json);
   Serial.println("Código HTTP: " + String(responseCode));
   Serial.println("Resposta: " + http.getString());

   http.end();
}