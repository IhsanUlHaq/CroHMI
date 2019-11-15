void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

struct loraValues{
  long a;
  float b;
  float c;
  float d;
  float e;
  float f;
} rgbObj;


void loop() {
  rgbObj.a = 1;
  rgbObj.b = 2.4;
  rgbObj.c = 3.845;
  rgbObj.d = 3.845;
  rgbObj.e = 3.845;
  rgbObj.f = 3.845;

  
  Serial.write((const uint8_t *) &rgbObj, sizeof(rgbObj));
  delay(10000);
  
}
