
#include <SoftwareSerial.h> 
#include <RHReliableDatagram.h>               //Used for LoRa module
#include <RH_RF95.h>                          //Used for Lora module
#include <SPI.h>

#define CLIENT_ADDRESS 4
#define SERVER_ADDRESS 2
 

#define Reset 10
#define RX 6
#define TX 7
SoftwareSerial SoftSerial(RX, TX);                 //Readind data from A3 pin of Board

struct Sensor_data{
  float soilMoisture = 0.0;
  float soilTemperature = 0.0;
};

struct nodeData{
  long nodeNumber = 0.0;
  float batteryVoltage = 0.0;
  float soilMoisture = 0.0;
  float soilTemperature = 0.0;
  float airTemperature = 0.0;
  float airMoisture = 0.0;
};


int batteryVoltagePin = A3;  
byte buff[8];
int Read;
byte buf[RH_RF95_MAX_MESSAGE_LEN];
double packetNo = 0;
int availableBytes;
int value = -1;

Sensor_data data;
nodeData dataFromSensor;

RH_RF95 driver;
RHReliableDatagram manager(driver, CLIENT_ADDRESS);

void gatherDataFromSensors(){
    value = 1;
    
    // say what you got:
    Serial.print("I received: ");
    Serial.println(value, DEC); 

    SoftSerial.write(value);
    
      delay(100);
      
      if(SoftSerial.available()>0){
         Serial.println("Recieving data");
        //availableBytes = SoftSerial.available();
        for(int n = 0 ; n < 8; n++){
        buff[n] = SoftSerial.read();
        }
        //SoftSerial.readBytes(buf,8);
   
        memcpy(&data , buff ,  sizeof(buff));
        Serial.println(data.soilMoisture);
        Serial.println(data.soilTemperature);
        
      }
      else{Serial.println("Serial not Available");}
          

    dataFromSensor.soilTemperature = data.soilTemperature;
    dataFromSensor.soilMoisture = data.soilMoisture;
    dataFromSensor.batteryVoltage = analogRead(batteryVoltagePin)/(0.12821);   //Voltage Divider between 1k and 6.8 k
    dataFromSensor.nodeNumber = CLIENT_ADDRESS -1 ;
    dataFromSensor.airMoisture = 0.0;
    dataFromSensor.airTemperature = 0.0;
   
   
}

void setup() {
  // put your setup code here, to run once:
  SoftSerial.begin(9600);
  Serial.begin(9600);
  Serial.println("Starting...");
  while (!Serial) ; 
  if (!manager.init())
    Serial.println("init failed");
  pinMode(Reset,OUTPUT);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(300000);
  gatherDataFromSensors();
  
  memcpy(buf, &dataFromSensor, sizeof(dataFromSensor));

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
  
  
}
