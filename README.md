# 📦 Projeto ESP32 – API REST para Histórico de Temperatura e Umidade (Versão do dia 06/06/2025)

Este projeto implementa um servidor HTTP local em um ESP32, com conexão Wi-Fi dinâmica via **WiFiManager**, que disponibiliza um endpoint `/historico` para leitura de dados de temperatura e umidade em formato JSON. As leituras são simuladas e armazenadas localmente em memória RAM.

---

## 📁 Estrutura do Projeto

- 📶 Conexão Wi-Fi dinâmica (modo Access Point para configuração inicial)
- 🧠 Armazenamento em RAM de até 50 leituras com horário
- 🌐 Servidor HTTP com endpoint `/historico`
- 🧪 Simulação de sensor de temperatura/umidade
- ⏱️ Coleta automática a cada 10 segundos
- 📤 Retorno das leituras no formato JSON

---

## 🛠️ Bibliotecas Utilizadas

| Biblioteca       | Função                                       |
|------------------|-----------------------------------------------|
| `WiFi.h`         | Conexão Wi-Fi                                |
| `WiFiManager.h`  | Portal de configuração Wi-Fi local            |
| `WebServer.h`    | Servidor HTTP embutido                        |
| `ArduinoJson.h`  | Serialização dos dados no formato JSON        |
| `time.h`         | Sincronização de horário via NTP              |

---

## 📡 Conexão Wi-Fi

A biblioteca **WiFiManager** é utilizada para configurar dinamicamente a rede Wi-Fi do ESP32.

- Se o dispositivo não conseguir conectar-se a uma rede previamente salva, ele cria um Access Point chamado `Cortina-Setup`.
- Após configurar a rede via navegador, o ESP32 se conecta e inicia o servidor HTTP.

---

## 🧠 Armazenamento de Dados

As leituras são armazenadas em um array de structs `Leitura`, com as seguintes informações:

```cpp
struct Leitura {
  float temperatura;
  float umidade;
  time_t timestamp;
};
```

- Capacidade máxima: 50 leituras (`MAX_LEITURAS`)
- As leituras são atualizadas a cada 10 segundos (`intervaloLeitura`)
- Se o limite for atingido, novas leituras são descartadas

---

## 📤 Endpoint HTTP

### **GET** `/historico`

Retorna as leituras mais recentes no seguinte formato JSON:

```json
{
  "leituras": [
    {
      "temperatura": 25.3,
      "umidade": 61.1,
      "hora": "2025-06-06 20:25:00"
    },
    {
      "temperatura": 24.9,
      "umidade": 59.8,
      "hora": "2025-06-06 20:25:10"
    }
  ]
}
```

- Cada leitura inclui data e hora no formato `YYYY-MM-DD HH:MM:SS`
- Resposta com `Content-Type: application/json`

---

## 🔄 Loop Principal

No `loop()`, o servidor HTTP trata requisições via `server.handleClient()`. A cada 10 segundos:

1. Simula nova leitura de temperatura e umidade com variação aleatória
2. Armazena a leitura no histórico, se houver espaço

---
