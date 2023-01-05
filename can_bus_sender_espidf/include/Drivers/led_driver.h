#ifndef __LED_DRIVER_H__
#define __LED_DRIVER_H__

#include "driver/gpio.h"

#define GPIO_LED_RED        GPIO_NUM_5
#define GPIO_LED_GREEN      GPIO_NUM_18
#define GPIO_LED_BLUE       GPIO_NUM_2

#define ON      true
#define OFF     false

void init_LEDS(void);
void setBlueLed(bool ledState);
void setGreenLed(bool ledState);
void setRedLed(bool ledState);

#endif