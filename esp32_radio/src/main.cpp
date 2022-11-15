#include "common.h"
#include "logger_sdc.h"
#include "logger_rtc.h"
#include "logger_radio.h"
#include "logger_usart.h"

static const char *tag = "main";

TaskHandle_t HandleTask_NRF;
TaskHandle_t HandleTask_SDC;
TaskHandle_t HandleTask_USART;

#define LOOPTIME_MS 50
#define DEBUGTIME_MS 200

#define LOOPTIME_US (unsigned long)LOOPTIME_MS*1000
#define DEBUGTIME_US (unsigned long)DEBUGTIME_MS*1000

void setup() {
  //usarts
  DEBUG_USART.begin(115200);
  
  //led
  pinMode(BUILTIN_LED, OUTPUT);

  //RTC
  Wire.begin();
  //descomentar para setar o rtc. ordem dos bytes: sec-min-hrs-wdays-days-months-years
  byte dateCfg[7] = {00,1,23,1,13,11,22}; 
  //set_date(dateCfg);

  xTaskCreate(&TaskUSART, "USART", 10*1024, (void*)NULL, 4, &HandleTask_USART);
  xTaskCreate(&TaskRadio, "NRF24", 10*1024, (void*)NULL, 4, &HandleTask_NRF);
  xTaskCreate(&TaskSDC, "SD_CARD", 10*1024, (void*)NULL, 4, &HandleTask_SDC);
  xTaskCreate(&TaskDEBUG, "debug", 5*1024, (void*)NULL, 4, NULL);
}


void loop() {}