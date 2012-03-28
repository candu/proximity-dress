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
 RP -> 5   (set how long the RSSI analog signal lasts;; hex * 100ms)
 
*/

int RX_FROM_XBEE_PIN = 5;
int TX_TO_XBEE_PIN = 6;
int RSSI_PIN = 7;
int INNER_LED_PIN = 8;
int MIDDLE_LED_PIN = 9;
int OUTER_LED_PIN = 10;
int BOARD_LED_PIN = 13;

/**
 * The PWM cycle length is 8.32 ms, so we wait for approximately three
 * cycle lengths here. This avoids pulseIn() default behavior which is
 * to wait (and block!) for up to a second.
 */
int RSSI_TIMEOUT = 25000;   /* us */
int QUANTUM = 20000;        /* us */
int LOOP_QUANTUM = 500;     /* ms */

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

void executeLoop() {
  if (XBee.available() == 0) {
    return;
  }
  char c = char(XBee.read());
  if (c != '!') {
    return;
  }
  unsigned long rssiCounts = pulseIn(RSSI_PIN, HIGH, RSSI_TIMEOUT);
  Serial.print("[");
  Serial.print(millis());
  Serial.print("] ");
  Serial.println(rssiCounts);
  float curSignalValue = getSignalValue(rssiCounts);
  strength =
      STRENGTH_DECAY * strength +
      (1.0 - STRENGTH_DECAY) * curSignalValue;
}

void loop() {
  unsigned long start = millis();
  executeLoop();
  while (millis() - start < LOOP_QUANTUM) {
    lightThings(strength);
  }
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
