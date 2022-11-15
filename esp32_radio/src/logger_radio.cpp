#include "logger_radio.h"

static const char *tag = "nrf";

RF24 radio(NRF_PIN_CE, NRF_PIN_SS, 8000000); // CE, CSN
const byte addresses[][6] = {"00001", "00002"};
SPIClass RADIO_SPI(HSPI);

//radio
#ifdef MODE_RECEIVER
data rx_data;
#else
data tx_data;
#endif


void TaskRadio(void * pvParameters){

    RADIO_SPI.begin(14, 12, 13, 15);

    //LoRa
    LoRa.setSPI(RADIO_SPI);
    LoRa.setSPIFrequency(8000000);
    if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    }else{
    Serial.println("Starting LoRa OK.");
    }
    LoRa.idle();

    //radio
    //SPI.end();
    radio.begin(&RADIO_SPI);
    #ifdef MODE_RECEIVER
    radio.openWritingPipe(addresses[1]); // 00001
    radio.openReadingPipe(1, addresses[0]); // 00002
    #else
    radio.openWritingPipe(addresses[0]); // 00002
    radio.openReadingPipe(1, addresses[1]); // 00001
    #endif
    radio.setPALevel(RF24_PA_MAX);
    radio.setDataRate(RF24_1MBPS);

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

        tx_data.pulse[0] = 55;
        tx_data.pulse[1]++;
        radio.stopListening();
        radio.write(&tx_data, sizeof(data));
        radio.flush_tx();
        radio.txStandBy();

        LoRa.beginPacket();
        LoRa.print("hello ");
        LoRa.print((int)tx_data.pulse[1]);
        LoRa.endPacket();
        LoRa.idle();
        #endif

        vTaskDelay(50/portTICK_PERIOD_MS);
    }
}