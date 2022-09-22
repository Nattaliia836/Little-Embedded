#include <Adafruit_NeoPixel.h> // подключаем библиотеку
#include <SPI.h>
#include <Wire.h>
#include <SoftwareSerial.h>

#define PIN  7              // указываем пин для подключения ленты
#define NUMPIXELS 8  // указываем количество светодиодов в ленте
// создаем объект strip с нужными характеристиками
Adafruit_NeoPixel strip (NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define BUFFER_SIZE 100
char buff[BUFFER_SIZE];
volatile uint8_t index = 0;
volatile bool data_ready = false;
long color;
volatile uint8_t int_index = 0;

//byte raw[4];
//long &color2 = (long&)val_1;
union mytype {
  long l;
  byte b[4];
};

SoftwareSerial mySerial(2, 3); // RX, TX
long color_ss;
char text_message;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  strip.begin();
  strip.setBrightness(50);  // яркость

  pinMode(MISO, OUTPUT);
  SPCR |= _BV(SPE);
  SPI.attachInterrupt();
  Wire.begin(2);
  Wire.onRequest(requestEvent);
}

ISR(SPI_STC_vect) {
  byte c = SPDR;
  if (index < sizeof(buff)) {
    buff[index++] = c;
    if (c == 0xAD) {
      int_index = index;
    }
    if (c == 0xAF) {
      data_ready = true;
    }
  }
}


void requestEvent() {
  mytype var;
  var.l = color;
  for (byte i = 0; i < 4; i++) {
    Wire.write(var.b[i]);
  }

  Serial.println("I2C");
  /*
    strip.clear();
    strip.setPixelColor(1, strip.Color(0, 0, 255));
    strip.setPixelColor(5, strip.Color(0, 255, 0));
    strip.setPixelColor(7, strip.Color(((var.l / 1000000) % 1000), ((var.l / 1000) % 1000), (var.l % 1000)));
    strip.show();

    Serial.println("Red is:");
    Serial.println((var.l / 1000000) % 1000);
    Serial.println("Green is:");
    Serial.println((var.l / 1000) % 1000);
    Serial.println("Blue is:");
    Serial.println(var.l % 1000);
    Serial.println();
  */
}

void loop() {
  strip.clear();
  // put your main code here, to run repeatedly:
  if (data_ready) {
    index = 0;
    color = 0;
    color = *((long*)(buff + int_index));
    data_ready = false;

    Serial.println("SPI");
    /*
        strip.clear();
        strip.setPixelColor(0, strip.Color(0, 0, 255));
        strip.setPixelColor(5, strip.Color(0, 255, 0));
        strip.setPixelColor(7, strip.Color(((color / 1000000) % 1000), ((color / 1000) % 1000), (color % 1000)));
        strip.show();
      /*
        Serial.println("Red is:");
        Serial.println((color / 1000000) % 1000);
        Serial.println("Green is:");
        Serial.println((color / 1000) % 1000);
        Serial.println("Blue is:");
        Serial.println(color % 1000);
        Serial.println();
    */
    delay(100);
  }

  //==========================================================================

  delay(100);


  //==========================================================================
  mySerial.listen();
  //Serial.println(mySerial.available());
  if (mySerial.available()) {
    Serial.println("SS");
    //text_message = mySerial.read();
    //mySerial.print(text_message);
    //Serial.println(text_message);
    //Serial.println(color_ss);
    char cmd;
    cmd = mySerial.read();


    //color_ss = mySerial.read();
    //Serial.println(color_ss);
    /*
      Serial.println("SS");
      strip.clear();
      strip.setPixelColor(2, strip.Color(0, 0, 255));
      strip.setPixelColor(5, strip.Color(0, 255, 0));
      strip.setPixelColor(7, strip.Color(((color_ss / 1000000) % 1000), ((color_ss / 1000) % 1000), (color_ss % 1000)));
      strip.show();
      delay(5000);
    */
  }


}
