#include "logger_sdc.h"

static const char *tag = "sd_card";

/*********************** PROTOTYPES **********************/
static void myItoa(int number, char *mystring);


/*********************** GLOBAL VARIABLES ***********************/
SdFat sdCard;
SdFile file;
SdFile root;
SdSpiArduinoDriver sdcSpi;
uint16_t SDinit = 1;
uint16_t fileCounter = 0;
char fileName[15];

//test
date date_now;
logData_t sample_now;
/*********************** FUNCTIONS ***********************/

void init_sdcard(){
  //SD Card initialization:
  Serial.print("Initializing SD card...");
  
  if(!sdCard.begin(chipSelect, SPI_DIV6_SPEED)){
    Serial.println("SD initialization failed!");
    SDinit = 0;
    //sdCard.initErrorHalt();
  }else{
    Serial.println("SD initialization done.");
  }
  if(SDinit){
      if (!root.open("/")) {
        sdCard.errorHalt("open root failed");
      }
      while (file.openNext(&root, O_RDONLY)) {
        file.close();
        fileCounter++;
      }
      if (root.getError()) {
        Serial.println("openNext failed");
      } else {
        Serial.println("file Count Successfull!");
      }

      char fileNum[4] = {'0','0','0','\0'};
      myItoa(fileCounter, &fileNum[0]);
      Serial.println(fileNum);
      char Name[15] = {'L','o','g','F','i','l','e',fileNum[0],fileNum[1],fileNum[2],'.','t','x','t','\0'};
      for(int i = 0; i<15; i++){
        fileName[i] = Name[i];
      }
      // Abre o arquivo.TXT
      if (!file.open(fileName, O_RDWR | O_CREAT | O_AT_END)){
        sdCard.errorHalt("Erro na abertura do arquivo!");
      }else{
        file.print("*****Arquivo #");
        file.print(fileNum[0]); file.print(fileNum[1]); file.print(fileNum[2]);
        file.print(" criado com sucesso*****\n");
        file.close();
        //To-Do: escreve metadados:        
      } 
  }
}

void println_sdcard(date *timestamp, logData_t *sample){
      file.open(fileName, O_RDWR | O_AT_END);
      file.print(timestamp->days);
      file.print("/");
      file.print(timestamp->months);
      file.print("/");
      file.print(timestamp->years);
      file.print("  ");
      file.print(timestamp->hours);
      file.print(":");
      file.print(timestamp->mins);
      file.print(":");
      file.print(timestamp->secs);  
      file.print(" Umidade: ");
      file.print(sample->humidity);
      file.print(" TempDHT: ");
      file.print(sample->temp_dht);
      /*
      file.print(" ADraw(Vch: ");
      file.print(sample->voltage_raw);
      file.print(", Ich: ");
      file.print(sample->current_raw);
      */
      file.print(") Conversão(");
      file.print(sample->voltage);
      file.print(" V, ");
      file.print(sample->current);
      file.print(" A)");
      file.print(" TempLM35: ");
      file.print(sample->temp_lm35);
      file.print(" TempNTC: ");
      file.print(sample->temp_ntc);
      file.print(" BattVoltage: ");
      file.print(sample->vbatt);
      file.print("\n");
      file.close();
}

void deinit_sdcard(){
  sdCard.end();
}

static void myItoa(int number, char *mystring) {
  for (int n=2; n>=0; n--) {
     if(number > 0) mystring[n] = number%10 + 48;
     number /= 10;
  }  
  //mystring[5]=0;
}

void TaskSDC(void *pvParameters){
  init_sdcard();
  vTaskDelay(100/portTICK_PERIOD_MS);

  for(;;){
    vTaskDelay(500/portTICK_PERIOD_MS);

    get_rtc(&date_now);
    print_date(&date_now); 
    //dummy data
    sample_now.current = 12.3;
    sample_now.humidity = 98.0;
    sample_now.temp_dht = 25.0;
    sample_now.temp_lm35 = 25.5;
    sample_now.temp_ntc = 26.0;
    sample_now.vbatt = 12.6;
    sample_now.voltage = 3.78;
    //save line
    println_sdcard(&date_now, &sample_now);
  }

}