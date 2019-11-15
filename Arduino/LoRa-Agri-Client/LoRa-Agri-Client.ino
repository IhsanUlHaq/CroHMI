#include <RHReliableDatagram.h>
#include <RH_RF95.h>
#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define SOILMMAXV 560
#define SOILMMINV 320

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

#define ONE_WIRE_BUS 7

int batteryVoltagePin = A3;
int soilMoisturePin = A2;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

RH_RF95 driver;
RHReliableDatagram manager(driver, CLIENT_ADDRESS);

struct nodeData{
  long nodeNumber;
  float batteryVoltage;
  float soilMoisture;
  float soilTemperature;
  float airTemperature;
  float airMoisture;
};

// Dont put this on the stack:
byte buf[RH_RF95_MAX_MESSAGE_LEN];

double packetNo = 0;

float calculateMP(int analogInput){

  if(analogInput > SOILMMAXV){
    return 100;
  } else if(analogInput < SOILMMINV){
    return 0;
  } else{
    float m = ((float(0-100))/(float(SOILMMAXV - SOILMMINV)));
    float c = (-1*(m*float(SOILMMINV))) + 100;
    float y = m*float(analogInput) + c;
    return y;
  }
  
}

nodeData gatherDataFromSensors(){

  sensors.requestTemperatures();

  float tempSoilTemperature = sensors.getTempCByIndex(0);
  float tempSoilMoisture = 0.0;
  float tempBatteryVoltageScaled = 0.0;
  float tempBatteryVoltageActual = 0.0;

  int adcBatteryVoltage = analogRead(batteryVoltagePin);
  int adcSoilMoisture = analogRead(soilMoisturePin);

  tempBatteryVoltageScaled = (float(adcBatteryVoltage)) * (3.29/1023.0);
  tempBatteryVoltageActual = tempBatteryVoltageScaled * 118.0/18.0;

  tempSoilMoisture = calculateMP(analogRead(soilMoisturePin));
  
  Serial.println(tempSoilTemperature);
  Serial.println(tempSoilMoisture);

  nodeData dataFromSensor;
  
  dataFromSensor.soilTemperature = sensors.getTempCByIndex(0);
  dataFromSensor.soilMoisture = tempSoilMoisture;
  dataFromSensor.batteryVoltage = analogRead(batteryVoltagePin);
  dataFromSensor.nodeNumber = 1;
  dataFromSensor.airMoisture = 0.0;
  dataFromSensor.airTemperature = 0.0;

  return dataFromSensor;
  
}

void setup() {
  Serial.begin(9600);
  while (!Serial) ; 
  if (!manager.init())
    Serial.println("init failed");

  sensors.begin();
}


void loop() {

  nodeData gatheredData = gatherDataFromSensors();
  
  memcpy(buf, &gatheredData, sizeof(gatheredData));

  packetNo++;
  
  Serial.print("Sending to Server | Packet No: ");
  Serial.println(packetNo);
    
  if (manager.sendtoWait(buf, sizeof(buf), SERVER_ADDRESS))
  {
    // Now wait for a reply from the server
    uint8_t len = sizeof(buf);
    uint8_t from;   
    if (manager.recvfromAckTimeout(buf, &len, 2000, &from))
    {
      Serial.print("got reply from : 0x");
      Serial.print(from, HEX);
      Serial.print(": ");
      Serial.println((char*)buf);
    }
    else
    {
      Serial.println("No reply, is the server running?");
    }
  }
  else
    Serial.println("sendtoWait failed");
  delay(300000);
}
