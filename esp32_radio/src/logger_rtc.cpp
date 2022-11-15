#include "logger_rtc.h"

static const char *tag = "rtc";

/*********************** PROTOTYPES ***********************/


/*********************** FUNCTIONS ***********************/
void set_date(byte dateCfg[7])
{
  //byte dateCfg[7] = {0,22,11,7,17,7,22}; 
  for(byte i = 0; i<7; i++){
    byte bval = dec2bcd(dateCfg[i]);
    write_rtc(i, &bval, 1);
  }

  //descomentar para setar o alarme do RTC
  //set_alarm1_each_min();
}

void set_alarm1_each_min(void)
{
  byte controlCfg = 5;
  write_rtc(RTC_CONTROL, &controlCfg, 1);
  byte setZero = 0;
  byte setMSB = A1M;
  write_rtc(ALM1_SECONDS, &setZero, 1);
  write_rtc(ALM1_MINUTES, &setMSB, 1);
  write_rtc(ALM1_HOURS, &setMSB, 1);
  write_rtc(ALM1_DAYDATE, &setMSB, 1);
}

void reset_alarm(void)
{
   byte statusConf = 0;
   write_rtc(RTC_STATUS, &statusConf, 1);
}

byte read_rtc(byte addr, byte *values, byte nBytes)
{
    Wire.beginTransmission(RTC_ADDR);
    Wire.write(addr);
    if ( byte e = Wire.endTransmission() ) return e;
    Wire.requestFrom( (uint8_t)RTC_ADDR, nBytes );
    for (byte i=0; i<nBytes; i++) values[i] = Wire.read();
    return 0;
}


byte write_rtc(byte addr, byte *values, byte nBytes)
{
    Wire.beginTransmission(RTC_ADDR);
    Wire.write(addr);
    for (byte i=0; i<nBytes; i++) Wire.write(values[i]);
    return Wire.endTransmission();  // Returns the I2C status (zero if successful).
}

void get_rtc(date *date_now)
{
  byte rtc_data[7];
  read_rtc(0,&rtc_data[0],7);
  date_now->secs = bcd2dec(rtc_data[0]);
  date_now->mins = bcd2dec(rtc_data[1]);
  date_now->hours = bcd2dec(rtc_data[2]);
  date_now->wdays = bcd2dec(rtc_data[3]);
  date_now->days = bcd2dec(rtc_data[4]);
  date_now->months = bcd2dec(rtc_data[5]);
  date_now->years = bcd2dec(rtc_data[6]);
}

void print_date(date *dateStruct)
{
   Serial.print(dateStruct->days);
   Serial.print("/");
   Serial.print(dateStruct->months);
   Serial.print("/");
   Serial.print(dateStruct->years);
   Serial.print("  ");
   Serial.print(dateStruct->hours);
   Serial.print(":");
   Serial.print(dateStruct->mins);
   Serial.print(":");
   Serial.print(dateStruct->secs);  
   Serial.print("\n");  
}

byte bcd2dec(byte val)
{
  byte dec_val = (val & 0xF) + 10*((val & 0xF0)>>4);
  return dec_val;
}

byte dec2bcd(byte val)
{
  byte bcd_val = (val%10) | ((val/10)<<4);
  return bcd_val;
}
