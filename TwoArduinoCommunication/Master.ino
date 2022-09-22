//Master

#include<SPI.h>
#include<Wire.h>
#include <SoftwareSerial.h>

long color = 255000000;
//int command = 1;
int val = 0; 

SoftwareSerial mySerial(3, 4); // RX, TX

void setup() {
  // put your setup code here, to run once:
  pinMode(A1, INPUT);
   
  Serial.begin(9600);
  SPI.begin();
  pinMode(SS, OUTPUT);
  pinMode(SS, HIGH);

  Wire.begin();

  mySerial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

  val = analogRead(A1); 

  digitalWrite(SS, LOW);
  SPI.beginTransaction( SPISettings( (val*100) , MSBFIRST, SPI_MODE0) );

  SPI.transfer(color); 
  

    digitalWrite(SS, HIGH);
    delay(500);

//===========================================================================

    Wire.requestFrom(2, 6);
    //long mesagge1;
    while(Wire.available()){
      char c = Wire.read();
      Serial.print(c);
    }
    delay(500);

//=============================================================================
  char cmd;
  mySerial.write(cmd);

}
