/*

---- GSM Module ----

*/

//GSM Modem Model Definition
#define TINY_GSM_MODEM_A7

//Including libraries for internet connection
#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>

//Library for GPS
#include <TinyGPS++.h>
TinyGPSPlus gps;

//Hardware Serial Definition
#define GPSSerial Serial1
#define SerialMon Serial

//Setting up GSM Module
#define SerialAT Serial3 //SerialAT is for GSM Module

const char apn[] = "zongwap";
const char user[] = "";
const char pass[] = "";

const char server[] = "111.68.101.20";
String resource = "/CropHealth/datauploadscript.php?dat=2019";
const int  port = 80;

#ifdef DUMP_AT_COMMANDS
  #include <StreamDebugger.h>
  StreamDebugger debugger(SerialAT, SerialMon);
  TinyGsm modem(debugger);
#else
  TinyGsm modem(SerialAT);
#endif

TinyGsmClient client(modem);
HttpClient http(client, server, port);

/*

---- nRF Module ----

*/

//Including libraries for nRF24
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

/*

---- Other Variable Declaration ----

*/

//DHT - Air Moisture and Temperature Sensor
#include <dht.h>
dht DHT;

//Including libraries for temperature sensor
#include <OneWire.h>
#include <DallasTemperature.h>

//Configuring Soil Temperature Sensor
#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature soilTemperatureSensor(&oneWire);

//Defining radio pins
RF24 radio(9, 10); // CE, CSN

//Defining addresses for nRF24 Modules
byte Master[6] =   "00001";
byte Slave_01[6] = "00022";
byte Slave_02[6] = "00333";
byte Slave_03[6] = "04444";
byte Slave_04[6] = "05555";

//Necessary variables definition
int Wait = 3000;
unsigned int Count = 0;
unsigned int Time = 0;
int error = 9999;
int moisture = 1;
int temperature = 5;
int number = temperature * 10 + moisture;
int Soil_Moisture = 9999;
int Soil_Temp = 9999;
float Soil_Temperature = 9999.0;

//Variables to store data from nodes and master itself
float airM = 200.0;

float airT = 200.0;

float soilM1 = 200.0;
float soilM2 = 200.0;
float soilM3 = 200.0;
float soilM4 = 200.0;
float soilM5 = 200.0;

float soilT1 = 200.0;
float soilT2 = 200.0;
float soilT3 = 200.0;
float soilT4 = 200.0;
float soilT5 = 200.0;

String latitude = "12.11";
String longitude = "12.11";


//Pin definition for LED Indication
int S01 = 4;
int S02 = 5;
int S03 = 6;
int S04 = 7;

//Enable Pin for GSM Module
int pwrKeyGSM = 8;

//One-time run code
void setup() {
  
  //Setting pin modes for LEDs
  pinMode(S01, OUTPUT);
  pinMode(S02, OUTPUT);
  pinMode(S03, OUTPUT);
  pinMode(S04, OUTPUT);

  //Setting pin mode for turning on GSM Module
  pinMode(pwrKeyGSM, OUTPUT);

  //Setting Baud Rate of 9600, might need to switch it to 115200
  Serial.begin(115200);

  //Setting up GPS Connectivity
  GPSSerial.begin(9600);

  Serial.println("GPS Refreshing");
  Serial.println("");
  
  for(int i=1;i<15;i++){printGPS();}

  Serial.println("");
  Serial.println("GPS Refreshing Complete");
  Serial.println("");
  

  //nRF24 Setup
  radio.begin();
  radio.stopListening();
  radio.setPALevel(RF24_PA_MAX);

  //Turning off all LEDs (Remove LED Code Later)
  digitalWrite(S01, LOW);
  digitalWrite(S02, LOW);
  digitalWrite(S03, LOW);
  digitalWrite(S04, LOW);

  Serial.println("Turning On GSM Shield");

  digitalWrite(pwrKeyGSM, HIGH);
  delay(2500);
  digitalWrite(pwrKeyGSM, LOW);

  delay(10000);

  // Set GSM module baud rate
  SerialAT.begin(115200);
  delay(3000);

  // Restart takes quite some time
  SerialMon.println(F("Initializing modem..."));
  modem.restart();

  String modemInfo = modem.getModemInfo();
  SerialMon.print(F("Modem: "));
  SerialMon.println(modemInfo);
}

//Functions for nRF24 to work

void Send_data(uint8_t *Slave, int Message){
  
  radio.stopListening();
  radio.openWritingPipe(Slave);
  radio.write(&Message, sizeof(Message));
  
}

int Recv_data(int check){

  //Configure nRF to receive data
  radio.openReadingPipe(0, Master);
  radio.startListening();
  
  int Time = millis();
  int Count = Time;  
  
  while(!radio.available())
  {
    Time = millis();
     if(Count>Time)
    {
      Count = Time;
    }
    if(Time - Count >= Wait)
    {
      Serial.println("Timer Overflow");   
      return error;
    }
  }
  
  int received = 9999;
  
  radio.read(&received, sizeof(received));
  Serial.println(received);
  if(received <0)
  {
    check = -check;
  }
  if(received%10==check)
  {
      received = (received - check)/10;
      return received;
  }
  else
  {
    Serial.println("Error in Receiving");
    return error;
  }
}

/* 

GPS Functions

*/

void printGPS(){
  printLat(gps.hdop.hdop(), gps.hdop.isValid(), 6, 1);
  printLat(gps.location.lat(), gps.location.isValid(), 11, 6);
  printLong(gps.location.lng(), gps.location.isValid(), 12, 6);
  SerialMon.println();

  smartDelay(1000);

  if (millis() > 5000 && gps.charsProcessed() < 10)
    SerialMon.println(F("No GPS data received: check wiring"));
}

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (GPSSerial.available())
      gps.encode(GPSSerial.read());
  } while (millis() - start < ms);
}

static void printLat(float val, bool valid, int len, int prec)
{
  latitude="";
  if (!valid)
  {
    
    while (len-- > 1){
      latitude+="9";
      SerialMon.print('*');
    }
    SerialMon.print(' ');
  }
  else
  {
    latitude=String(val, prec);
    SerialMon.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i=flen; i<len; ++i)
      SerialMon.print(' ');
  }
  smartDelay(0);
}

static void printLong(float val, bool valid, int len, int prec)
{
  longitude="";
  if (!valid)
  {
    
    while (len-- > 1){
    longitude+="9";
      SerialMon.print('*');
    }
    SerialMon.print(' ');
  }
  else
  {
    longitude=String(val, prec);
    SerialMon.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i=flen; i<len; ++i)
      SerialMon.print(' ');
  }
  smartDelay(0);
}


/* 

Main Loop

*/

void loop(){

  
  Serial.println("---------------------------------------");

  //----------------------Node 1-----------------------

  //Refreshing variables
  Soil_Moisture = 9999;
  Soil_Temp = 9999;
  Soil_Temperature = 9999.0;
  
  Serial.println("---------Node 01---------");
  Send_data(Slave_01, number);

  Serial.println("Asking for Moisture");
  Soil_Moisture = Recv_data(moisture);
  Serial.print("Received Moisture: ");
  Serial.println(Soil_Moisture);
  
  Serial.println("Asking for Temperature");
  Soil_Temp = Recv_data(temperature);
  Serial.print("Received Moisture: ");
  Serial.println(Soil_Moisture);

  
  if(Soil_Moisture != error)
  {
    Serial.print("Humidity = ");
    Serial.println(Soil_Moisture);
    digitalWrite(S01, HIGH);
    
  }
  if(Soil_Temp != error)
  {
    Soil_Temperature = Soil_Temp/10.0;
    Serial.print("Temperature = ");
    Serial.println(Soil_Temperature);  
  }

  soilM1 = Soil_Moisture;
  soilT1 = Soil_Temperature;
   
  delay(500);
  digitalWrite(S01, LOW);
  
  //----------------------Node 2-----------------------
  
  //Refreshing variables
  Soil_Moisture = 9999;
  Soil_Temp = 9999;
  Soil_Temperature = 9999.0;
  
  Serial.println("---------Node 02---------");
  Send_data(Slave_02, number);

  Soil_Moisture = Recv_data(moisture);
  Soil_Temp = Recv_data(temperature);
  if(Soil_Moisture != error)
  {
    Serial.print("Humidity = ");
    Serial.println(Soil_Moisture);
    digitalWrite(S02, HIGH);
  }
  if(Soil_Temp != error)
  {
    Soil_Temperature = Soil_Temp/10.0;
    Serial.print("Temperature = ");
    Serial.println(Soil_Temperature);  
  }

  soilM2 = Soil_Moisture;
  soilT2 = Soil_Temperature;
  
  delay(500);
  digitalWrite(S02, LOW);
  
  //----------------------Node 3-----------------------

  //Refreshing variables
  Soil_Moisture = 9999;
  Soil_Temp = 9999;
  Soil_Temperature = 9999.0;
  
  Serial.println("---------Node 03---------");
  Send_data(Slave_03, number);

  Soil_Moisture = Recv_data(moisture);
  Soil_Temp = Recv_data(temperature);
  if(Soil_Moisture != error)
  {
    Serial.print("Humidity = ");
    Serial.println(Soil_Moisture);
    digitalWrite(S03, HIGH);
  }
  if(Soil_Temp != error)
  {
    Soil_Temperature = Soil_Temp/10.0;
    Serial.print("Temperature = ");
    Serial.println(Soil_Temperature);  
  }

  soilM3 = Soil_Moisture;
  soilT3 = Soil_Temperature;
    
  delay(500);
  digitalWrite(S03, LOW);
  
  //----------------------Node 4-----------------------

  //Refreshing variables
  Soil_Moisture = 9999;
  Soil_Temp = 9999;
  Soil_Temperature = 9999.0;

  Serial.println("---------Node 04---------");
  Send_data(Slave_04, number);

  Soil_Moisture = Recv_data(moisture);
  Soil_Temp = Recv_data(temperature);
  if(Soil_Moisture != error)
  {
    Serial.print("Humidity = ");
    Serial.println(Soil_Moisture);
    digitalWrite(S04, HIGH);
  }
  if(Soil_Temp != error)
  {
    Soil_Temperature = Soil_Temp/10.0;
    Serial.print("Temperature = ");
    Serial.println(Soil_Temperature);  
  }

  soilM4 = Soil_Moisture;
  soilT4 = Soil_Temperature;
  
  delay(500);
  digitalWrite(S04, LOW);

  //----------------------Master Node-----------------------

  //Refreshing variables
  Soil_Moisture = 9999;
  Soil_Temp = 9999;
  Soil_Temperature = 9999.0;

  //Reading Soil Temperature and Soil Moisture
  soilM5 = analogRead(A0);
  soilTemperatureSensor.requestTemperatures();
  soilT5 = soilTemperatureSensor.getTempCByIndex(0);

  //Reading Air Temperature and Air Moisture
  int dhtCheck = DHT.read11(52); //Change this pin number after confirmation
  airM = DHT.humidity;
  airT = DHT.temperature;

  Serial.println("Getting GPS Data");
  printGPS();
  Serial.println("GPS Data Aquisition Complete");
  
  delay(1000);
  
  //End of gathering data
  //Showing all data on Serial Monitor
  
  Serial.println("");
  Serial.println("---------All Data---------");
  Serial.println("");
  Serial.print("Air Temperature: ");
  Serial.println(airT);
  Serial.println("");
  Serial.print("Air Moisture: ");
  Serial.println(airM);
  Serial.println("");
  Serial.print("Soil Moisture Pole 1: ");
  Serial.println(soilM1);
  Serial.print("Soil Moisture Pole 2: ");
  Serial.println(soilM2);
  Serial.print("Soil Moisture Pole 3: ");
  Serial.println(soilM3);
  Serial.print("Soil Moisture Pole 4: ");
  Serial.println(soilM4);
  Serial.print("Soil Moisture Pole 5: ");
  Serial.println(soilM5);
  Serial.println("");
  Serial.print("Soil Temperature Pole 1: ");
  Serial.println(soilT1);
  Serial.print("Soil Temperature Pole 2: ");
  Serial.println(soilT2);
  Serial.print("Soil Temperature Pole 3: ");
  Serial.println(soilT3);
  Serial.print("Soil Temperature Pole 4: ");
  Serial.println(soilT4);
  Serial.print("Soil Temperature Pole 5: ");
  Serial.println(soilT5);
  Serial.println("");
  
  Serial.print("GPS Latitude: ");
  Serial.println(latitude);
  Serial.print("GPS Longitutde: ");
  Serial.println(longitude);
  Serial.println("");

  delay(2000);
  
  //Adding all the data to the HTTP GET Query
  resource = "/CropHealth/datauploadscript.php?dat=2019"; //Resetting the query
  
  Serial.println("Adding variables to query");
  resource = resource + "&lat=" + latitude + "&lng=" + longitude + "&airm=" + airM + "&airt=" + airT +"&soilm1=" + soilM1 + "&soilt1=" + soilT1 + "&pn1=1" + "&soilm2=" + soilM2 + "&soilt2=" + soilT2 + "&pn2=2" + "&soilm3=" + soilM3 + "&soilt3=" + soilT3 + "&pn3=3" + "&soilm4=" + soilM4 + "&soilt4=" + soilT4 + "&pn4=4" + "&soilm5=" + soilM5 + "&soilt5=" + soilT5 + "&pn5=5";
  Serial.println(resource);
  Serial.println("");

  delay(2000);

  /*

  GPSR Connection
  
  */

  SerialMon.print(F("Waiting for network..."));
  if (!modem.waitForNetwork()) {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" OK");

  SerialMon.print(F("Connecting to "));
  SerialMon.print(apn);
  SerialMon.print("...");
  if (!modem.gprsConnect(apn, user, pass)) {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" OK");

  SerialMon.print(F("Performing HTTP GET request... "));
  int err = http.get(resource);

  Serial.print("GET Successful");
  
  // Disconnect from Server and GPRS to conserve battery

  http.stop();
  SerialMon.println(F("Server disconnected"));

  modem.gprsDisconnect();
  SerialMon.println(F("GPRS disconnected"));

  
}
