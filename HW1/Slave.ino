//Slave

#include<SPI.h>
#include<Wire.h>
#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN  7              // указываем пин для подключения ленты
#define NUMPIXELS 8  // указываем количество светодиодов в ленте

// создаем объект strip с нужными характеристиками
Adafruit_NeoPixel strip (NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

volatile boolean received = false;
long color;
volatile byte Slavereceived;


SoftwareSerial mySerial(3, 4); // RX, TX
char cmd;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  strip.begin();                     // инициализируем ленту
  strip.setBrightness(150);  // указываем яркость светодиодов (максимум 255)
   
  pinMode(MISO,OUTPUT); 
  SPCR |= _BV(SPE);                 
  SPI.attachInterrupt(); 

  Wire.begin(2);
  Wire.onRequest(requestEvent);

  mySerial.begin(9600);
}

ISR (SPI_STC_vect)                   
{
  Slavereceived = SPDR;         // сохраняем значение принятое от master (ведущего) в переменной slavereceived
  received = true;                        //Sets received as True 
}

void loop() {
  // put your main code here, to run repeatedly:
  
  if(received){
    strip.setPixelColor(0, strip.Color(0, 0, 255));
    if (Slavereceived==1) 
      {
        strip.setPixelColor(5, strip.Color(0, 255, 0));
        Serial.println("Slave Slavereceived == 1");
      }else
      {
        strip.setPixelColor(5, strip.Color(255, 0, 0));
        Serial.println("Slave Slavereceived!=1");
      }
      strip.setPixelColor(7, strip.Color(150, 100, 20));
      strip.show();   // отправляем сигнал на ленту
  }
  strip.clear();

  if (mySerial.available()){
    strip.setPixelColor(2, strip.Color(0, 0, 255));
    strip.setPixelColor(5, strip.Color(0, 255, 0));
    strip.setPixelColor(7, strip.Color(150, 100, 20));
    cmd = mySerial.read();
    strip.show();
    delay(500);
  } 
  strip.clear();
}

void requestEvent(){
  strip.setPixelColor(1, strip.Color(0, 0, 255));
  strip.setPixelColor(5, strip.Color(0, 255, 0));
  strip.setPixelColor(7, strip.Color(150, 100, 20));
  Wire.write(Slavereceived);
  strip.show();
  delay(500);
  strip.clear();
}
