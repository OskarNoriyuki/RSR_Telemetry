#ifndef LOGGER_RTC_H
#define LOGGER_RTC_H

/*********************** EXT LIB ***********************/
#include <Arduino.h>
#include <Wire.h>


/********************* DEFINITIONS *********************/
#define RTC_SECONDS 0x00                        
#define RTC_MINUTES 0x01
#define RTC_HOURS 0x02
#define RTC_DAY 0x03
#define RTC_DATE 0x04
#define RTC_MONTH 0x05
#define RTC_YEAR 0x06
#define ALM1_SECONDS 0x07
#define ALM1_MINUTES 0x08
#define ALM1_HOURS 0x09
#define ALM1_DAYDATE 0x0A
#define ALM2_MINUTES 0x0B
#define ALM2_HOURS 0x0C
#define ALM2_DAYDATE 0x0D
#define RTC_CONTROL 0x0E
#define RTC_STATUS 0x0F
#define RTC_AGING 0x10
#define RTC_TEMP_MSB 0x11
#define RTC_TEMP_LSB 0x12

//control reg
#define BBSQW 0b01000000
#define INTCN 0b00000100
#define A2IE  0b00000010
#define A1IE  0b00000001

//alm2 reg
#define A2M   0b10000000

//alm1 reg
#define A1M   0b10000000

//ds3231 i2c address
#define RTC_ADDR 0x68

/********************* DATA STRUCTS *********************/
typedef struct{
  byte secs;
  byte mins;
  byte hours;
  byte wdays;
  byte days;
  byte months;
  byte years;
}date;

/********************** PROTOTYPES **********************/
void set_date(byte dateCfg[7]);
void set_alarm1_each_min(void);
void reset_alarm(void);

byte read_rtc(byte addr, byte *values, byte nBytes);
byte write_rtc(byte addr, byte *values, byte nBytes);
void get_rtc(date *date_now);

void print_date(date *dateStruct);
byte bcd2dec(byte val);
byte dec2bcd(byte val);

#endif
