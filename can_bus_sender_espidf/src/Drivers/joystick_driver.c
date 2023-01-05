#include "Drivers/joystick_driver.h"


void init_Joystick(){
    adc2_config_channel_atten(CH_JOYSTICK_X, ADC_ATTEN_11db);
    adc2_config_channel_atten(CH_JOYSTICK_Y, ADC_ATTEN_11db);
}

int getAxisRaw_X(){
    int raw = 0;
    adc2_get_raw(CH_JOYSTICK_X, ADC_WIDTH_12Bit, &raw);
    return raw;
}

int getAxisRaw_Y(){
    int raw = 0;
    adc2_get_raw(CH_JOYSTICK_Y, ADC_WIDTH_12Bit, &raw);
    return raw;
}

float getAxisVoltage_X(){
    int raw = 0;
    adc2_get_raw(CH_JOYSTICK_X, ADC_WIDTH_12Bit, &raw);
    esp_adc_cal_characteristics_t adc_chars = {};
    esp_adc_cal_characterize(ADC_UNIT_2, ADC_ATTEN_11db, ADC_WIDTH_12Bit, 1100, &adc_chars);
    uint32_t voltage_mv = esp_adc_cal_raw_to_voltage((uint32_t)raw, &adc_chars);
    float voltage = ((float)voltage_mv)/1000.0;
    return voltage;
}

float getAxisVoltage_Y(){
    int raw = 0;
    adc2_get_raw(CH_JOYSTICK_Y, ADC_WIDTH_12Bit, &raw);
    esp_adc_cal_characteristics_t adc_chars = {};
    esp_adc_cal_characterize(ADC_UNIT_2, ADC_ATTEN_11db, ADC_WIDTH_12Bit, 1100, &adc_chars);
    uint32_t voltage_mv = esp_adc_cal_raw_to_voltage((uint32_t)raw, &adc_chars);
    float voltage = ((float)voltage_mv)/1000.0;
    return voltage;
}