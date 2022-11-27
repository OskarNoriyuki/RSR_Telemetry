#include "logger_radio.h"
#include "esp32-hal-spi.h"

static const char *tag = "nrf";

RF24 radio(NRF_PIN_CE, NRF_PIN_SS, 4000000); // CE, CSN
const byte addresses[][6] = {"00001", "00002"};
SPIClass RADIO_SPI(HSPI);

//radio
#ifdef MODE_RECEIVER
data rx_data;
#else
char tx_data[32];
#endif

spi_device_handle_t xHnd; 
esp_err_t xSPIMasterInit(xTimingSPI_t *xTimingParams){
    //  Aux Variables
    esp_err_t xRet;
    uint32_t u32ClkPer = GET_CLK_PERIOD_NS(xTimingParams->u32ClkSpeed);
    
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
    xRet = spi_bus_add_device(SPI_MASTER_HOST, &xDevCfg, &xHnd);
    if(xRet != ESP_OK) return xRet;
    xRet = spi_device_acquire_bus(xHnd, portMAX_DELAY);
    if(xRet != ESP_OK) return xRet;

    return xRet;
}

void TaskRadio(void * pvParameters){

    RADIO_SPI.begin(NRF_PIN_SCK, NRF_PIN_MISO, NRF_PIN_MOSI, 32);

    //radio
    //SPI.end();
    radio.begin(&RADIO_SPI);
    radio.setAutoAck(false);

    #ifdef MODE_RECEIVER
    radio.openWritingPipe(addresses[1]); // 00001
    radio.openReadingPipe(1, addresses[0]); // 00002
    #else
    radio.openWritingPipe(addresses[0]); // 00002
    radio.openReadingPipe(1, addresses[1]); // 00001
    #endif
    radio.setPALevel(RF24_PA_MAX);
    radio.setDataRate(RF24_1MBPS);

    int counterTest = 0;

    for(;;){
        #ifdef MODE_RECEIVER
        //radio
        radio.startListening();
        if (radio.available()) {
        while (radio.available()) {
            radio.read(&rx_data, 32);
        }
        }
        //led
        if(rx_data.pulse[0] > 1500){
        digitalWrite(PC13, LOW);
        }else{
        digitalWrite(PC13, HIGH);
        }
        #else

        counterTest++;

        sprintf(tx_data, "aaargh! counter = oudri + %d", counterTest);

        radio.stopListening();
        radio.write(&tx_data, sizeof(data));
        radio.flush_tx();
        radio.txStandBy();
        #endif

        vTaskDelay(50/portTICK_PERIOD_MS);
    }
}