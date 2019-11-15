/**************************************************************
 *
 * TinyGSM Getting Started guide:
 *   http://tiny.cc/tiny-gsm-readme
 *
 * NOTE:
 * Some of the functions may be unavailable for your modem.
 * Just comment them out.
 *
 **************************************************************/
#include <SoftwareSerial.h>
//For the gps/////////////////////////////////////
#include <TinyGPS++.h>
// The TinyGPS++ object
TinyGPSPlus gps;
static const int RXPin = 10, TXPin = 11;
static const uint32_t GPSBaud = 9600;


// The serial connection to the GPS device
//SoftwareSerial ss(RXPin, TXPin);
#define ss Serial3
/////////////////////////////////////////////////


//for the gsm////////////////////////////////////
// Select your modem:
 #define TINY_GSM_MODEM_A7

// Set serial for debug console (to the Serial Monitor, speed 115200)
#define SerialMon Serial

// Set serial for AT commands (to the module)
// Use Hardware Serial on Mega, Leonardo, Micro
#define SerialAT Serial1

// or Software Serial on Uno, Nano
//SoftwareSerial SerialAT(2, 3); // RX, TX

//SoftwareSerial ss(RXPin, TXPin);

const char server[] = "35.238.149.191";

//#define DUMP_AT_COMMANDS
#define TINY_GSM_DEBUG SerialMon

// Your GPRS credentials
// Leave empty, if missing user or pass
const char apn[]  = "zongwap";
const char user[] = "";
const char pass[] = "";

#include <TinyGsmClient.h>

#ifdef DUMP_AT_COMMANDS
  #include <StreamDebugger.h>
  StreamDebugger debugger(SerialAT, SerialMon);
  TinyGsm modem(debugger);
#else
  TinyGsm modem(SerialAT);
#endif

#ifdef USE_SSL
  TinyGsmClientSecure client(modem);
  const int  port = 4322;
#else
  TinyGsmClient client(modem);
  const int  port = 4322;
#endif

///////////////////////////////////////////////////

////////////////For Sensor Values////////////////////
// Read Data from Grove - Multichannel Gas Sensor
#include <Wire.h>
#include "MutichannelGasSensor.h"
//For DHT Sensor
#include <dht.h>

dht DHT;
String nh3;
String co;
String no2;
String ch4;
String humid;
String temp;
String latitude;
String longitude;
String humid2;
String temp2;
 

void setup() {
  // Set console baud rate
  SerialMon.begin(2000000);
  delay(10);
  ss.begin(9600);

 //for the gps:
 for(int i=1;i<15;i++){printGPS();}
   /////////////////////
 
  //changing shield baud rate
  delay (8000);
  SerialAT.begin(115200);
  SerialAT.print("AT+IPR=9600\r");
  delay(2000);
  SerialMon.println("OK");
  
  pinMode(OUTPUT,8);
  digitalWrite(8,HIGH);
  //delay(3000);
  //digitalWrite(8,LOW);
  SerialAT.begin(9600);
  while(SerialAT.available()) SerialAT.read();
  
  //for(int i=1;i<15;i++){printGPS();}
  /////////////////Setup for Sensors Values///////////////////////////////////////
  gas.begin(0x04);//the default I2C address of the slave is 0x04
    gas.powerOn();
    SerialMon.print("Firmware Version = ");
    SerialMon.println(gas.getVersion());
    SerialMon.println("NH3,CO,NO2,CH4,Temp, Humidity");
///////////////////////////////////////////////////////////////////////////////////

  // Set GSM module baud rate
  //TinyGsmAutoBaud(SerialAT);
}

void loop() {
  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  DBG("Initializing modem...");
  if (!modem.restart()) {
    delay(1000);
    return;
  }

  String modemInfo = modem.getModemInfo();
  DBG("Modem:", modemInfo);

  // Unlock your SIM card with a PIN
  //modem.simUnlock("0000");

  DBG("Waiting for network...");
  if (!modem.waitForNetwork()) {
    delay(1000);
    return;
  }
  
  if (modem.isNetworkConnected()) {
    DBG("Network connected");
  }
  //SerialMon.println(""+nh3+"&"+co+"&"+no2+"&"+ch4+"&"+temp+"&"+humid);

while(true){

  DBG("Connecting to", apn);
  if (!modem.gprsConnect(apn, user, pass)) {
    delay(1000);
    return;
  }

  bool res = modem.isGprsConnected();
  DBG("GPRS status:", res ? "connected" : "not connected");

  printGPS();
  getValues();
  //Copied Code
    SerialMon.print(F("Connecting to "));
  SerialMon.print(server);
  if (!client.connect(server, port)) {
    SerialMon.println(" fail");
    delay(1000);
    continue;
  }
  SerialMon.println(" OK");
  // Make an HTTP GET request:
  client.print(String("GET /store?") + nh3+"&"+co+"&"+no2+"&"+ch4+"&"+temp+"&"+humid +"&"+latitude +"&"+longitude + " HTTP/1.0\r\n");
  client.print(String("Host: ") + server + "\r\n");
  client.print("Connection: close\r\n\r\n");

  unsigned long timeout = millis();
  while (client.connected() && millis() - timeout < 10000L) {
    // Print available data
    while (client.available()){
      char c = client.read();
      SerialMon.print(c);
      timeout = millis();
    }
  }
  SerialMon.println();

  // Shutdown
  client.stop();
  SerialMon.println(F("Server disconnected"));
delay(200);
}

  if (!modem.isGprsConnected()) {
    DBG("GPRS disconnected");
  } else {
    DBG("GPRS disconnect: Failed.");
  }
}


void getValues(){
    float c;

    c = gas.measure_NH3();
    //if(c>=0) Serial.print(c);
    nh3=String(c);
    //Serial.print(",");

    c = gas.measure_CO();
    //if(c>=0) Serial.print(c);
    co=String(c);
    //Serial.print(",");

    c = gas.measure_NO2();
    //if(c>=0) Serial.print(c);
    no2=String(c);
    //Serial.print(",");

    /*c = gas.measure_C3H8();
    if(c>=0) Serial.print(c);
    Serial.print(",");*/

    /*c = gas.measure_C4H10();
    if(c>=0) Serial.print(c);
    Serial.print(",");*/

    c = gas.measure_CH4();
    //if(c>=0) Serial.print(c);
    ch4=String(c);
    //Serial.print(",");

    /*c = gas.measure_H2();
    if(c>=0) Serial.print(c);
    Serial.print(",");*/

    /*c = gas.measure_C2H5OH();
    if(c>=0) Serial.print(c);
    Serial.print(",");*/
  //For DHT Sensor

  int chk = DHT.read11(7);
  humid=String(DHT.humidity);
  temp=String(DHT.temperature);

  humid2=String(DHT.humidity);
  temp2=String(DHT.temperature);

  Serial.println("Humid1: ");
  Serial.println(humid);

  Serial.println("Temp1: ");
  Serial.println(temp);

  Serial.println("Humid2: ");
  Serial.println(humid2);

  Serial.println("Temp2: ");
  Serial.println(temp2);
  
  delay(1000);
  }

 void printGPS(){
  printLat(gps.hdop.hdop(), gps.hdop.isValid(), 6, 1);
  printLat(gps.location.lat(), gps.location.isValid(), 11, 6);
  printLong(gps.location.lng(), gps.location.isValid(), 12, 6);
  SerialMon.println();

  smartDelay(1000);

  if (millis() > 5000 && gps.charsProcessed() < 10)
    SerialMon.println(F("No GPS data received: check wiring"));
  }

// This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}

static void printLat(float val, bool valid, int len, int prec)
{
  latitude="";
  if (!valid)
  {
    
    while (len-- > 1){
      latitude+="*";
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
    longitude+="*";
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
