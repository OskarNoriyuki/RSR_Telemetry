#include <Arduino.h>
#include <string.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define DEBUG_USART SerialUSB
#define PANEL_USART Serial2

#define LOOPTIME_MS 50
#define DEBUGTIME_MS 100

#define MODE_TRANSMITTER
//#define MODE_RECEIVER

RF24 radio(PB0, PA4); // CE, CSN
const byte addresses[][6] = {"00001", "00002"};

void setup() {
  //usarts
  DEBUG_USART.begin(115200);
  PANEL_USART.begin(115200);
  //led
  pinMode(PC13, OUTPUT);
  //radio
  radio.begin();
  #ifdef MODE_RECEIVER
  radio.openWritingPipe(addresses[1]); // 00001
  radio.openReadingPipe(1, addresses[0]); // 00002
  #else
  radio.openWritingPipe(addresses[0]); // 00002
  radio.openReadingPipe(1, addresses[1]); // 00001
  #endif
  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_1MBPS);
}

//radio
typedef struct estrutura_data{
  uint16_t pulse[8];
  byte data[16];
}data;
#ifdef MODE_RECEIVER
data rx_data;
#else
data tx_data;
#endif

//debug
unsigned long lastPrint = 0;

//BUFFER THINGS
#define RX_BUF_SIZE 100
#define RX_STARTING_CH    '{'
#define RX_ENDING_CH      '}'
char recvStr[RX_BUF_SIZE];
char lastMsgStr[RX_BUF_SIZE];
typedef enum{
  SM_BUF_EMPTY,
  SM_BUF_STARTED,
  SM_BUF_FINISHED,
  SM_BUF_OVERFLOW
}RX_BUF_STATE_T;
RX_BUF_STATE_T smRxBuf = SM_BUF_EMPTY;
int rxBufIndex = 0;
bool rxBufCollected = true;
bool rxBufReady = false;

unsigned long lastLoop = 0, timeNow;
void loop() {
  timeNow = millis();
  /* main task */
  if(timeNow - lastLoop > LOOPTIME_MS){
    lastLoop = timeNow;
    //********************** LOOP START **********************//

    /* debug print task */
    if(timeNow - lastPrint > DEBUGTIME_MS){
      lastPrint = timeNow;
      //debug start
      //Serial.println("Olar");
      if(rxBufReady){
        Serial.print("Message received: ");
        Serial.println(recvStr);
        rxBufCollected = true;
      }


      //debug end
    }

    char c;
    int breakW = 0;
    int auxCnt = 0;
    switch(smRxBuf){
      case SM_BUF_EMPTY:
        while(PANEL_USART.available() && breakW == 0){
          c = PANEL_USART.read();
          if(c == RX_STARTING_CH){
            smRxBuf = SM_BUF_STARTED;
            rxBufIndex = 0;
            breakW = 1;
          }
        }
        break;
      case SM_BUF_STARTED:
        while(PANEL_USART.available() && breakW == 0){
          c = PANEL_USART.read();
          if(c != RX_ENDING_CH){
            recvStr[rxBufIndex] = c;
            rxBufIndex++;
            if(rxBufIndex == RX_BUF_SIZE){
              smRxBuf = SM_BUF_OVERFLOW;
              breakW = 1;
            }
          }else{
            smRxBuf = SM_BUF_FINISHED;
            rxBufCollected = false;
            rxBufReady = true;
            recvStr[rxBufIndex + 1] = '\0';
            strcpy(lastMsgStr, recvStr);
            breakW = 1;
          }
        }
        break;
      case SM_BUF_FINISHED:
        if(rxBufCollected){
          smRxBuf = SM_BUF_EMPTY;
          rxBufReady = false;
        }
        break;
      case SM_BUF_OVERFLOW:
        DEBUG_USART.println("Error: RX2 overflow");
        smRxBuf = SM_BUF_EMPTY;
        break;
    }


    #ifdef MODE_RECEIVER
    //radio
    radio.startListening();
    if (radio.available()) {
      while (radio.available()) {
          radio.read(&rx_data, 32);
      }
    }
    //led
    if(rx_data.pulse[0] > 1500){
      digitalWrite(PC13, LOW);
    }else{
      digitalWrite(PC13, HIGH);
    }
    #else
    tx_data.pulse[0] = 55;
    tx_data.pulse[1]++;
    radio.stopListening();
    radio.write(&tx_data, sizeof(data));
    radio.flush_tx();
    #endif
    //*********************** LOOP END **********************//
  }


}