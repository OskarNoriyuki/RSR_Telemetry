#include <Arduino.h>
#include <string.h>
#include <Wire.h>

// A1332 I2C address is 0x0C(12)
// A1332 #1 -> 0X0D
// A1332 #2 -> 0X0E
#define Addr 0x0C

#define ANGLE_DATA_FORMAT "{ANG=%04d}"
static const char *TEST_STRING = "{testando1234}";
char sendBuf[100] = {0};

//prototypes
static int fReadAngleRaw(void);
static float fReadAngle(void);

void setup() {
  Wire.begin();
  Serial.begin(115200);
  Serial2.begin(115200);
}

void loop() {
  int angleRaw = fReadAngleRaw();
  sprintf(sendBuf, ANGLE_DATA_FORMAT, angleRaw);
  //send string via USART1
  Serial.print("Enviando... \"");
  Serial.print(sendBuf);
  Serial.println("\"");

  //send string via USART2
  Serial2.print(sendBuf);

  //wait for tx buffer to flush
  Serial.flush();
  Serial2.flush();

  delay(400);
}

static int fReadAngleRaw(){
  unsigned int data[2];
  Wire.beginTransmission(Addr); // Start I2C Transmission 
  Wire.endTransmission();       // Stop I2C Transmission 
  Wire.requestFrom(Addr, 2);    // Request 2 byte of data
  delayMicroseconds(100);
  if(Wire.available() == 2){    // Read 2 bytes of data: raw_adc msb, raw_adc lsb 
    data[0] = Wire.read();
    data[1] = Wire.read();
  }
  // Convert the data to 12-bits
  int raw_angle = ((data[0] & 0x0F) * 256) + (data[1] & 0xFF);
  return raw_angle;
}

static float fReadAngle(){
  int raw_angle = fReadAngleRaw();
  float angle = (raw_angle * 360.0) / 4096.0;
  return angle;
}