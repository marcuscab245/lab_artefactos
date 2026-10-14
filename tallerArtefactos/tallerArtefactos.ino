#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
// ======================================================
// CONFIGURACIÓN WIFI
// ======================================================

const char* WIFI_SSID = "ARTEFACTOS";
const char* WIFI_PASSWORD = "s^3sQ#9teskVgnb";


// ======================================================
// CONFIGURACIÓN MQTT
// ======================================================


const char* MQTT_SERVER = "10.110.0.8";
const int MQTT_PORT = 1883;

// Topics utilizados por el dashboard React.
const char* TOPIC_TEMPERATURA = "clima/esp32/temperatura";
const char* TOPIC_HUMEDAD = "clima/esp32/humedad";
const char* TOPIC_LUZ = "clima/esp32/luz";


// ======================================================
// CONFIGURACIÓN DE SENSORES
// ======================================================

#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
const int PIN_LDR = 32;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Enviar una lectura cada 2 segundos.
const unsigned long INTERVALO_ENVIO = 2000;

unsigned long ultimoEnvio = 0;

void conectarWiFi() {

  Serial.print("Conectando a WiFi");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi conectado");

  Serial.print("IP del ESP32: ");
  Serial.println(WiFi.localIP());
}

void conectarMQTT() {

  while (!mqttClient.connected()) {

    Serial.print("Conectando a Mosquitto...");

    // Crear un Client ID diferente para este ESP32.
    String clientId = "ESP32-";
    clientId += String(
      (uint32_t)(ESP.getEfuseMac() & 0xFFFFFFFF),
      HEX
    );

    if (mqttClient.connect(clientId.c_str())) {

      Serial.println(" conectado");

    } else {

      Serial.print(" error, rc=");
      Serial.print(mqttClient.state());
      Serial.println(". Reintentando en 2 segundos...");

      delay(2000);
    }
  }
}


// ======================================================
// LEER Y PUBLICAR SENSORES
// ======================================================

void publicarSensores() {
  // temperatura
  float temp = dht.readTemperature();
  // humedad
  float hum = dht.readHumidity();

  // lectura LDR   
  int ldrADC = analogRead(PIN_LDR);
  int luz = map(ldrADC, 0, 4095, 0, 100);
  luz = constrain(luz, 0, 100);

  // MQTT
  mqttClient.publish(TOPIC_TEMPERATURA, String(temp, 2).c_str());
  mqttClient.publish(TOPIC_HUMEDAD, String(hum, 2).c_str());
  mqttClient.publish(TOPIC_LUZ, String(luz).c_str());

  // monitor serail
  Serial.print("Temperatura: ");
  Serial.print(temp, 2);
  Serial.println(" °C");

  Serial.print("Humedad: ");
  Serial.print(hum, 2);
  Serial.println(" %");

  Serial.print("LDR ADC: ");
  Serial.print(ldrADC);
  Serial.print(" | Luz: ");
  Serial.print(luz);
  Serial.println(" %");

  Serial.println("------------------------------");
}

void setup() {

  Serial.begin(115200);

  // (Estudiante)
  // Inicializar los sensores y configurar los pines
  // necesarios.
  dht.begin();

  conectarWiFi();

  mqttClient.setServer(
    MQTT_SERVER,
    MQTT_PORT
  );
}

void loop() {

  // Si se pierde WiFi, volver a conectarse.
  if (WiFi.status() != WL_CONNECTED) {
    conectarWiFi();
  }

  // Si se pierde MQTT, volver a conectarse.
  if (!mqttClient.connected()) {
    conectarMQTT();
  }

  mqttClient.loop();


  // Publicar sensores cada 2 segundos.
  if (millis() - ultimoEnvio >= INTERVALO_ENVIO) {

    ultimoEnvio = millis();

    publicarSensores();
  }
}