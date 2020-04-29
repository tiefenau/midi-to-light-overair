#include <RFM69.h>       
#include <RFM69_ATC.h>     
#include <SPI.h>          
//*********************************************************************************************
//************ IMPORTANT SETTINGS - YOU MUST CHANGE/CONFIGURE TO FIT YOUR HARDWARE *************
//*********************************************************************************************
#define NODEID      99
#define NETWORKID   100
#define GATEWAYID   1
#define FREQUENCY   RF69_868MHZ

#define SERIAL_BAUD 115200 
#define MIDI_BAUD  31250 
#define BOARDPIN    9

#include <SoftwareSerial.h>

SoftwareSerial midiSerial(3, 2);

RFM69 radio;

int channel = 4; // midi channel
bool keepOpen = false;

byte note;
byte velocity;

byte sendSize=0;
boolean requestACK = false;

typedef struct {
  int           address; 
  int           cmd; 
  int           value;  
} Payload;
Payload data;

void setup() {
  Serial.begin(MIDI_BAUD);
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  radio.setHighPower(); //must include this only for RFM69HW/HCW!
  //radio.encrypt(ENCRYPTKEY); // For future Chris: needed? 
  pinMode(BOARDPIN, OUTPUT);
  midiSerial.begin(SERIAL_BAUD);
  midiSerial.println("Setup complete");
}

byte getNextByte(){
  while(Serial.available() <=0 ); 
  return Serial.read();
}

void loop() {
  if (Serial.available() > 0)
  {
    byte incomingByte = Serial.read();
    //midiSerial.println(incomingByte);
    if (incomingByte == (144 + (channel-1))){     //Note on on Channel x
      note = getNextByte();
      velocity = getNextByte();
  
      if (!keepOpen) keepOpen = note == 1; 
      
      sendCommand(note,velocity);
     // Serial.println("HammerOn: Note " + String(note) + " Vel " +String(velocity)+ " ko1:"+String(keepOpen));
    } else if (incomingByte == (128 + (channel-1))){ // note off message starting
      note = getNextByte();
      velocity = getNextByte();
      //Serial.println("HammerOff: Note " + String(note) + " Vel " +String(velocity)+ " ko1:"+String(keepOpen));
      if (!keepOpen) 
        //Serial.println("Killing Note");
        sendCommand(note,0);
      if (keepOpen && note == 1) keepOpen = false; 
    }else if (incomingByte== 208 + (channel-1)){ // aftertouch message starting
  
    }else if (incomingByte== 160 + (channel-1)){ // polypressure message starting
    }
  }
}

void sendCommand(byte note, byte value){
  data.address = note-note%2;//%2 == 1 ? note-1 : note;
  data.cmd = note%2;
  data.value = value;
  radio.send(GATEWAYID, (const void*)(&data), sizeof(data));
  Blink();
}

void Blink()
{
  digitalWrite(BOARDPIN,HIGH);
  delay(5);
  digitalWrite(BOARDPIN,LOW);
}
