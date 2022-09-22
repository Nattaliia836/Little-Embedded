#include <SPI.h>
#include <Wire.h>
#include <SoftwareSerial.h>

// for SPI
long color = 250000000;
byte raw[4];

// for I2C
long color_read;

// for SoftwareSerial
SoftwareSerial mySerial(2, 3); // RX, TX

void setup() {
  Serial.begin(9600);
  SPI.begin();
  pinMode(SS, HIGH);
  Wire.begin();
  mySerial.begin(9600);
}

void loop() {
  digitalWrite(SS, LOW);
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));

  // число лонг
  SPI.transfer(0xAD); // початок исла типу лонг
  for (int j = 0; j < sizeof(color); j++) {
    byte b = color >> 8 * j;
    SPI.transfer(b);
  }
  SPI.transfer(0xAF);
  SPI.endTransaction();
  digitalWrite(SS, HIGH);
  
  Serial.print("SPI send : ");
  Serial.println(color);
  delay(1000);

  //========================================================================

  Wire.requestFrom(2, 6);

  for (byte i = 0; i < 4; i++)
  {
    raw[i] = Wire.read();
  }
  long &color1 = (long&)raw;
  Serial.print("I2C recived : ");
  Serial.println(color1);
  delay(1000);

  //========================================================================

  String message = String(color1);
  char Buf[50];
  message.toCharArray(Buf, 50);
  mySerial.write(Buf);
  Serial.print("SS send : ");
  Serial.println(Buf);
  Serial.println();



  //mySerial.write((char)color1);
  //Serial.println("SS");
  //Serial.println(color1);
  //Serial.println(message);

  delay(500);
}
