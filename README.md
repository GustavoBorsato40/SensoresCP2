# Sensores CP2, Edge Computing

Projeto IoT Vinheria Agnello (FIWARE + ESP32)

Autores :
Gustavo Borsato (rm - 564621)
Vitor Rampazzi (rm - 562270)
Daniel Brito (rm - 566236)


Versão final para hardware físico – integração com FIWARE (broker MQTT da FIAP)

⸻

- Descrição
  
Este projeto implementa um sistema IoT com ESP32 que coleta dados ambientais (temperatura, umidade e luminosidade) e os envia para uma plataforma FIWARE via protocolo MQTT.
Além disso, o sistema permite o controle remoto de um LED (simbolizando um atuador) através de comandos MQTT recebidos do broker.

⸻

- Componentes Utilizados
  
Componente Função
ESP32 Microcontrolador com WiFi integrado
DHT22 Sensor de temperatura e umidade
LDR Sensor de luminosidade
LED (onboard ou externo) Atuador controlado via MQTT
Broker MQTT (FIWARE/Orion) Servidor para comunicação entre dispositivo e sistema

⸻

- Bibliotecas Necessárias

#include <WiFi.h>           // Conexão WiFi
#include <PubSubClient.h>   // Comunicação MQTT
#include "DHT.h"            // Leitura do sensor DHT22
#include <ArduinoJson.h>    // Criação e envio de mensagens JSON

⸻

- Definições e Configurações

#define DHTPIN 4          // Pino do DHT22
#define DHTTYPE DHT22
#define LDRPIN 34         // Pino analógico do LDR
#define LEDPIN 2          // LED onboard do ESP32

- Configurações de Rede e Broker MQTT

const char* SSID = "NOME_DA_SUA_REDE_WIFI";
const char* PASSWORD = "SENHA_DA_SUA_REDE_WIFI";
const char* BROKER_MQTT = "44.210.89.155";   // Servidor FIWARE
const int BROKER_PORT = 1883;
const char* TOPICO_SUBSCRIBE = "/TEF/device001/cmd";   // Recebe comandos
const char* TOPICO_PUBLISH = "/TEF/device001/attrs";   // Envia dados
const char* ID_MQTT = "fiware_001";

⸻

- Funções Principais
  
initSerial()
Inicializa a comunicação serial para debug e monitoramento no console.
initWiFi()
Realiza a conexão com a rede WiFi definida nas constantes.
Repete tentativas até obter sucesso.
initMQTT()
Configura o cliente MQTT e define a função de callback (mqtt_callback) que será executada quando o dispositivo receber mensagens.
mqtt_callback(char* topic, byte* payload, unsigned int length)
Trata mensagens recebidas via MQTT.
Se o payload contiver "on", acende o LED; se contiver "off", apaga.
Também atualiza a variável EstadoSaida.
reconnectMQTT()
Tenta reconectar ao broker caso a conexão seja perdida.
Ao reconectar, faz a inscrição no tópico de comandos.
VerificaConexoesWiFIEMQTT()
Garante que as conexões WiFi e MQTT estejam ativas.
Reinicia conexões automaticamente em caso de falha.
sendDataMQTT(float temp, float hum, int lum)
Envia dados coletados (temperatura, umidade, luminosidade e estado do LED) para o broker MQTT em formato JSON.
Exemplo de payload:
{
 "temperatura": 25.4,
 "umidade": 62.3,
 "luminosidade": 80,
 "estadoLed": "on"
}

⸻

- Função setup()

Configura o ambiente inicial do sistema:
1. Inicializa a porta serial.
2. Define o pino do LED como saída e o apaga.
3. Conecta ao WiFi.
4. Conecta ao broker MQTT.
5. Inicializa o sensor DHT22.
   
⸻

- Função loop()
  
Executa continuamente:
1. Verifica conexões WiFi e MQTT.
2. Mantém o cliente MQTT ativo (MQTT.loop()).
3. Lê os valores dos sensores DHT22 e LDR.
4. Converte o valor do LDR para uma escala percentual (0–100%).
5. Envia os dados ao FIWARE via MQTT a cada 5 segundos.
6. Caso o DHT22 falhe, exibe mensagem de erro no monitor serial.
   
⸻

- Fluxo de Comunicação MQTT
  
Direção Tópico Conteúdo
→ FIWARE /TEF/device001/attrs JSON com dados dos sensores
← FIWARE /TEF/device001/cmd Comando "on" ou "off" para o LED

⸻

- Exemplo de Saída no Serial Monitor
  
Conectando-se ao WiFi...
WiFi conectado!
IP: 192.168.1.12
Conectando ao Broker MQTT... Conectado!
Dados enviados ao FIWARE:
{"temperatura":24.7,"umidade":61.5,"luminosidade":73,"estadoLed":"off"}
Mensagem recebida: on

⸻

- Observações Importantes
  
• Alterar SSID e PASSWORD para as credenciais reais da rede WiFi.
• Verificar se o broker MQTT (IP e porta) está ativo.
• Sensor DHT22 deve estar corretamente conectado ao pino 4 do ESP32.
• LDR deve estar ligado ao pino 34 (ADC) com divisor resistivo.
• Certifique-se de instalar as bibliotecas corretas via Arduino IDE → Gerenciador de Bibliotecas.
