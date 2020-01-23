#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>

#define RX2 D4
#define TX2 D5
SoftwareSerial ESPSerial(RX2, TX2); 

struct allData{
  long nodeNumber = 0;
  float batteryVoltage = 0.0;
  float soilMoisture = 0.0;
  float soilTemperature = 0.0;
  float airTemperature = 0.0;
  float airMoisture = 0.0;
  float NH3 = 0.0;
  float CO = 0.0;
  float NO2 = 0.0;
  float C3H8 = 0.0;
  float C4H10 = 0.0;
  float CH4 = 0.0;
  float H2 = 0.0;
  float C2H5OH = 0.0;
};

allData dataFromArduino;
byte buff[sizeof(allData)];

const char* ssid ="NIAQ"; //replace this with your wifi  name
const char* password ="indoorairquality"; //replace with your wifi password
char hostname[] ="mqtt-dashboard.com"; //replace this with IP address of machine 
//on which broker is installed
#define TOKEN "killmachine"

WiFiClient wifiClient;
PubSubClient client(wifiClient);

int status = WL_IDLE_STATUS;

void setup()
{
Serial.begin(115200);
ESPSerial.begin(115200);
WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
  Serial.println("ESP8266 AS PUBLISHER");
  client.setServer(hostname, 1883 ); //default port for mqtt is 1883
}

void loop()
{
  if ( !client.connected() )
  {
    reconnect();
  }
  if(ESPSerial.available()){
    Serial.println("Serial Available");
    for(int n = 0 ; n < sizeof(buff) ; n++ ){
      buff[n] = ESPSerial.read();
    } 
    Serial.println("Data Gathered");
    memcpy(&dataFromArduino , buff , sizeof(buff));
    MQTTPOST(dataString(&dataFromArduino));
    Serial.println("Data Sent");
   }
    
 
  
  delay(500);//delay 5 Sec
}

void MQTTPOST(String payload)
{
//payload formation begins here

  char attributes[1000];
  payload.toCharArray( attributes, 1000 );
  client.publish("Crohmi", attributes); //topic="test" MQTT data post command.
  Serial.println( attributes );
}
//this function helps you reconnect wifi as well as broker if connection gets disconnected.
void reconnect() 
{
  while (!client.connected()) {
    status = WiFi.status();
    if ( status != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("Connected to AP");
  }
  Serial.print("Connecting to Broker …");
  Serial.print(hostname);

  if ( client.connect("ESP8266 Device", TOKEN, NULL) )
  {
  Serial.println("[DONE]" );
  }
  else {
  Serial.println( " : retrying in 5 seconds]" );
  delay( 5000 );
}
}
}

String dataString(allData * data){
  String payload = "|";
  payload += data->nodeNumber;
  payload += "|";
  payload += data->batteryVoltage;
  payload += "|";
  payload += data->soilMoisture;
  payload += "|";
  payload += data->soilTemperature;
  payload += "|";
  payload += data->airTemperature;
  payload += "|";
  payload += data->airMoisture;
  payload += "|";
  payload += data->NH3;
  payload += "|";
  payload += data->CO;
  payload += "|";
  payload += data->NO2;
  payload += "|";
  payload += data->C3H8;
  payload += "|";
  payload += data->C4H10;
  payload += "|";
  payload += data->CH4;
  payload += "|";
  payload += data->H2;
  payload += "|";
  payload += data->C2H5OH;
  //payload += "|";
   return payload;
  }

