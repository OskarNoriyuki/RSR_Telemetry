#ifndef __JOYSTICK_DRIVER_H__
#define __JOYSTICK_DRIVER_H__

#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "driver/gpio.h"

#define GPIO_JOYSTICK_Y     GPIO_NUM_4
#define GPIO_JOYSTICK_X     GPIO_NUM_15

#define CH_JOYSTICK_Y       ADC2_CHANNEL_0
#define CH_JOYSTICK_X       ADC2_CHANNEL_3

void init_Joystick();
int getAxisRaw_X();
int getAxisRaw_Y();
float getAxisVoltage_X();
float getAxisVoltage_Y();

#endif