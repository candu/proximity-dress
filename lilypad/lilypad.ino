#include <SoftwareSerial.h>
/*
 XBee Signal Strength Reader
 Context: Arduino (Lilypad)

 The hookups:
 
 XBee pin 6 -> Lilypad pin 7
 XBee RX    -> Lilypad 5
 XBee TX    -> Lilypad 6
 XBee +     -> Lilypad +
 XBee -     -> Lilypad -
 
 LED string -> one each on Lilypad 8, 9, 10
 
 for XBee:
 
 P0 -> 1   (enable RSSI/PWM)
 RP -> 14  (set how long the RSSI analog signal lasts to 2 seconds)
 
*/

int RSSI_PIN = 7;
int INNER_LED_PIN = 8;
int MIDDLE_LED_PIN = 9;
int OUTER_LED_PIN = 10;

int ledPins[] = {INNER_LED_PIN, MIDDLE_LED_PIN, OUTER_LED_PIN, -1};
int numPins = 0;

unsigned long signal_value = 0;

int MAX_SIGNAL_VALUE = 255;
int QUANTUM = 20;

SoftwareSerial XBee(5,6);

void setup() {
  Serial.begin(9600);
  XBee.begin(9600);
  
  pinMode(RSSI_PIN, INPUT);
  
  pinMode(INNER_LED_PIN, OUTPUT);
  pinMode(MIDDLE_LED_PIN, OUTPUT);
  pinMode(OUTER_LED_PIN, OUTPUT);
}

void loop() {
  if (XBee.available()) {
    Serial.println(XBee.read());
    
    unsigned long rssi_counts = pulseIn(RSSI_PIN, HIGH);
    signal_value = rssi_counts;
    lightThings();
  }
}

/* from led_test */
void lightThings() {
  float numPinsToLight = numPins * (signal_value / (1.0 * MAX_SIGNAL_VALUE));
  int numFullPins = (int)numPinsToLight;
  float lastPinBrightness = numPinsToLight - numFullPins;
  int i = 0;
  for (; i < numFullPins; i++) {
    digitalWrite(ledPins[i], HIGH);
  }
  if (lastPinBrightness > 0.0) {
    pwm(ledPins[i], lastPinBrightness);
    i++; 
  }
  for (; i < numPins; i++) {
    digitalWrite(ledPins[i], LOW); 
  }
}

void pwm(int pin, float brightness) {
  int pulseOn = max(1, (int)(QUANTUM * brightness));
  int pulseOff = QUANTUM - pulseOn;
  digitalWrite(pin, HIGH);
  delay(pulseOn);
  digitalWrite(pin, LOW);
  delay(pulseOff);
}

