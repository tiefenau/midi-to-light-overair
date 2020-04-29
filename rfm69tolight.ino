#include <RFM69.h>         
#include <RFM69_ATC.h>     
#include <SPI.h>      
#include <Adafruit_NeoPixel.h>  

#define NODEID      1
#define NETWORKID   100
#define FREQUENCY   RF69_868MHZ
#define PIN         6
#define NUMPIXELS   64

#define SERIAL_BAUD 115200

int CHANNELNO = 2;

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

RFM69 radio;
uint32_t colors[3] = {
  pixels.Color(255, 0, 0),
  pixels.Color(0, 255, 0),
  pixels.Color(0, 0, 255),
};

int lastcolor = 0;

typedef struct {
  int address;
  int cmd; 
  int value;   
} Payload;
Payload data;

void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(10);
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  radio.setHighPower(); //must include this only for RFM69HW/HCW!
  pinMode(9, OUTPUT);

  initializePixels();
  notifyChannel();
  
  Serial.println("Setup done");
}


void loop() {
  if (radio.receiveDone())
  {
    if (radio.DATALEN == sizeof(Payload))
    {
      data = *(Payload*)radio.DATA; //assume radio.DATA actually contains our struct and not something else
      
      if(data.address != CHANNELNO) {
        return;
      }
      
      switch(data.cmd){
        case 0:
          pixels.setBrightness(data.value*2);
          Serial.print("Setting brightness: ");
          Serial.println(data.value);
          break;
        case 1:
          lastcolor = data.value;
          Serial.print("Setting color: ");
          Serial.println(data.value);
          break;
      }
  
      for(int i=0; i<NUMPIXELS; i++) {
        pixels.setPixelColor(i, colors[lastcolor]);    
      }
      pixels.show();
    }
    Blink();
  }
  /*for(int i=0; i<NUMPIXELS; i++) {
     pixels.setPixelColor(i, colors[lastcolor++%2]);    
  }
  pixels.show();*/
}

void Blink()
{
  digitalWrite(9,HIGH);
  delay(3);
  digitalWrite(9,LOW);
}

void notifyChannel()
{
  for(int i=0;i<CHANNELNO/2;i++){
      digitalWrite(9,HIGH);
      delay(200);
      digitalWrite(9,LOW);
      delay(200);
  }
}


void initializePixels(){
  pixels.begin();
  pixels.clear(); 
  for(int i=0;i<CHANNELNO/2;i++){           
    pixels.setBrightness(50);
  
    for(int i=0; i<NUMPIXELS; i++) {
      pixels.setPixelColor(i, colors[lastcolor]);    
    }
    pixels.show();
    delay(200);
    
    pixels.setBrightness(0);
    pixels.show();// Turn OFF all pixels ASAP
    delay(200);
  }
}
