#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <LiquidCrystal.h>

//led pins
#define led1 6

//display macros
#define NUM_PARTS 4
#define displayPower A5

//display
LiquidCrystal lcd(9,10, 5, 4, 3, 2);
//display refresh global vars:
unsigned long lastDisplay_refresh = 0;
unsigned long display_period = 20;
int refreshPart = 0;
char printBuf[13];

//RADIO
RF24 radio(7,8); // CE, CSN
const byte addresses[][6] = {"00001", "00002"};
char rdata[32];

//PROTOTYPES
uint8_t DisplayRefreshPart(uint8_t part, char *message);
void DisplayRefreshRxText(char *radio_rx_buf);

void setup(){
  //led
  pinMode(led1, OUTPUT);

  //display control
  pinMode(displayPower, OUTPUT);
  digitalWrite(displayPower, LOW);
  delay(100);

  //radio
  radio.begin();
  radio.setAutoAck(false);
  radio.openWritingPipe(addresses[1]); // 00001
  radio.openReadingPipe(1, addresses[0]); // 00002
  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_1MBPS);
  radio.startListening();

  //lcd
  lcd.begin(24,2);

  //debug serial
  Serial.begin(115200);
}


void loop() {
  delay(10);

  //radio
  if (radio.available()) {
    while (radio.available()) {
        radio.read(&rdata, 32);
    }
  }

  //display
  if((millis() - lastDisplay_refresh)>display_period){
    DisplayRefreshRxText(rdata);
    lastDisplay_refresh = millis();
  }
}

void DisplayRefreshRxText(char *radio_rx_buf){
  //aux vars
  int i, updt_ini_ch, updt_len;

  //select rx buf part
  switch(refreshPart){
    case 0:{
      updt_ini_ch = 0;
      updt_len = 12;
      break;
    }
    case 1:{
      updt_ini_ch = 12;
      updt_len = 12;
      break;
    }
    case 2:{
      updt_ini_ch = 24;
      updt_len = 8;
      break;
    }
    case 3:{
      updt_ini_ch = 0;
      updt_len = 0;
      break;
    }
    default:
      break;
  }

  //copy rx buf part
  for(i = 0; i < updt_len; i++)
    printBuf[i] = radio_rx_buf[i + updt_ini_ch];
  printBuf[updt_len] = 0;

  //refresh
  DisplayRefreshPart(refreshPart, printBuf);

  //update index
  refreshPart++;
  if(refreshPart >= NUM_PARTS) refreshPart = 0;
}

uint8_t DisplayRefreshPart(uint8_t part, char *message){
  //aux variables
  uint8_t column, line;
  uint8_t ret = 0;

  //select display region
  switch(part){
    case 0:{
      column = 0;
      line = 0;
      ret = 1;
      break;
    }
    case 1:{
      column = 12;
      line = 0;
      ret = 1;
      break;
    }
    case 2:{
      column = 0;
      line = 1;
      ret = 1;
      break;
    }
    case 3:{
      column = 12;
      line = 1;
      ret = 1;
      break;
    }
    default:
      ret = 2;
    break;
  }

  //refresh
  lcd.setCursor(column, line);
  lcd.print("            ");
  lcd.setCursor(column, line);
  lcd.print(message);

  return ret;
}