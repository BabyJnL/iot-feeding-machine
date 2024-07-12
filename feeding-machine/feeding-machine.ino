#include <WiFi.h>
#include <PubSubClient.h>
#include "HX711.h"
#include <ESP32Servo.h>

// DEFINE PIN
#define DOUT 14
#define CLK  12
#define SERVO_PIN 13

// WiFi credentials
const char* ssid = "cahaya majun";
const char* password = "cahaya89";

// MQTT broker setup
const char* mqtt_server = "203.194.113.47";
const int mqtt_port = 1883;  // Port default MQTT
const char* broker_username = "garudahacks";
const char* broker_password = "asustufgaming";

WiFiClient espClient;
PubSubClient client(espClient);
HX711 scale;
Servo servoController;

float calibration_factor = 97.50;
int GRAM;

// GLOBAL VARIABLE
int targetWeight = 0;

// Function declarations
void setup_wifi();
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();

void setup() 
{
  // Serial monitor
  Serial.begin(115200);

  // Scale initialization
  scale.begin(DOUT, CLK);
  scale.set_scale();
  scale.tare();
  long zero_factor = scale.read_average();
  Serial.print("Zero factor: ");
  Serial.println(zero_factor);

  // Servo initialization
  servoController.attach(SERVO_PIN);

  // Connect to WiFi
  setup_wifi();

  // Set MQTT server and callback function
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  delay(1000);
}

void printWeight()
{
  scale.set_scale(calibration_factor);
  GRAM = scale.get_units(), 4;
  Serial.print("Reading: ");
  Serial.print(GRAM);
  Serial.print(" Gram");
  Serial.print(" calibration_factor: ");
  Serial.print(calibration_factor);
  Serial.println();
}

void loop() 
{
  printWeight();

  // Jika koneksi MQTT terputus, coba untuk tersambung kembali
  if (!client.connected())
    reconnect();

  // Menghandle pesan dari broker MQTT
  client.loop();

  openTube();
  
  // Contoh pengiriman data ke broker MQTT setiap 3 detik
  static unsigned long lastMillis = 0;
  if (millis() - lastMillis > 3000) 
  {
    lastMillis = millis();
    // Kirim pesan ke topik "esp32/test"
    String message = String(GRAM);
    client.publish("weightData", message.c_str());
  }
}

void setup_wifi() 
{
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) 
{
  String data = "";
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  // Print pesan yang diterima
  for (int i = 0; i < length; i++) {
    data += (char)payload[i];
    Serial.print((char)payload[i]);
  }

  targetWeight = GRAM - data.toInt();

  Serial.println();
}

void reconnect() {
  // Loop sampai terhubung dengan broker MQTT
  while (!client.connected()) 
  {
    Serial.print("Attempting MQTT connection...");
    
    // ID klien unik
    String clientId = "ESP32Client-";

    clientId += String(random(0xffff), HEX);
    
    // Coba untuk tersambung
    if (client.connect(clientId.c_str(), broker_username, broker_password)) 
    {
      Serial.println("connected");
      // Subscribe ke topik "esp32/test"
      client.subscribe("garudahacks/tes");
    } else 
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Tunggu 5 detik sebelum mencoba lagi
      delay(5000);
    }
  }
}

void openTube()
{
  if(targetWeight != 0)
  {
    if(GRAM > targetWeight)
    {
      servoController.write(120);
    }
    else
    {
      servoController.write(72);
      targetWeight = 0;
    }
  }
}