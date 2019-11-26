#include <RHReliableDatagram.h>
#include <RH_RF95.h>
#include <SPI.h>
//#include <OneWire.h>
//#include <DallasTemperature.h>
#include "DHT.h"

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2
#define ONE_WIRE_BUS 4
#define DHT_PIN 9

#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);

//OneWire oneWire(ONE_WIRE_BUS);
//DallasTemperature sensors(&oneWire);

RH_RF95 driver;
RHReliableDatagram manager(driver, SERVER_ADDRESS);

struct nodeData{
  
  long nodeNumber;
  float batteryVoltage;
  float soilMoisture;
  float soilTemperature;
  float airTemperature;
  float airMoisture;
  
} incomingData;

byte data[] = "Ack";
// Dont put this on the stack:
byte buf[RH_RF95_MAX_MESSAGE_LEN];

void setup() {

  //Starting Dallas Temperature Sensor
//  sensors.begin();

  //Starting DHT11
  dht.begin();

  Serial.begin(9600);
  while (!Serial) ; 
  if (!manager.init())
    Serial.println("init failed");
}

void gatherSendSelfData(){

  int analogInSoilMoisture = 0;
  float tempSoilMoisture = 0.0;

  //Creating Node Data Object
  nodeData selfData;

  
//  sensors.requestTemperatures();
  
  //Gathering Data
//  selfData.soilTemperature = sensors.getTempCByIndex(0); // Soil Temperature
  //selfData.airTemperature = dht.readTemperature();       // Air Temperature
  selfData.airMoisture = dht.readHumidity();             // Air Moisture
  selfData.nodeNumber = 5;                               // Node Number

  for (int i = 0; i < 5; i++){
    analogInSoilMoisture = analogInSoilMoisture + analogRead(A5);
    delay(10);
  }

  analogInSoilMoisture = analogInSoilMoisture / 5;
  
  tempSoilMoisture = 163.63 - (0.1818*float(analogInSoilMoisture));  
  
  selfData.soilMoisture = tempSoilMoisture;
  selfData.batteryVoltage = float(analogRead(A4))*0.03684;
  
  Serial.write((const uint8_t *) &selfData, sizeof(selfData));

  delay(1000);
    
}

void loop() {

  if (Serial.available()){

    if (Serial.read() == 'x'){
      gatherSendSelfData();
    }
    
  }
  
  if (manager.available())
  {
    // Wait for a message addressed to us from the client
    uint8_t len = sizeof(buf);
    uint8_t from;
    if (manager.recvfromAck(buf, &len, &from))
    {
      
      memcpy(&incomingData, buf, sizeof(incomingData));

      incomingData.airTemperature = dht.readTemperature();
      incomingData.airMoisture = dht.readHumidity();
      
      Serial.write((const uint8_t *) &incomingData, sizeof(incomingData));
      

      // Send a reply back to the originator client
      if (!manager.sendtoWait(data, sizeof(data), from))
        Serial.println("sendtoWait failed");
    }
  }
}
