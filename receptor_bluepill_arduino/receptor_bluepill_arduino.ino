#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(PB3,PA4); // CE, CSN
const byte addresses[][6] = {"00001", "00002"};

void setup() {
  //misc
  pinMode(PC13, OUTPUT);
  
  //serial
  Serial.begin(115200);

  //radio
  radio.begin();
  radio.openWritingPipe(addresses[1]); // 00001
  radio.openReadingPipe(1, addresses[0]); // 00002
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_1MBPS);

}
uint8_t rx_data[32];


void loop() {
 //radio
 delay(50);
 radio.startListening();
 if (radio.available()) {
   while (radio.available()) {
      radio.read(&rx_data, 32);
   }
   Serial.print(rx_data[0]);
   Serial.print(" ");
   Serial.print(rx_data[1]);
   Serial.print(" ");
   Serial.println(rx_data[2]);
 }
}
