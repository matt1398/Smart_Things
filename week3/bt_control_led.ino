#define TXD D6
#define RXD D7
#define LED_1 D3
#define LED_2 D4
#define LED_3 D5

#include <SoftwareSerial.h>

SoftwareSerial bt(TXD, RXD);
int led_state[3] = {0, 0, 0};

void setup() {
  bt.begin(9600);
  Serial.begin(9600);
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
}

void loop() {
  if (bt.available()) {
    long num = bt.parseInt();
    if (num <= 3) {
      Serial.print("[LED ");
      Serial.print(num);
      led_state[num-1] ^= 0x01;
      Serial.print("] : ");
      Serial.println(led_state[num-1] == 0 ? "OFF" : "ON");
      switch (num) {
        case 1:
          digitalWrite(LED_1, led_state[num-1] == 0 ? LOW : HIGH);
          break;
        case 2:
          digitalWrite(LED_2, led_state[num-1] == 0 ? LOW : HIGH);
          break;
        case 3:
          digitalWrite(LED_3, led_state[num-1] == 0 ? LOW : HIGH);
          break;
      }
    }
  }
}
