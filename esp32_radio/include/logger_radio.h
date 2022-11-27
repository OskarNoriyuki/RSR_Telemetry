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
#include "driver/spi_master.h"
#include "driver/gpio.h"

/********************* DEFINITIONS *********************/
#define MODE_TRANSMITTER
//#define MODE_RECEIVER

//NRF24
#define NRF_USE_HSPI

#if defined NRF_USE_HSPI
#define NRF_PIN_MOSI  27
#define NRF_PIN_MISO  14
#define NRF_PIN_SCK   12
#define NRF_PIN_SS    GPIO_NUM_26
#define NRF_PIN_CE    GPIO_NUM_13  // This pin features: PWM, ADC2_0, HSPI_HD, Touch0, RTC GPIO0
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


//////////SPI

/******************** DEFINES & TYPEDEFS *********************/
#define SPI_MASTER_HOST         HSPI_HOST       
#define GPIO_MOSI               NRF_PIN_MOSI   
#define GPIO_MISO               NRF_PIN_MISO    
#define GPIO_SCLK               NRF_PIN_SCK    
#define GPIO_CS                 NRF_PIN_SS    
//mode
#define SPI_DEFAULT_MODE        0               /*!< CPHA = 0, CPOL = 0*/ 
#define SPI_LAST_MODE           3               /*!< CPHA = 1, CPOL = 1*/ 

//speed
#define SPI_MIN_CLK_SPEED          100*1000     /*!< 100 kHz */
#define SPI_MAX_CLK_SPEED        17000*1000     /*!< 17 MHz, According to the ADS8688IDBT slave*/ 

//others
#define SPI_QUEUE_SIZE           1              /*!< Max number of unfinished transactions in queue */
#define SPI_T_CMD_LENTH         16              /*!< Command phase size (bits)*/
#define SPI_T_DATA_LENTH        16              /*!< Data phase size (bits)*/
#define SPI_T_FULL_LENGTH       32              /*!< Full transaction size (bits)*/
#define ESP32_INPUT_DELAY_NS    50              /*!< From ESP32 slave using IO_MUX pins */
#define SPI_SEMPH_TIMEOUT_MS    25              /*!< Enough time to do 256 transactions at 1MHz */
#define SPI_DMA_ENABLE       false              /*!< If true, DMA is responsible for the first data transfer*/
#define SPI_POLLING           true              /*!< If true, the processor polls for the end of transaction*/

typedef struct
{
    uint32_t u32ClkSpeed;                       /*!< SPI Clock (Hz) */
    uint16_t u16InputDelay_ns;                  /*!< delay between sclk and miso */
    uint16_t u16PostTrans_ns;                   /*!< extra time before CS rise  */
    uint8_t u8ClkMode;                          /*!< clock polarity and phase  */
} xTimingSPI_t;
#define GET_CLK_PERIOD_NS(clkFreq) 1000000000/clkFreq       /*!< From the defined clock frequency */


/********************* DATA STRUCTS ********************/
typedef struct estrutura_data{
  uint16_t pulse[8];
  byte data[16];
}data;

/********************** PROTOTYPES *********************/
void TaskRadio(void * pvParameters);

#endif
