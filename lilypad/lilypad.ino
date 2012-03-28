#include <SoftwareSerial.h>
/*
 XBee Signal Strength Reader
 Context: Arduino (Lilypad)

 The hookups:
 
 XBee +     -> Lilypad +
 XBee -     -> Lilypad -
 XBee TX    -> Lilypad pin 5
 XBee RX    -> Lilypad pin 6
 XBee RSSI  -> Lilypad pin 7
 LED inner  -> Lilypad pin 8
 LED mid    -> Lilypad pin 9
 LED outer  -> Lilypad pin 10
 
 for XBee:
 
 P0 -> 1   (enable RSSI/PWM)
 RP -> 14  (set how long the RSSI analog signal lasts to 2 seconds)
 
*/

int RX_FROM_XBEE_PIN = 5;
int TX_TO_XBEE_PIN = 6;
int RSSI_PIN = 7;
int INNER_LED_PIN = 8;
int MIDDLE_LED_PIN = 9;
int OUTER_LED_PIN = 10;
int BOARD_LED_PIN = 13;

int RSSI_TIMEOUT = 200;     /* us */
int QUANTUM = 20000;        /* us */

int ledPins[] = {INNER_LED_PIN, MIDDLE_LED_PIN, OUTER_LED_PIN, -1};
int numPins = 0;

float strength = 0.0;
float STRENGTH_DECAY = 0.85;

SoftwareSerial XBee(RX_FROM_XBEE_PIN, TX_TO_XBEE_PIN);

void setup() {
  Serial.begin(9600);
  XBee.begin(9600);
  
  pinMode(RSSI_PIN, INPUT);
 
  for (numPins = 0; ledPins[numPins] != -1; numPins++) {
    pinMode(ledPins[numPins], OUTPUT);
  }

  pinMode(BOARD_LED_PIN, OUTPUT);
}

void loop() {
  if (!XBee.available()) {
    return;
  }
  char c = char(XBee.read());
  Serial.println(c);
  if (c != '!') {
    return;
  }
  unsigned long rssiCounts = pulseIn(RSSI_PIN, HIGH, RSSI_TIMEOUT);
  float curSignalValue = getSignalValue(rssiCounts);
  lightThings(curSignalValue);
}

/**
 * According to the Arduino docs, delayMicroseconds() is inaccurate for
 * large input values. We work around that by using delay() as well.
 */
void microDelay(int us) {
  delay(us / 1000);
  delayMicroseconds(us % 1000);
}

float getSignalValue(unsigned long rssiCounts) {
  // TODO: implement this
  return 0.666;
}

/**
 * This function lights up the pins listed in ledPins according to the
 * current signal strength. It will PWM exactly one of them, possibly at
 * zero brightness, to force a one-QUANTUM delay.
 */
void lightThings(float strength) {
  float numPinsToLight = numPins * strength;
  int numFullPins = int(numPinsToLight);
  float lastPinBrightness = numPinsToLight - numFullPins;
  int i = 0;
  for (; i < numFullPins; i++) {
    digitalWrite(ledPins[i], HIGH);
  }
  pwm(ledPins[i], lastPinBrightness);
  i++; 
  for (; i < numPins; i++) {
    digitalWrite(ledPins[i], LOW); 
  }
}

void pwm(int pin, float brightness) {
  int pulseOn = int(QUANTUM * brightness);
  int pulseOff = QUANTUM - pulseOn;
  digitalWrite(pin, HIGH);
  microDelay(pulseOn);
  digitalWrite(pin, LOW);
  microDelay(pulseOff);
}
