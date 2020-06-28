#include <SoftwareSerial.h> 
#include <RH_RF95.h>
#include <RHReliableDatagram.h>

#include <SPI.h>
#include "DHT.h"
//#include "MutichannelGasSensor.h"


#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2
#define DHTPIN 5


#define Reset 8
#define RX 6
#define TX 7
#define RX2 3
#define TX2 4


SoftwareSerial SoftSerial(RX, TX); 
SoftwareSerial ESPSerial(RX2, TX2);  

#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);

struct Sensor_data{
  float soilMoisture = 0.0;
  float soilTemperature = 0.0;
};

Sensor_data Sensordata;

struct nodeData{
  long nodeNumber = 0;
  float batteryVoltage = 0.0;
  float soilMoisture = 0.0;
  float soilTemperature = 0.0;
  float airTemperature = 0.0;
  float airMoisture = 0.0;
};




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


allData dataFromSensor;

RH_RF95 driver;
RHReliableDatagram manager(driver, SERVER_ADDRESS);


byte data[] = "Ack";
// Dont put this on the stack:
byte buf[sizeof(nodeData)]; 
int batteryVoltagePin = A3;  
byte buff[8];
byte buff2[sizeof(allData)];
int value = -1;
long timenow = 0;
int period = 10000;



void gatherDataFromSensors(){
    
    value = 1;
    
    // say what you got:
    Serial.print("I received: ");
    Serial.println(value, DEC); 

    SoftSerial.write(value);
    
    delay(50);
    if(SoftSerial.available()>0){

       for(int n = 0 ; n < sizeof(buff); n++){
        buff[n] = SoftSerial.read();
        }
        memcpy(&Sensordata , buff ,  sizeof(buff));
        Serial.println(Sensordata.soilMoisture);
        Serial.println(Sensordata.soilTemperature);
      }
      else{
        Serial.println("Serial not Available");
        }

    
    dataFromSensor.soilTemperature = Sensordata.soilTemperature;
    dataFromSensor.soilMoisture = Sensordata.soilMoisture;
    dataFromSensor.batteryVoltage = (analogRead(batteryVoltagePin)*(5/1024))/(0.12821);   //Voltage Divider between 1k and 6.8 k
    dataFromSensor.nodeNumber = 1;
    dataFromSensor.airTemperature = dht.readTemperature();
    /*dataFromSensor.NH3 = gas.measure_NH3();
    dataFromSensor.CO = gas.measure_CO();
    dataFromSensor.NO2 = gas.measure_NO2();
    dataFromSensor.C3H8 = gas.measure_C3H8();
    dataFromSensor.C3H8 = gas.measure_C3H8();
    dataFromSensor.C4H10 = gas.measure_C4H10();
    dataFromSensor.CH4 = gas.measure_CH4();
    dataFromSensor.H2 = gas.measure_H2();
    dataFromSensor.C2H5OH = gas.measure_C2H5OH();*/
    dataFromSensor.airMoisture = dht.readHumidity();
    
    Serial.println(dataFromSensor.airMoisture);
    Serial.println(dataFromSensor.airTemperature);
    //Serial.println("Data Gathered");
    // printData(dataFromSensor);

     

    memcpy(buff2 , &dataFromSensor , sizeof(buff2));
    ESPSerial.write(buff2 , sizeof(buff2));
   //Serial.write((const uint8_t *) &dataFromSensor, sizeof(dataFromSensor));
   //Serial.println("Data Sent");
  
}

void setup() {
  // put your setup code here, to run once:
  SoftSerial.begin(9600);
  Serial.begin(9600);
  ESPSerial.begin(115200);
  Serial.println("Starting...");
  dht.begin();
  Serial.println("power on!");
  //gas.begin(0x04);//the default I2C address of the slave is 0x04
  //gas.powerOn();
  //Serial.print("Firmware Version = ");
  //Serial.println(gas.getVersion());
  Serial.println("-----------------");
  while (!Serial) ; 
  if (!manager.init())
    Serial.println("init failed");
  pinMode(Reset,OUTPUT);
  
}

void loop() {
  // put your main code here, to run repeatedly:

if (millis() > timenow *1000 + period){

     gatherDataFromSensors();
     timenow = millis()/1000;
  }
  
  
  
  if (manager.available())
  {
    // Wait for a message addressed to us from the client
    uint8_t len = sizeof(buf);
    uint8_t from;
    if (manager.recvfromAck(buf, &len, &from))
    {
      nodeData incomingData;
      
      memcpy(&incomingData, buf, sizeof(incomingData));

      Serial.print("Data Receuved from Node = ");
      Serial.println(incomingData.nodeNumber);
      Serial.print("Soil Moisture = ");
      Serial.println(incomingData.soilMoisture);
      Serial.print("Soil Temperature = ");
      Serial.println(incomingData.soilTemperature);
      allData slaveData;
      slaveData.soilTemperature = incomingData.soilTemperature;
      slaveData.soilMoisture = incomingData.soilMoisture;
      slaveData.batteryVoltage = incomingData.batteryVoltage; 
      slaveData.nodeNumber = incomingData.nodeNumber;
      slaveData.airMoisture = dht.readHumidity();
      slaveData.airTemperature =  dht.readTemperature();

      //printData(slaveData);
      
      //Serial.write((const uint8_t *) &slaveData, sizeof(slaveData));

      memcpy(buff2 , &slaveData , sizeof(buff2));
      ESPSerial.write(buff2 , sizeof(buff2));

      Serial.println("Data Sent from Node = ");
      Serial.println(incomingData.nodeNumber);
      // Send a reply back to the originator client
      if (!manager.sendtoWait(data, sizeof(data), from)){
        Serial.println("sendtoWait failed");}
      
        
    }
  }
}


/*void printData(allData x){
  Serial.println("--------------------------------------");
  Serial.println("             Sensor Data               ");
  Serial.print("nodeNumber = ");
  Serial.println(x.nodeNumber);
  /*Serial.print("batteryVoltage = ");
  Serial.println(x.batteryVoltage);
  Serial.print("soilMoisture = ");
  Serial.println(x.soilMoisture);
  Serial.print("soilTemperature = ");
  Serial.println(x.soilTemperature);*/
  /*Serial.print("airTemperature = ");
  Serial.println(x.airTemperature);
  Serial.print("airMoisture = ");
  Serial.println(x.airMoisture);*/
  /*Serial.print("NH3 = ");
  Serial.println(x.NH3);
  Serial.print("CO = ");
  Serial.println(x.CO);
  Serial.print("NO2 = ");
  Serial.println(x.NO2);
  Serial.print("C3H8 = ");
  Serial.println(x.C3H8);
  Serial.print("C4H10 = ");
  Serial.println(x.C4H10);
  Serial.print("CH4 = ");
  Serial.println(x.CH4);
  Serial.print("H2 = ");
  Serial.println(x.H2);
  Serial.print("C2H5OH = ");
  Serial.println(x.C2H5OH);
 
  Serial.println("--------------------------------------");
 
  }*/
