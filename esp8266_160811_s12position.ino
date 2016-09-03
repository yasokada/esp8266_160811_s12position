#include <ESP8266WiFi.h>
#include <stdint.h>

/*
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

void setup() {
  WiFi.disconnect();
  Serial.begin(115200);

  pinMode(kRelayPin, OUTPUT);
  digitalWrite(kRelayPin, LOW);
}

void loop() {
  char szbuf[] = "hello";
  static int8_t cnt = 0;
  unsigned long mils;

  Serial.println(szbuf);

  if (cnt == 0) {
    digitalWrite(kRelayPin, HIGH);
    delay(60); // msec
    digitalWrite(kRelayPin, LOW);
  }

  cnt++;
  if (cnt >= kCount_interval) {
    cnt =0;
  }
  delay(3000);
}
