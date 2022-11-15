#ifndef LOGGER_SDC_H
#define LOGGER_SDC_H

/*********************** EXT LIB ***********************/
#include <SdFat.h>

/*********************** LOCAL LIB ***********************/
#include "common.h"
#include "logger_rtc.h"

/********************* DEFINITIONS *********************/
#define chipSelect      5     //SD SPI
#define FILE_BASE_NAME  "LogFile"
#define FILE_FORMAT     ".txt"

//#include "FS.h"
//#include "SD.h"
#define SD_PIN_MOSI  13
#define SD_PIN_MISO  27
#define SD_PIN_SCK   14
#define SD_PIN_SS    15
//SPIClass spiSD(HSPI);
  
/********************* DATA STRUCTS ********************/

typedef struct{
  float humidity;
  float current;
  float voltage;
  float temp_lm35;
  float temp_ntc;
  float temp_dht;
  float vbatt;
}logData_t;

/********************** PROTOTYPES *********************/
void init_sdcard();
void println_sdcard(date *timestamp, logData_t *sample);
void deinit_sdcard();

void TaskSDC(void *pvParameters);

#endif
