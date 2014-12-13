#include <Enrf24.h>
#include <nRF24L01.h>
#include <string.h>
#include <SPI.h>


char pot1_str[4];
char pot2_str[4];
char joined_str[8];

Enrf24 radio(P2_0, P2_1, P2_2);  // P2.0=CE, P2.1=CSN, P2.2=IRQ
const uint8_t txaddr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0x01 };
void dump_radio_status_to_serialport(uint8_t);


void setup() {

  Serial.begin(9600);
  Serial.println("Running Setup...");
  
  pinMode(P1_1, INPUT);
  pinMode(P1_3, INPUT);
  
  // Initializes serial interface with radio
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);

  radio.begin();  // Defaults 1Mbps, channel 0, max TX power
  dump_radio_status_to_serialport(radio.radioState());
  radio.setTXaddress((void*)txaddr);
  
}

void loop() {
  
  Serial.print("Building packet: ");
  
  // Reads input values and stores the results in character arrays
  itoa(analogRead(P1_1), pot1_str, 10);
  itoa(analogRead(P1_3), pot2_str, 10);
  
  strcat(joined_str, pot1_str);
  strcat(joined_str, ",");
  strcat(joined_str, pot2_str);

  Serial.println(joined_str);
  
  // Transmission of data packet
  Serial.print("Sending packet...");
  radio.print(joined_str);
  radio.flush();
  dump_radio_status_to_serialport(radio.radioState());
  
  // Flush memory for next cycle
  Serial.println("Flushing data...");
  strcpy(pot1_str, "");
  strcpy(pot2_str, "");
  strcpy(joined_str, "");
  
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
