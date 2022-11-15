#ifndef LOGGER_RADIO_H
#define LOGGER_RADIO_H

/*********************** EXT LIB ***********************/
//radio
#include <RF24.h>
#include <nRF24L01.h>

//LoRa
#include <LoRa.h>

/*********************** LOCAL LIB ***********************/
#include "common.h"

/********************* DEFINITIONS *********************/
#define MODE_TRANSMITTER
//#define MODE_RECEIVER

//NRF24
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

//LoRa
//aux pins
#define ss 17
#define rst 16
#define dio0 2

/********************* DATA STRUCTS ********************/
typedef struct estrutura_data{
  uint16_t pulse[8];
  byte data[16];
}data;

/********************** PROTOTYPES *********************/
void TaskRadio(void * pvParameters);

#endif
