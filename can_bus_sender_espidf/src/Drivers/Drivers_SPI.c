/**
 * @file Drivers_SPI.c
 * @author ISI SIM (https://www.senairs.org.br/institutos/solucoes-integradas-em-metalmecanica)
 * @brief SPI driver source file
 * @version 1.0
 * @date 2022-06-13
 * @attention Written specifficaly for a half-duplex, 16+16 bit slave (ADS8688)
 */

#include "Drivers/Drivers_SPI.h"

/**************************** Variables *********************************/

static const char* TAG = "SPI_Driver";  /*!< SPI Driver Debug tag */
static bool bSPI_Started = false;       /*!< Flag that is set when the init function is called */
spi_device_handle_t xHandle;            /*!< Handle needed by IDF's SPI driver */

/**************************** Functions *********************************/

esp_err_t xSPIMasterInit(xTimingSPI_t *xTimingParams){
    esp_err_t xRet;
    uint32_t u32ClkPer = GET_CLK_PERIOD_NS(xTimingParams->u32ClkSpeed);
    ESP_LOGI(TAG, "Initializing HSPI bus...");

    /*   Verify previous initialization  */
    if(bSPI_Started){
        ESP_LOGW(TAG, "SPI Already configured!");
        return ESP_ERR_INVALID_STATE;
    }

    /*   Pin configuration of the SPI bus   */
    spi_bus_config_t buscfg={
        .mosi_io_num=   GPIO_MOSI,
        .miso_io_num=   GPIO_MISO,
        .sclk_io_num=   GPIO_SCLK,
        .quadwp_io_num= -1, //write protect signal not used
        .quadhd_io_num= -1  //hold signal not used
    };

    /*  Configuration of the slave SPI device  */
    spi_device_interface_config_t xDevCfg;
    memset(&xDevCfg, 0, sizeof(xDevCfg));
    //  Flags
    xDevCfg.flags |= SPI_DEVICE_HALFDUPLEX;
    //  Length
    xDevCfg.command_bits = SPI_T_CMD_LENTH;
    //  Clock Speed     
    if( xTimingParams->u32ClkSpeed < SPI_MIN_CLK_SPEED){
        ESP_LOGW(TAG, "Invalid Clock Speed, starting at min freq = %d", SPI_MIN_CLK_SPEED);
        xTimingParams->u32ClkSpeed = SPI_MIN_CLK_SPEED;
    }else if(xTimingParams->u32ClkSpeed > SPI_MAX_CLK_SPEED){
        ESP_LOGW(TAG, "Invalid Clock Speed, starting at max freq = %d", SPI_MAX_CLK_SPEED);
        xTimingParams->u32ClkSpeed = SPI_MAX_CLK_SPEED;
    }
    xDevCfg.clock_speed_hz = xTimingParams->u32ClkSpeed;
    //  Mode
    if( xTimingParams->u8ClkMode > SPI_LAST_MODE){
        ESP_LOGW(TAG, "Invalid spi mode, starting at default mode = %d", SPI_DEFAULT_MODE);
        xTimingParams->u8ClkMode = SPI_DEFAULT_MODE;
    }
    xDevCfg.mode = xTimingParams->u8ClkMode;
    xDevCfg.spics_io_num = GPIO_CS;
    //  Input Delay (delay between sclk and miso)
    xDevCfg.input_delay_ns = xTimingParams->u16InputDelay_ns;
    //  Post-Transfer delay (keep the CS low for a period after the transaction)
    if(xTimingParams->u16PostTrans_ns == 0)
        xDevCfg.cs_ena_posttrans = 0;
    else
        xDevCfg.cs_ena_posttrans = (xTimingParams->u16PostTrans_ns / u32ClkPer) + 1;
    //  Queue
    xDevCfg.queue_size  = SPI_QUEUE_SIZE;

    /*  Initialize the SPI bus  */
    xRet = spi_bus_initialize(SPI_MASTER_HOST, &buscfg, SPI_DMA_ENABLE? SPI_DMA_CH_AUTO : SPI_DMA_DISABLED);
    if(xRet != ESP_OK) return xRet;

    /*  Add device  */
    xRet = spi_bus_add_device(SPI_MASTER_HOST, &xDevCfg, &xHandle);
    if(xRet != ESP_OK) return xRet;
    xRet = spi_device_acquire_bus(xHandle, portMAX_DELAY);
    if(xRet != ESP_OK) return xRet;
    
    /*  sinalize  */
    bSPI_Started = true;
    return xRet;
}


esp_err_t xSENT_Init(xTimingSPI_t *xTimingParams){
    esp_err_t xRet;
    uint32_t u32ClkPer = GET_CLK_PERIOD_NS(xTimingParams->u32ClkSpeed);
    ESP_LOGI(TAG, "Initializing SENT interface over SPI...");

    /*   Verify previous initialization  */
    if(bSPI_Started){
        ESP_LOGE(TAG, "SPI is already set");
        return ESP_ERR_INVALID_STATE;
    }

    /*   Pin configuration of the SPI bus   */
    spi_bus_config_t buscfg={
        .mosi_io_num=   GPIO_MOSI,
        .miso_io_num=   GPIO_MISO,
        .sclk_io_num=   GPIO_SCLK,
        .quadwp_io_num= -1, //write protect signal not used
        .quadhd_io_num= -1  //hold signal not used
    };

    /*  Configuration of the slave SPI device  */
    spi_device_interface_config_t xDevCfg;
    memset(&xDevCfg, 0, sizeof(xDevCfg));
    //  Flags
    xDevCfg.flags |= SPI_DEVICE_NO_DUMMY;
    //  Clock Speed     
    xDevCfg.clock_speed_hz = xTimingParams->u32ClkSpeed;
    //  Mode
    if( xTimingParams->u8ClkMode > SPI_LAST_MODE){
        ESP_LOGW(TAG, "Invalid spi mode, starting at default mode = %d", SPI_DEFAULT_MODE);
        xTimingParams->u8ClkMode = SPI_DEFAULT_MODE;
    }
    xDevCfg.mode = xTimingParams->u8ClkMode;
    //CS pin
    xDevCfg.spics_io_num = 27;
    //  Input Delay (delay between sclk and miso)
    xDevCfg.input_delay_ns = xTimingParams->u16InputDelay_ns;
    //  Post-Transfer delay (keep the CS low for a period after the transaction)
    if(xTimingParams->u16PostTrans_ns == 0)
        xDevCfg.cs_ena_posttrans = 0;
    else
        xDevCfg.cs_ena_posttrans = (xTimingParams->u16PostTrans_ns / u32ClkPer) + 1;
    //  Queue
    xDevCfg.queue_size  = SPI_QUEUE_SIZE;

    /*  Initialize the SPI bus  */
    xRet = spi_bus_initialize(SPI_MASTER_HOST, &buscfg, SPI_DMA_CH_AUTO); // DMA on
    if(xRet != ESP_OK) return xRet;

    /*  Add device  */
    xRet = spi_bus_add_device(SPI_MASTER_HOST, &xDevCfg, &xHandle);
    if(xRet != ESP_OK) return xRet;
    xRet = spi_device_acquire_bus(xHandle, portMAX_DELAY);
    if(xRet != ESP_OK) return xRet;
    
    /*  sinalize  */
    bSPI_Started = true;
    return xRet;
}

esp_err_t xSENT_Transmit(uint8_t *u8RxData, uint8_t *u8TxData){
    if(!bSPI_Started){
        ESP_LOGE(TAG, "Cannot perform transactions - SPI not initialized!");
        return ESP_FAIL;
    }
    esp_err_t xRet = ESP_OK;

    //  fill transaction config struct
    spi_transaction_t xTrConf;
    memset(&xTrConf, 0, sizeof(xTrConf));

    // transaction size
    xTrConf.rxlength = 1024*8;
    xTrConf.length = 1024*8;

    // buffer
    xTrConf.rx_buffer = u8RxData;
    xTrConf.tx_buffer = u8TxData;

    //  transmit
    xRet = spi_device_polling_transmit(xHandle, &xTrConf);

    return xRet;
}

esp_err_t xHalfDuplexTransmit_32(uint16_t * u16RxData, uint16_t * u16TxData){
    if(!bSPI_Started){
        ESP_LOGE(TAG, "Cannot perform transactions - SPI not initialized!");
        return ESP_FAIL;
    }
    esp_err_t xRet = ESP_OK;
    spi_transaction_t xTrConf;
    //  fill transaction config struct
    memset(&xTrConf, 0, sizeof(xTrConf));
    xTrConf.rxlength = SPI_T_DATA_LENTH;
    //  uses command phase instead of TXDATA, to avoid an issue of spi_master driver
    xTrConf.cmd = *u16TxData;
    //  since we have a short 32b transaction, store its content directly in the transaction struct
    xTrConf.flags = SPI_TRANS_USE_RXDATA;
    //  transmit
    if(SPI_POLLING)
        xRet = spi_device_polling_transmit(xHandle, &xTrConf);
    else
        xRet=spi_device_transmit(xHandle, &xTrConf);
    //  save last 2 bytes from 32b rx_data into the 16b argument
    *u16RxData = (xTrConf.rx_data[0] << 8 | xTrConf.rx_data[1]);
    return xRet;
}

bool bIsSpiConfigured(void){
    return bSPI_Started;
}