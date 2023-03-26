#define TXD D6
#define RXD D7
#define SERVO D5
#define SERVO_DELAY 20

#include <SoftwareSerial.h>
#include <Servo.h>

SoftwareSerial bt(TXD, RXD);
Servo servo;
int current_angle = 0;


void setup() {
  servo.attach(SERVO);
  pinMode(SERVO, OUTPUT);
  bt.begin(9600);
  Serial.begin(9600);
  servo.write(current_angle);
}

void loop() {
  if (bt.available()) {
    long num = int(bt.parseInt() * 1.8);
    Serial.print("[SERVO MOVE] ");
    Serial.print(current_angle);
    Serial.print(" -> ");
    Serial.print(num);
    for (; current_angle != num; current_angle > num ? current_angle-- : current_angle++) {
      servo.write(current_angle);
      delay(SERVO_DELAY);
    } 
    Serial.println("..Done");

  }
}