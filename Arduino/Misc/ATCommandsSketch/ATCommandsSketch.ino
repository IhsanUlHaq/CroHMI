
int pwrKey = 9;

void setup()
{
  // Open serial communications to computer
  Serial.begin(115200);
  Serial3.begin(115200); // Default for the board
   
  //Clear out any waiting serial data
  while (Serial3.available())
  {
    Serial3.read();
  }

  pinMode(pwrKey, OUTPUT);
  
  digitalWrite(pwrKey, HIGH);
  delay(3000);
  digitalWrite(pwrKey, LOW);
  
  
}

void loop(){

  
  
  /*
   * This loop just takes whatever comes in from the console and sends it to the board
   */
  if (Serial.available())
  {
    Serial3.write(Serial.read());
  }
  if (Serial3.available())
  {
    Serial.write(Serial3.read());
  }
}
