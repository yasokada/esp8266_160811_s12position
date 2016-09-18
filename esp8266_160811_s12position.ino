#include <ESP8266WiFi.h>
#include <stdint.h>
#include "esp8266_160602_udpTxToLogger.h"

extern "C" {
#include "user_interface.h"
}

/*
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

void setup() {
  WiFi.disconnect();
  Serial.begin(115200);

  pinMode(kRelayPin, OUTPUT);
  digitalWrite(kRelayPin, LOW);

  WiFi_setup();
  WiFi_printConnectionInfo();
}

void outputToLogger(int idx_st1,float val) {
  char szbuf[200];
  int whl, frac; // whole and fractional parts

  whl = (int)val;
  frac = (int)(val*100) % 100;
  sprintf(szbuf, "%d,%d.%02d\r\n", idx_st1, whl, frac);

  WiFi_txMessage(szbuf);
}

void loop() {
  char szbuf[] = "hello";
  static int8_t cnt = 0;
  unsigned long mils;

  Serial.println(szbuf);

  if (cnt == 0) {
    digitalWrite(kRelayPin, HIGH);
//    delay(60); // msec
    delay(120); // msec
    digitalWrite(kRelayPin, LOW);
  } else if (cnt == 2) {
    uint ADvalue;
    ADvalue = system_adc_read();
    float voltage = ADvalue * 1.0 / 1024;

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
