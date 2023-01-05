#include "Drivers/led_driver.h"

void init_LEDS(void){
    gpio_config_t conf = {};
    conf.intr_type      =     GPIO_INTR_DISABLE;
    conf.mode           =     GPIO_MODE_OUTPUT;
    conf.pull_down_en   =     GPIO_PULLDOWN_DISABLE;
    conf.pull_up_en     =     GPIO_PULLUP_DISABLE;
    //set red led pin
    conf.pin_bit_mask   =     (1 << GPIO_LED_RED);
    gpio_config(&conf);
    //set green led pin
    conf.pin_bit_mask   =     (1 << GPIO_LED_GREEN);
    gpio_config(&conf);
    //set blue led pin
    conf.pin_bit_mask   =     (1 << GPIO_LED_BLUE);
    gpio_config(&conf);
}


void setRedLed(bool ledState){
    gpio_set_level(GPIO_LED_RED, (uint32_t)ledState);
}

void setGreenLed(bool ledState){
    gpio_set_level(GPIO_LED_GREEN, (uint32_t)ledState);
}

void setBlueLed(bool ledState){
    gpio_set_level(GPIO_LED_BLUE, (uint32_t)ledState);
}
