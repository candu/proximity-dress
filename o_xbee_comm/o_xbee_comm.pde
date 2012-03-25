#include <NewSoftSerial.h>

NewSoftSerial xbeeSerial(0, 1);
int ledState = LOW;
int ledIters = 50;

void setup() {
  Serial.begin(9600);
  xbeeSerial.begin(9600);
  pinMode(13, OUTPUT);
}

void updateLed() {
  if (ledIters == 50) {
    if (ledState == LOW) {
      ledState = HIGH; 
    } else {
      ledState = LOW; 
    }
    digitalWrite(13, ledState);
    ledIters = 0;
  }
  ledIters++;
}

void loop() {
  if (xbeeSerial.available()) {
      Serial.print((char)xbeeSerial.read());
  }
  if (Serial.available()) {
      xbeeSerial.print((char)Serial.read());
  }
  updateLed();
  delay(20);
}
