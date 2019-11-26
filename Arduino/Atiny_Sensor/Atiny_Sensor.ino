
#include <SoftwareSerial.h>                   //For Serial read write
#include <OneWire.h>                          //OneWire lets you access 1-wire devices made by Maxim/Dallas
#include <DallasTemperature.h>                //Temp Sensor
#include <EEPROM.h>                           // For saving data in flash

#define RX 4                                  
#define TX 3
#define buttonPin 0       // analog input pin to use as a digital input

#define ONE_WIRE_BUS 1

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire); 
SoftwareSerial SoftSerial(RX, TX);


struct Sensor_data{
  float soilMoisture;
  float soilTemperature;
};


Sensor_data data;
int value;
byte buff [sizeof(Sensor_data)];
const byte soilMoisturePin = A1;
float High_moisture = 0.0;
float Low_moisture = 0.0;
int eeAddress = 0;
int temp = 0;

//Button functions
int checkButton();
void clickEvent();
void doubleClickEvent();
void holdEvent(); 

int e = 0;
int debounce = 20;          // ms debounce period to prevent flickering when pressing or releasing the button
int DCgap = 250;            // max ms between clicks for a double click event
int holdTime = 1000;        // ms hold period: how long to wait for press+hold event
int longHoldTime = 3000;    // ms long hold period: how long to wait for press+hold event

// Button variables
boolean buttonVal = LOW;   // value read from button
boolean buttonLast = LOW;  // buffered value of the button's previous state
boolean DCwaiting = false;  // whether we're waiting for a double click (down)
boolean DConUp = false;     // whether to register a double click on next release, or whether to wait and click
boolean singleOK = true;    // whether it's OK to do a single click
long downTime = -1;         // time the button was pressed down
long upTime = -1;           // time the button was released
boolean ignoreUp = false;   // whether to ignore the button release because the click+hold was triggered
boolean waitForUp = false;        // when held, whether to wait for the up event
boolean holdEventPast = false;    // whether or not the hold event happened already


void setup() {
  // put your setup code here, to run once:
SoftSerial.begin(9600);
sensors.begin();
pinMode(buttonPin, INPUT);


EEPROM.get(eeAddress , High_moisture);
eeAddress += sizeof(float);
EEPROM.get(eeAddress , Low_moisture);
delay(3000);
memcpy (buff, &data, sizeof(data));
SoftSerial.write(buff,sizeof(buff));

}



void loop() {
  // put your main code here, to run repeatedly:
  
  if (SoftSerial.available()>0){
    value = SoftSerial.read(); 
    delay(100);
    if(value == 1){              //1 to read and send data
      sensors.requestTemperatures();
      
      temp = analogRead(soilMoisturePin);
      data.soilTemperature = sensors.getTempCByIndex(0);

      if (temp <= High_moisture){
        data.soilMoisture = 100;
      }else if (temp >= Low_moisture){
          data.soilMoisture = 0.0;
        }else{
        data.soilMoisture = 100*((temp - Low_moisture)/(High_moisture - Low_moisture));   //Two point Slope formula                ;
        }
      memcpy (buff, &data, sizeof(data));
      SoftSerial.write(buff,sizeof(buff));
      
      //SoftSerial.println(data.soilMoisture);
     // SoftSerial.println(data.soilTemperature);
     
     value = -1 ;
    }


    if (value == 9){         //9 to send High Moisture Value
    // In setup Mode now
    eeAddress = 0;
    
    // Put sensor in water

    High_moisture = analogRead(soilMoisturePin);

    //Send Data
    data.soilMoisture = High_moisture ;

    memcpy (buff, &data, sizeof(data));
    SoftSerial.write(buff,sizeof(buff));

    EEPROM.put(eeAddress , High_moisture);

    value = -1 ;
    }
    
    if(value == 0){            //0 to set low moisture value
      eeAddress = 0;
      eeAddress += sizeof(float);
    //Put sensor in dry Soil
      Low_moisture = analogRead(soilMoisturePin);

  // Send data 
    data.soilMoisture = Low_moisture ;

    memcpy (buff, &data, sizeof(data));
    SoftSerial.write(buff,sizeof(buff));

    EEPROM.put(eeAddress , Low_moisture);
    value = -1 ;
    }

  }
  e = checkButton();
  if (e == 1) clickEvent();
  if (e == 2) doubleClickEvent();
  if (e == 3) holdEvent();
  e = 0;
   
  
  delay(100);
}

void clickEvent() {
   sensors.requestTemperatures();
      
      temp = analogRead(soilMoisturePin);
      data.soilTemperature = sensors.getTempCByIndex(0);

      if (temp <= High_moisture){
        data.soilMoisture = 100;
      }else if (temp >= Low_moisture){
          data.soilMoisture = 0.0;
        }else{
        data.soilMoisture =100*((temp - Low_moisture)/(High_moisture - Low_moisture))                ;
        }
      memcpy (buff, &data, sizeof(data));
      SoftSerial.write(buff,sizeof(buff));
   
}  
void doubleClickEvent() {
    eeAddress = 0;
      eeAddress += sizeof(float);
    //Put sensor in dry Soil
      Low_moisture = analogRead(soilMoisturePin);

  // Send data 
    data.soilMoisture = Low_moisture ;

    memcpy (buff, &data, sizeof(data));
    SoftSerial.write(buff,sizeof(buff));

    EEPROM.put(eeAddress , Low_moisture);
    value = 0 ;
} 
void holdEvent() {
   eeAddress = 0;
    
    // Put sensor in water

    High_moisture = analogRead(soilMoisturePin);

    //Send Data
    data.soilMoisture = High_moisture ;

    memcpy (buff, &data, sizeof(data));
    SoftSerial.write(buff,sizeof(buff));

    EEPROM.put(eeAddress , High_moisture);

}


int checkButton() {    
   int event = 0;
   buttonVal = digitalRead(buttonPin);
   
   // Button pressed down
   if (buttonVal == HIGH && buttonLast == LOW  && (millis() - upTime) > debounce)
   {
       downTime = millis();
       ignoreUp = false;
       waitForUp = false;
       singleOK = true;
       holdEventPast = false;
      
       if ((millis()-upTime) < DCgap && DConUp == false && DCwaiting == true)  DConUp = true;
       else  DConUp = false;
       DCwaiting = false;
   }
   // Button released
   else if (buttonVal == LOW && buttonLast == HIGH && (millis() - downTime) > debounce)
   {        
       if (not ignoreUp)
       {
           upTime = millis();
           if (DConUp == false) DCwaiting = true;
           else
           {
               event = 2;
               DConUp = false;
               DCwaiting = false;
               singleOK = false;
           }
       }
   }
   // Test for normal click event: DCgap expired
   if ( buttonVal == LOW && (millis()-upTime) >= DCgap && DCwaiting == true && DConUp == false && singleOK == true && event != 2)
   {
       event = 1;
       DCwaiting = false;
   }
   // Test for hold
   if (buttonVal == HIGH && (millis() - downTime) >= holdTime) {
       // Trigger "normal" hold
       if (not holdEventPast)
       {
           event = 3;
           waitForUp = true;
           ignoreUp = true;
           DConUp = false;
           DCwaiting = false;
           //downTime = millis();
           holdEventPast = true;
       }
        
   }
   buttonLast = buttonVal;
   return event;
}
