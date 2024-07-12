#include <WiFi.h>
#include <PubSubClient.h>

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

// Function declarations
void setup_wifi();
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();

void setup() 
{
  // Serial monitor
  Serial.begin(115200);

  // Connect to WiFi
  setup_wifi();

  // Set MQTT server and callback function
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() 
{
  // Jika koneksi MQTT terputus, coba untuk tersambung kembali
  if (!client.connected())
    reconnect();

  // Menghandle pesan dari broker MQTT
  client.loop();
  
  // Contoh pengiriman data ke broker MQTT setiap 5 detik
  static unsigned long lastMillis = 0;
  if (millis() - lastMillis > 5000) 
  {
    lastMillis = millis();
    // Kirim pesan ke topik "esp32/test"
    String message = "Hello from ESP32";
    client.publish("esp32/test", message.c_str());
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
  String state = "";
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  // Print pesan yang diterima
  for (int i = 0; i < length; i++) {
    state += (char)payload[i];
    Serial.print((char)payload[i]);
  }

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
      client.subscribe("garudahacks");
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
