#define TXD D6
#define RXD D7

#include <SoftwareSerial.h>

SoftwareSerial bt(TXD, RXD);

void setup() {
  Serial.begin(9600);
  bt.begin(9600);
}

void loop() {
  if (Serial.available()) {
    bt.write(Serial.read());
  }
  if (bt.available()) {
    Serial.write(bt.read());
  }
}