#ifndef LOGGER_SDC_H
#define LOGGER_SDC_H

/*********************** EXT LIB ***********************/
#include <Arduino.h>
#include <SPI.h>
#include <SdFat.h>

/*********************** LOCAL LIB ***********************/
#include "logger_rtc.h"

/********************* DEFINITIONS *********************/
#define chipSelect      5     //SD SPI
#define FILE_BASE_NAME  "LogFile"
#define FILE_FORMAT     ".txt"
  
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

#endif
