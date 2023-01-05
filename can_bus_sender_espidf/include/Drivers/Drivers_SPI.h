/**
 * @file Drivers_SPI.h
 * @author ISI SIM (https://www.senairs.org.br/institutos/solucoes-integradas-em-metalmecanica)
 * @brief Sets ESP32's HSPI interface. Performs basic transactions to/from a half-duplex SPI slave.
 * @version 1.0
 * @date 2022-06-13
 * @attention Written specifficaly for a half-duplex, 16+16 bit slave (ADS8688)
 */

#ifndef __DRIVERS_SPI_H__
#define __DRIVERS_SPI_H__

/******************** C LIB *********************/
#include <string.h>
#include <stdlib.h>

/******************** ESP LIB *********************/
#include "esp_log.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

/******************** DEFINES & TYPEDEFS *********************/
#define SPI_MASTER_HOST         HSPI_HOST       /*!< Hardware SPI peripheral*/ 
#define GPIO_MOSI               GPIO_NUM_13     /*!< Default MOSI pin for HSPI*/ 
#define GPIO_MISO               GPIO_NUM_12     /*!< Default MISO pin for HSPI*/
#define GPIO_SCLK               GPIO_NUM_14     /*!< Default SCLK pin for HSPI*/
#define GPIO_CS                 GPIO_NUM_15     /*!< Default CS pin for HSPI*/
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

/**
 * @brief Struct that contains the SPI bus timing parameters.
 * All parameters are verified inside xSPIMasterInit() function,
 * which will operate with default values when this struct is initialized with {0}.
 */       
typedef struct
{
    uint32_t u32ClkSpeed;                       /*!< SPI Clock (Hz) */
    uint16_t u16InputDelay_ns;                  /*!< delay between sclk and miso */
    uint16_t u16PostTrans_ns;                   /*!< extra time before CS rise  */
    uint8_t u8ClkMode;                          /*!< clock polarity and phase  */
} xTimingSPI_t;

/******************** MACROS & FUNCTIONS *********************/
#define GET_CLK_PERIOD_NS(clkFreq) 1000000000/clkFreq       /*!< From the defined clock frequency */

/**
 * @brief initialize SPI master peripheral
 * 
 * The SPI conf elements are verified by this function.
 * 
 * An empty struct will cause the SPI to init with the default values (and minimum clk freq.).
 * 
 * @param xTimingParams timing constraints required by spi slave
 * @return esp_err_t 
 *         - ESP_ERR_INVALID_ARG   if configuration is invalid
 *         - ESP_ERR_INVALID_STATE if host already is in use
 *         - ESP_ERR_NOT_FOUND     if there is no DMA channels available
 *         - ESP_ERR_NO_MEM        if out of memory
 *         - ESP_OK                on success
 */
esp_err_t xSPIMasterInit(xTimingSPI_t *xTimingParams);

/**
 * @brief Function responsible for a 16+16 bit length, half-duplex transmission
 * 
 *        -First 16 bits: master sends TxData (E.g. Command to the ADC). MISO is irrelevant.
 * 
 *        -Last 16 bits: capture data from MISO line (E.g. ADC current sample).
 * 
 *        This function waits for the transaction to complete before returning.
 * 
 * @param[out] u16RxData word to be received from slave
 * @param[in] u16TxData word to be sent to slave
 * @return esp_err_t 
 *         - ESP_ERR_INVALID_ARG   if configuration is invalid
 *         - ESP_FAIL              if the SPI was not initialized
 *         - ESP_OK                on success
 */
esp_err_t xHalfDuplexTransmit_32(uint16_t *u16RxData, uint16_t *u16TxData);

/**
 * @brief spi status getter
 * 
 * @return TRUE if spi has been configured
 * @return FALSE if spi hasn't been configured
 */
bool bIsSpiConfigured(void);

esp_err_t xSENT_Init(xTimingSPI_t *xTimingParams);
esp_err_t xSENT_Transmit(uint8_t *u8RxData, uint8_t *u8TxData);



#endif