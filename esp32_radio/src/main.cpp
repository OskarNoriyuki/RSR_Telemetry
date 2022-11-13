#include <Arduino.h>
#include <string.h>
#include <SPI.h>
#include <LoRa.h>

#define ss 17
#define rst 16
#define dio0 2

//******************************** SDC ********************************
//#include "FS.h"
//#include "SD.h"
#define SD_PIN_MOSI  13
#define SD_PIN_MISO  27
#define SD_PIN_SCK   14
#define SD_PIN_SS    15
//SPIClass spiSD(HSPI);
#include "logger_sdc.h"
#include "logger_rtc.h"

//******************************* RADIO *******************************
#include <nRF24L01.h>
#include <RF24.h>
#define NRF_USE_HSPI

#if defined NRF_USE_HSPI
#define NRF_PIN_MOSI  MOSI        // According to arduino VSPI
#define NRF_PIN_MISO  MISO        // According to arduino VSPI
#define NRF_PIN_SCK   SCK         // According to arduino VSPI
#define NRF_PIN_SS    GPIO_NUM_0
#define NRF_PIN_CE    GPIO_NUM_4  // This pin features: PWM, ADC2_0, HSPI_HD, Touch0, RTC GPIO0
#else
#define NRF_PIN_MOSI  MOSI        // According to arduino VSPI
#define NRF_PIN_MISO  MISO        // According to arduino VSPI
#define NRF_PIN_SCK   SCK         // According to arduino VSPI
#define NRF_PIN_SS    GPIO_NUM_0
#define NRF_PIN_CE    GPIO_NUM_4  // This pin features: PWM, ADC2_0, HSPI_HD, Touch0, RTC GPIO0
#endif

#define MODE_TRANSMITTER
//#define MODE_RECEIVER
RF24 radio(NRF_PIN_CE, NRF_PIN_SS, 8000000); // CE, CSN
const byte addresses[][6] = {"00001", "00002"};

#define DEBUG_USART Serial
#define PANEL_USART Serial2

#define LOOPTIME_MS 50
#define DEBUGTIME_MS 100

SPIClass NRF_SPI(HSPI);

void setup() {
  //usarts
  DEBUG_USART.begin(115200);
  //PANEL_USART.begin(115200);
  //led
  pinMode(BUILTIN_LED, OUTPUT);

  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
  }else{
    Serial.println("Starting LoRa OK.");
  }
  LoRa.idle();

  //SDC
  init_sdcard();
  //radio
  //SPI.end();
  NRF_SPI.begin(14, 12, 13, 15);
  radio.begin(&NRF_SPI);
  #ifdef MODE_RECEIVER
  radio.openWritingPipe(addresses[1]); // 00001
  radio.openReadingPipe(1, addresses[0]); // 00002
  #else
  radio.openWritingPipe(addresses[0]); // 00002
  radio.openReadingPipe(1, addresses[1]); // 00001
  #endif
  radio.setPALevel(RF24_PA_MAX);
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
  // main task 
  if(timeNow - lastLoop > LOOPTIME_MS){
    lastLoop = timeNow;
    //------------------------ LOOP START ------------------------//

    // debug print task
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
    /*
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
    */

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
    //------------------------ LOOP END ------------------------//
  }


}

/*
// include the SD library:
#include <SPI.h>
#include <SD.h>
 
// WeMos D1 esp8266: D8 as standard
const int chipSelect = 2;
 
void printDirectory(File dir, int numTabs);
 
void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
 
 
  Serial.print("\nInitializing SD card...");
 
  // we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
  if (!SD.begin(SS)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card inserted?");
    Serial.println("* is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    while (1);
  } else {
    Serial.println("Wiring is correct and a card is present.");
  }
 
  // print the type of card
  Serial.println();
  Serial.print("Card type:         ");
  switch (SD.cardType()) {
    case CARD_NONE:
      Serial.println("NONE");
      break;
    case CARD_MMC:
      Serial.println("MMC");
      break;
    case CARD_SD:
      Serial.println("SD");
      break;
    case CARD_SDHC:
      Serial.println("SDHC");
      break;
    default:
      Serial.println("Unknown");
  }
 
  // print the type and size of the first FAT-type volume
//  uint32_t volumesize;
//  Serial.print("Volume type is:    FAT");
//  Serial.println(SDFS.usefatType(), DEC);
 
  Serial.print("Card size:  ");
  Serial.println((float)SD.cardSize()/1000);
 
  Serial.print("Total bytes: ");
  Serial.println(SD.totalBytes());
 
  Serial.print("Used bytes: ");
  Serial.println(SD.usedBytes());
 
  File dir =  SD.open("/");
  printDirectory(dir, 0);
 
}
 
void loop(void) {
}
 
void printDirectory(File dir, int numTabs) {
  while (true) {
 
    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.print(entry.size(), DEC);
      time_t lw = entry.getLastWrite();
      struct tm * tmstruct = localtime(&lw);
      Serial.printf("\tLAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
    }
    entry.close();
  }
}
*/