#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <LiquidCrystal.h>
#define led1 6
#define numParts 2
#define displayPower A5

//car constants:
#define dirHyst 5    //ADC unit
#define pulseHystUp 10    //ADC unit
#define pulseHystDown 20    //ADC unit
#define escWait 50 //ms
#define steerCenter 1398
#define R_steerLim 1122
#define L_steerLim 1644
#define tamanhoMedia 7

//#define MODE_TRANSMITTER
#define MODE_RECEIVER

LiquidCrystal lcd(9,10, 5, 4, 3, 2);
RF24 radio(7,8); // CE, CSN
const byte addresses[][6] = {"00001", "00002"};

void setup(){
  pinMode(led1, OUTPUT);
  pinMode(displayPower, OUTPUT);
  digitalWrite(displayPower, LOW);
  delay(100);
  radio.begin();
  #ifdef MODE_TRANSMITTER
  radio.openWritingPipe(addresses[0]); // 00002
  radio.openReadingPipe(1, addresses[1]); // 00001
  #else
  radio.openWritingPipe(addresses[1]); // 00001
  radio.openReadingPipe(1, addresses[0]); // 00002
  #endif
  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_1MBPS);
  lcd.begin(24,2);
  Serial.begin(115200);
}
   
typedef struct estrutura_data{
  int pulse[4];
  byte data[16];
}data;
data rdata; // return
data tdata; // forward

unsigned int lastDisplay_refresh = 0;
int display_period = 20;
int refreshPart = 0;

//car vars:
unsigned long lastRev;
int read3v[20];
int soma_r3;
float media_r3;
int media_r3i;

void loop() {
      delay(10);
      //A0 vertical right, 
      //A1 horizontal right
      //A2 vertical left
      //A3 horizontal left
      //A6 pot
      /*
      center value:
      A0 - 496
      A1 - 517
      A2 - 523
      A3 - 505
      */
      int read1 = analogRead(A0); 
      int read2 = analogRead(A1); //servo
      int read3 = analogRead(A2); //esc
      int read4 = analogRead(A3);
      int read5 = analogRead(A6);
      
      //media movel
      for(int k = (tamanhoMedia-2); k >= 0; k--){
        read3v[k+1] = read3v[k];
      }
      read3v[0] = read3;
      soma_r3 = 0;
      for(int j = 0; j < tamanhoMedia; j++){
        soma_r3 += read3v[j];
      }
      float media_r3 = (float)soma_r3/tamanhoMedia;
      media_r3i = (int)media_r3;
      Serial.println(media_r3i);

      //controla ESC
      int adcT = media_r3i;
      switch(tdata.data[0]){
        case 0:
          if(adcT <= (525-dirHyst)){
            tdata.data[0] = 1;
            lastRev = millis();
          }
          if((millis() - lastRev) > escWait && adcT >= (525+pulseHystDown)){
            tdata.pulse[0] = map(adcT, (525+pulseHystDown), 1023, 1000, 2000);
          }else tdata.pulse[0] = 1000;
        break;
        case 1:
          if(adcT >= (525+dirHyst)){
            tdata.data[0] = 0;
            lastRev = millis();
          }
          if((millis() - lastRev) > escWait && adcT <= (525-pulseHystUp)){
            tdata.pulse[0] = map(adcT, (525-pulseHystUp) , 0, 1000, 2000);
          }else tdata.pulse[0] = 1000;
        break;
        default:
        tdata.pulse[0] = 1000;
        break;
      }
      
      if(read2 > 517)  tdata.pulse[1] = map(read2, 518, 1023, steerCenter, steerCenter - 300);
      if(read2 <= 517) tdata.pulse[1] = map(read2, 517, 0, steerCenter, steerCenter + 300);
      if(tdata.pulse[1] > L_steerLim) tdata.pulse[1] = L_steerLim;
      if(tdata.pulse[1] < R_steerLim) tdata.pulse[1] = R_steerLim;
      
      #ifdef MODE_TRANSMITTER
      radio.stopListening();
      radio.write(&tdata, sizeof(data));
      #else
      //radio
      radio.startListening();
      if (radio.available()) {
        while (radio.available()) {
            radio.read(&rdata, 32);
        }
      }
      #endif

      if((millis() - lastDisplay_refresh)>display_period){
        switch(refreshPart){
          case 0:
            #ifdef MODE_TRANSMITTER
             lcd.setCursor(0, 0);
             lcd.print("           ");
             lcd.setCursor(0, 0);
             lcd.print("esc: ");
             lcd.print(tdata.pulse[0]);

             lcd.setCursor(18, 0);
             lcd.print("      ");
             lcd.setCursor(18, 0);
             lcd.print("DIR:");
             lcd.print(tdata.data[0]);
             #else
             lcd.setCursor(0, 0);
             lcd.print("           ");
             lcd.setCursor(0, 0);
             lcd.print("pulse0: ");
             lcd.print(rdata.pulse[0]);
             #endif
          break;
          case 1:
             #ifdef MODE_TRANSMITTER
             lcd.setCursor(0, 1);
             lcd.print("           ");
             lcd.setCursor(0, 1);
             lcd.print("servo: ");
             lcd.print(tdata.pulse[1]);
             #else
             lcd.setCursor(0, 1);
             lcd.print("           ");
             lcd.setCursor(0, 1);
             lcd.print("pulse1: ");
             lcd.print(rdata.pulse[1]);
             #endif
          break;
          /*
           * 
           */
          default:
          break;
        }
        refreshPart++;
        if(refreshPart > (numParts - 1)) refreshPart = 0;
        lastDisplay_refresh = millis();
      }
}