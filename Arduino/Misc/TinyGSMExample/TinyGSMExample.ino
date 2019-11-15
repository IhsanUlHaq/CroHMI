/**************************************************************
 *
 * This sketch connects to a website and downloads a page.
 * It can be used to perform HTTP/RESTful API calls.
 *
 * For this example, you need to install ArduinoHttpClient library:
 *   https://github.com/arduino-libraries/ArduinoHttpClient
 *   or from http://librarymanager/all#ArduinoHttpClient
 *
 * TinyGSM Getting Started guide:
 *   https://tiny.cc/tinygsm-readme
 *
 * For more HTTP API examples, see ArduinoHttpClient library
 *
 **************************************************************/

// Select your modem:
// #define TINY_GSM_MODEM_SIM800
// #define TINY_GSM_MODEM_SIM808
// #define TINY_GSM_MODEM_SIM900
// #define TINY_GSM_MODEM_UBLOX
// #define TINY_GSM_MODEM_BG96
// #define TINY_GSM_MODEM_A6
#define TINY_GSM_MODEM_A7
// #define TINY_GSM_MODEM_M590
// #define TINY_GSM_MODEM_ESP8266

// Increase RX buffer if needed
//#define TINY_GSM_RX_BUFFER 512

#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>

// Uncomment this if you want to see all AT commands
//#define DUMP_AT_COMMANDS

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

// Use Hardware Serial on Mega, Leonardo, Micro
#define SerialAT Serial3

// or Software Serial on Uno, Nano
//#include <SoftwareSerial.h>
//SoftwareSerial SerialAT(2, 3); // RX, TX


// Your GPRS credentials
// Leave empty, if missing user or pass
const char apn[]  = "zongwap";
const char user[] = "";
const char pass[] = "";

String latitude = "200.20";
String longitude = "200.20";

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

// Server details
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

int pwrKey = 9;

void setup() {

  resource = resource + "&lat=" + latitude + "&lng=" + longitude + "&airm=" + airM + "&airt=" + airT +"&soilm1=" + soilM1 + "&soilt1=" + soilT1 + "&pn1=1" + "&soilm2=" + soilM2 + "&soilt2=" + soilT2 + "&pn2=2" + "&soilm3=" + soilM3 + "&soilt3=" + soilT3 + "&pn3=3" + "&soilm4=" + soilM4 + "&soilt4=" + soilT4 + "&pn4=4" + "&soilm5=" + soilM5 + "&soilt5=" + soilT5 + "&pn5=5";
  
  // Set console baud rate
  SerialMon.begin(115200);
  delay(10);

  pinMode(pwrKey, OUTPUT);

  delay(1000);
  digitalWrite(pwrKey, HIGH);
  delay(2500);
  digitalWrite(pwrKey, LOW);
  delay(1000);

  SerialMon.println(resource);
  
  // Set GSM module baud rate
  SerialAT.begin(115200);
  delay(3000);


  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  SerialMon.println(F("Initializing modem..."));
  modem.restart();

  String modemInfo = modem.getModemInfo();
  SerialMon.print(F("Modem: "));
  SerialMon.println(modemInfo);

  // Unlock your SIM card with a PIN
  //modem.simUnlock("1234");
}

void loop() {
  SerialMon.print(F("Waiting for network..."));
  if (!modem.waitForNetwork()) {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" OK");

  SerialMon.print(F("Connecting to "));
  SerialMon.print(apn);
  if (!modem.gprsConnect(apn, user, pass)) {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }
  SerialMon.println(" OK");

  SerialMon.print(F("Performing HTTP GET request... "));
  int err = http.get(resource);

  Serial.println("GET Successful");

  
  
  // Shutdown

  http.stop();
  SerialMon.println(F("Server disconnected"));

  modem.gprsDisconnect();
  SerialMon.println(F("GPRS disconnected"));

  
  
}
