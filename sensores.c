#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
#include <ArduinoJson.h>
 
#define DHTPIN 4          
#define DHTTYPE DHT11
#define LDRPIN 34        
#define LEDPIN 2          
 
const char* SSID = "FIAP-IOT";      
const char* PASSWORD = "F!@p25.IOT";
const char* BROKER_MQTT = "54.227.182.115";        
const int BROKER_PORT = 1883;
const char* TOPICO_SUBSCRIBE = "/TEF/device001/cmd";
const char* TOPICO_PUBLISH = "/TEF/device001/attrs";
const char* ID_MQTT = "fiware_001";
 
WiFiClient espClient;
PubSubClient MQTT(espClient);
DHT dht(DHTPIN, DHTTYPE);
 
char EstadoSaida = '0';
 
// ===== FUNÇÕES =====
void initSerial() {
  Serial.begin(115200);
}
 
void initWiFi() {
  WiFi.begin(SSID, PASSWORD);
  Serial.print("Conectando-se ao WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}
 
void initMQTT() {
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
  MQTT.setCallback(mqtt_callback);
}
 
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (int i = 0; i < length; i++) msg += (char)payload[i];
  Serial.print("Mensagem recebida: ");
  Serial.println(msg);
 
  if (msg.indexOf("on") >= 0) {
    digitalWrite(LEDPIN, HIGH);
    EstadoSaida = '1';
  } else if (msg.indexOf("off") >= 0) {
    digitalWrite(LEDPIN, LOW);
    EstadoSaida = '0';
  }
}
 
void reconnectMQTT() {
  while (!MQTT.connected()) {
    Serial.print("Conectando ao Broker MQTT...");
    if (MQTT.connect(ID_MQTT)) {
      Serial.println("Conectado!");
      MQTT.subscribe(TOPICO_SUBSCRIBE);
    } else {
      Serial.println("Falha, tentando novamente em 2s...");
      delay(2000);
    }
  }
}
 
void VerificaConexoesWiFIEMQTT() {
  if (!MQTT.connected()) reconnectMQTT();
  if (WiFi.status() != WL_CONNECTED) initWiFi();
}
 
void sendDataMQTT(float temp, float hum, int lum) {
  StaticJsonDocument<256> doc;
  doc["temperatura"] = temp;
  doc["umidade"] = hum;
  doc["luminosidade"] = lum;
  doc["estadoLed"] = (EstadoSaida == '1') ? "on" : "off";
 
  char buffer[256];
  serializeJson(doc, buffer);
 
  MQTT.publish(TOPICO_PUBLISH, buffer);
  Serial.println("Dados enviados ao FIWARE:");
  Serial.println(buffer);
}
 
void setup() {
  initSerial();
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, LOW);
  initWiFi();
  initMQTT();
  dht.begin();
}
 
void loop() {
  VerificaConexoesWiFIEMQTT();
  MQTT.loop();
 
  float temperatura = dht.readTemperature();
  float umidade = dht.readHumidity();
  int ldrValue = analogRead(LDRPIN);
  int luminosidade = map(ldrValue, 0, 4095, 0, 100);
 
  if (!isnan(temperatura) && !isnan(umidade)) {
    sendDataMQTT(temperatura, umidade, luminosidade);
  } else {
    Serial.println("Falha na leitura do DHT11!");
  }
 
  delay(5000);
}
