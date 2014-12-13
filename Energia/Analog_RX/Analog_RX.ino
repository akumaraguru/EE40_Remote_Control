#include <Enrf24.h>
#include <nRF24L01.h>
#include <string.h>
#include <SPI.h>

Enrf24 radio(P2_0, P2_1, P2_2);
const uint8_t rxaddr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0x01 };
void dump_radio_status_to_serialport(uint8_t);


char* LStr;
char* RStr;
int LValue;
int RValue;
char inbuf[33];


void setup() {
  Serial.begin(9600);
  Serial.println("Running Setup...");
  
  pinMode(P1_2, OUTPUT);
  pinMode(P2_5, OUTPUT);

  // Initializes serial interface with radio
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  
  radio.begin();  // Defaults 1Mbps, channel 0, max TX power
  dump_radio_status_to_serialport(radio.radioState());

  radio.setRXaddress((void*)rxaddr);
  radio.enableRX();  // Start listening
}

void loop() {
  
  dump_radio_status_to_serialport(radio.radioState());  // Should show Receive Mode

  //  Loop freezes when radio not available
  while (!radio.available(true))
    ;
  if (radio.read(inbuf)) {
    Serial.print("Received packet: ");
    Serial.println(inbuf);
    
    //  String deconstruction, type conversion and value adjustment
    LStr = strtok (inbuf ,",");
    LValue = atoi(LStr);
    LValue = (2*LValue)/5;
    
    
    RStr = strtok (NULL, ",");
    RValue = atoi(RStr);
    RValue = (2*RValue)/5;
    
    // Calibration ratio: Modify this ratio to correct for variances between the two values
    RValue = (131*RValue)/102;
    
    // Sets deadzones and saturation zones
//    if (LValue>245){
//      LValue = 255;
//    }
//    if (RValue>245){
//      RValue = 255;
//    }
    if (LValue<15){
      LValue = 5;
    }
    if (RValue<15){
      RValue = 5;
    }    
    
    // Update PWM signals
    analogWrite(P1_2,LValue);
    analogWrite(P2_5,RValue);
    Serial.println(LValue);
    Serial.println(RValue);   

  }
}

void dump_radio_status_to_serialport(uint8_t status)
{
  Serial.print("Enrf24 radio transceiver status: ");
  switch (status) {
    case ENRF24_STATE_NOTPRESENT:
      Serial.println("NO TRANSCEIVER PRESENT");
      break;

    case ENRF24_STATE_DEEPSLEEP:
      Serial.println("DEEP SLEEP <1uA power consumption");
      break;

    case ENRF24_STATE_IDLE:
      Serial.println("IDLE module powered up w/ oscillators running");
      break;

    case ENRF24_STATE_PTX:
      Serial.println("Actively Transmitting");
      break;

    case ENRF24_STATE_PRX:
      Serial.println("Receive Mode");
      break;

    default:
      Serial.println("UNKNOWN STATUS CODE");
  }
}
