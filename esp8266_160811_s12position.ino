#include <ESP8266WiFi.h>
#include <stdint.h>
#include "esp8266_160602_udpTxToLogger.h"
#include <float.h>

extern "C" {
#include "user_interface.h"
}

/*
 * v0.9 2016 Sep. 18
 *   - add measureExcludingMinMax()
 * v0.8 2016 Sep. 18
 *   - outputToLogger() outputs [distance_cm]
 *   - add toDistance_cm()
 * v0.7 2016 Sep. 18
 *   - change 60msec delay to 120msec 
 * v0.6 2016 Sep. 3
 *   - add outputToLogger()
 *   - add [esp8266_160602_udpTxToLogger]
 * v0.5 2016 Sep. 3
 *   - obtain TOUT voltage
 *     + obtain ADvalue value
 *     + include "user_interface"
 * v0.4 2016 Sep. 3
 *   - remove debug pring for millis()
 *   - change 30msec delay to 60msec (about 2cm movement)
 *   - change 300msec delay to 30msec (about 1cm movement)
 *     + did not move
 * v0.3 2016 Sep. 3
 *   - measure time for delay(300);
 * v0.2 2016 Sep. 3
 *   - execute for every [kCount_interval] times 
 *     + add [kCount_interval]
 *   - include <stdint.h>
 * v0.1 2016 Jul. 11
 *   - handle relay pin
 *   - add loop()
 *   - add setup()
 */

static const int kRelayPin = 14;
static const int kCount_interval = 3;
static int s_totalCount = 0;
static const int kCount_average = 7;

void setup() {
  WiFi.disconnect();
  Serial.begin(115200);

  pinMode(kRelayPin, OUTPUT);
  digitalWrite(kRelayPin, LOW);

  WiFi_setup();
  WiFi_printConnectionInfo();
}

float toDistance_cm(float voltage)
{
  // for 50cm to 140cm
  // obtained from measurement on Sep. 18, 2016
  float res = -390.01 * voltage + 287.51;
  return res;
}

void outputToLogger(int idx_st1,float voltage) {
  char szbuf[200];
  int whl, frac; // whole and fractional parts
  int pos = 0;
  float dist_cm;

  dist_cm = toDistance_cm(voltage);

  whl = (int)voltage;
  frac = (int)(voltage * 1000) % 1000;
  pos = sprintf(szbuf, "%d,%d.%03d", idx_st1, whl, frac);

  whl = (int)dist_cm;
  frac = (int)(dist_cm * 100) % 100;
  pos = sprintf(&szbuf[pos], ",%d.%02d\r\n", whl, frac);
  
  WiFi_txMessage(szbuf);
}

float measureExcludingMinMax(void)
{
  float array[kCount_average];
  uint ADvalue;

  for(int loop=0; loop < kCount_average; loop++) {
    ADvalue = system_adc_read();
    array[loop] = ADvalue * 1.0 / 1024;
    delay(5); // msec
  }

  float fmin = FLT_MAX;
  float fmax = (- FLT_MAX);
  float fsum = 0.0;
  for(int idx=0; idx < kCount_average; idx++) {
    if (fmin > array[idx]) {
      fmin = array[idx];
    }
    if (fmax < array[idx]) {
      fmax = array[idx];
    }
    fsum += array[idx];
  }
  
  // excluding min and max for average
  fsum -= fmin;
  fsum -= fmax;  
  float voltage = fsum / (float)(kCount_average - 1 - 1); // 1:min, 1:max
  
  return voltage;
}

void loop() {
  char szbuf[] = "hello";
  static int8_t cnt = 0;
  unsigned long mils;

  Serial.println(szbuf);

  if (cnt == 0) {
    digitalWrite(kRelayPin, HIGH);
    delay(120); // msec
    digitalWrite(kRelayPin, LOW);
  } else if (cnt == 2) {
    float voltage = measureExcludingMinMax();
    s_totalCount++;
    outputToLogger(s_totalCount, voltage);
    Serial.print(voltage);
    Serial.println();
  }

  cnt++;
  if (cnt >= kCount_interval) {
    cnt =0;
  }
  delay(3000);
}
