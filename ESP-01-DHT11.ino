
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#define DHTPIN 1 //DHT11 data pin is connected to ESP8266 pin GPIO1 (TX)
#define DHTTYPE DHT11 //se coloca el tippo
// INSTANCIA DLE SENSOR
DHT dht(DHTPIN, DHTTYPE); // enlace a la libreria
// Update these with values suitable for your network.

const char* ssid = "ZTE";
const char* password = "Peru1234";
const char *mqtt_server = "192.168.1.5 "; 
const int mqtt_port = 1883; //....
const char *root_topic_subscribe = "ESP01/DHT11"; 
const char *root_topic_publish = "ESP01/DHT11"; 

/*********IP WIFI CONFIGURATION*********/   
IPAddress ip(192,168,1,150);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);
/***************************************/

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
long count=0;
#define LED1 0 // relay co  nnected to  GPIO0
#define LED2 2 // relay co  nnected to  GPIO2

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
  pinMode( LED1 , OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode( LED2 , OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  dht.begin(); // se inicia el sensor
   
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

//*****************************
//***    CONEXION WIFI      ***
//*****************************

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network 
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
   //IP STATICA
    WiFi.config(ip,gateway,subnet);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

//*****************************
//***       CALLBACK        ***
//*****************************

void callback(char* topic, byte* payload, unsigned int length) {
   String incoming = "";
  Serial.print("Mensaje recibido desde -> ");
  Serial.print(topic);
  Serial.print(" ");
for (int i = 0; i < length; i++) {
    incoming += (char)payload[i];
  }
  if (incoming == "1") {
    Serial.println("Encender Foco");
    digitalWrite(LED1, HIGH);
  } else if(incoming == "0") {
    Serial.println("Apagar Foco");
    digitalWrite(LED1, LOW);
  } 
   incoming.trim();
   Serial.println("Mensaje -> " + incoming);

}

//*****************************
//***    CONEXION MQTT      ***
//*****************************

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection..."); 
     // Creamos un cliente ID
    String clientId = "IOTICOS_H_W_";
    clientId += String(random(0xffff), HEX);
    // Intentamos conectar
    if (client.connect("root_topic_subscribe")) {
      Serial.println("Conectado!");
      // Nos suscribimos
      if(client.subscribe(root_topic_subscribe)){
        Serial.println("Suscripcion ok");
         digitalWrite(LED2, HIGH);
      }else{
        Serial.println("fallo Suscripciión");
      }
    } else {
      Serial.print("falló :( con error -> ");
      Serial.print(client.state());
      Serial.println(" Intentamos de nuevo en 5 segundos");
      digitalWrite(LED2, LOW);
      delay(5000);
    }
    
  }
}

void loop() {
 float t = dht.readTemperature(); //leed datos de t° y humedad
 float h = dht.readHumidity(); //Lee la humedad
 String strTmp;
 char chrTmp [5];
 strTmp += t; //concatenando
 
  Serial.print("Humedad: ");
  Serial.println(h);
  Serial.println(" %\t");
  Serial.print("Temperatura: ");
  Serial.println(t); 
  delay(50000);
   
  if (!client.connected()) {
    reconnect();
  }
  if (client.connected()){
   // String str = "La cuenta es -> " + String(count);
    strTmp.toCharArray(chrTmp,5);
    client.publish(root_topic_publish,chrTmp);
    count++;
    delay(300);
  }
  
  client.loop();

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, 75, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("iso3/rele3", msg);
  }
}
