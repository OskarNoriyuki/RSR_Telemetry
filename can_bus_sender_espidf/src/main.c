//C Libs
#include <stdio.h>
#include <string.h>
//FreeRTOS Libs
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
//Local Libs
#include "Drivers/led_driver.h"
#include "Drivers/joystick_driver.h"
#include "Drivers/Drivers_SPI.h"

static const char * TAG = "main";

//pisca leds
void ledTask(void *pvParameters){
    for(;;){
        /*
        vTaskDelay(1000/portTICK_PERIOD_MS);
        setGreenLed(ON);
        vTaskDelay(100/portTICK_PERIOD_MS);
        setGreenLed(OFF);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        setRedLed(ON);
        vTaskDelay(100/portTICK_PERIOD_MS);
        setRedLed(OFF);
        */
        vTaskDelay(50/portTICK_PERIOD_MS);
        setBlueLed(ON);
        vTaskDelay(50/portTICK_PERIOD_MS);
        setBlueLed(OFF);
    }
}

//print dos valores lidos nos eixos do joystick
void joystickTask(void *pvParameters){
    for(;;){
        //printf("valor bruto eixo X: %d, eixo Y: %d\n", getAxisRaw_X(), getAxisRaw_Y());
        printf("X: %.2fV, Y: %.2fV\n", getAxisVoltage_X(), getAxisVoltage_Y());
        vTaskDelay(200/portTICK_PERIOD_MS);
    }
}

void sensorTask(void *pvParameters){
    uint8_t rxBuf[1024];
    uint8_t txBuf[1024];

    uint16_t i;
    for(i = 0; i < 1024; i++)
        txBuf[i] = 0;
    
    for(i = 0; i < 256; i++){
        txBuf[2*i] = 0xFF;
        txBuf[2*i+1] = 0xFF;
    }

    for(;;){
        vTaskDelay(200/portTICK_PERIOD_MS);
        xSENT_Transmit(rxBuf, txBuf);
    }

}

void app_main() {
    //inicializa perifericos
    init_LEDS();

    xTimingSPI_t xTimingParams = {
        .u16InputDelay_ns = 10,
        .u16PostTrans_ns = 0,
        .u32ClkSpeed = 10000000,
        .u8ClkMode = 0
    };

    esp_err_t xRet = xSENT_Init(&xTimingParams);
    if(xRet != ESP_OK){
        ESP_LOGE(TAG, "Failed to init SENT over SPI");
    }

    xTaskCreate(&ledTask, "leds", 2*1024, NULL, 5, NULL);
    xTaskCreate(&sensorTask, "a1343", 8*1024, NULL, 5, NULL);
}