/**
 * Proximity Dress - Lilypad
 *
 * @author candu (Evan Stratford)
 * @author valkyriesavage (Valkyrie Savage)
 *
 * Controls the Lilypad and XBee coordinator for Proximity Dress.
 */

#include <SoftwareSerial.h>

int RX_FROM_XBEE_PIN = 2;
int TX_TO_XBEE_PIN = 3;
int RSSI_PIN = 4;

/**
 * The PWM cycle length is 8.32 ms, so we wait for approximately three
 * cycle lengths here. This avoids pulseIn() default behavior which is
 * to wait (and block!) for up to a second.
 */
int RSSI_TIMEOUT = 25000;     /* us */
int RSSI_T_PWM = 8320;        /* us */
int QUANTUM = 20000;          /* us */
int LOOP_QUANTUM = 100;       /* ms */

float STRENGTH_DECAY = 0.85;  /* [0, 1] */

// The LEDs will be lit in this order; see lightThings() below.
int ledPins[] = {5, 6, 7, 8, 9, 10, 11, 12, 13, -1};

// Rolling averaged signal strength
float strength = 0.0;         /* [0, 1] */

// Inited in setup()
int numPins;
float mindBm;
float maxdBm;

/**
 * Note that (RX, TX) on the Lilypad MUST be connected to (TX, RX) on the
 * XBee respectively.
 */
SoftwareSerial XBee(RX_FROM_XBEE_PIN, TX_TO_XBEE_PIN);

void setup() {
  Serial.begin(9600);
  XBee.begin(9600);
  
  pinMode(RX_FROM_XBEE_PIN, INPUT);
  pinMode(TX_TO_XBEE_PIN, OUTPUT);
  pinMode(RSSI_PIN, INPUT);
  
  for (numPins = 0; ledPins[numPins] != -1; numPins++) {
    pinMode(ledPins[numPins], OUTPUT);
  }
  
  // NOTE: maximum duty cycle is actually 95%, but we don't care
  mindBm = dBm(0);            // -16.86 dBm
  maxdBm = dBm(RSSI_T_PWM);   // 41.66 dbm
}

/**
 * Prints some debug output across Serial. You can attach to the serial
 * port over USB to read this data; XBee communication uses SoftwareSerial
 * instead.
 */
void debugRssi(
    unsigned long rssiCounts,
    float curdBm,
    float curSignalValue,
    float curStrength) {
  Serial.print("(");
  Serial.print(millis());
  Serial.print(", ");
  Serial.print(rssiCounts);
  Serial.print(", ");
  Serial.print(curdBm);
  Serial.print(", ");
  Serial.print(curSignalValue);
  Serial.print(", ");
  Serial.print(curStrength);
  Serial.println(")");
}

/**
 * This does the work of fetching a '!' from the XBee UART RX buffer, then
 * reading the RSSI PWM signal using pulseIn() and mapping that to a
 * normalized signal strength value on [0, 1]. 0 means turn off all lights;
 * 1 means turn on all lights.
 * 
 * To reduce the effect of random noise, we smooth this raw signal strength
 * value by using a rolling exponential average (see STRENGTH_DECAY above.)
 *
 * Returns true iff '!' was read during this loop iteration.
 */
boolean executeLoop() {
  if (XBee.available() == 0) {
    return false;
  }
  char c = char(XBee.read());
  if (c != '!') {
    return false;
  }
  unsigned long rssiCounts = pulseIn(RSSI_PIN, HIGH, RSSI_TIMEOUT);
  float curdBm = dBm(rssiCounts);
  float curSignalValue = getSignalValue(curdBm);
  strength =
      STRENGTH_DECAY * strength +
      (1.0 - STRENGTH_DECAY) * curSignalValue;
  debugRssi(rssiCounts, curdBm, curSignalValue, strength);
  return true;
}

/**
 * We use loop() here to perform poll-rate limiting while maintaining the
 * PWM on our LEDs. LOOP_QUANTUM is kept relatively low so we have a good
 * chance of receiving the RSSI PWM signal within its 500 ms duration.
 */
void loop() {
  unsigned long start = millis();
  boolean receivedPing = executeLoop();
  while (millis() - start < LOOP_QUANTUM) {
    lightThings(strength);
  }
}

/**
 * According to the Arduino docs, delayMicroseconds() is inaccurate for
 * large input values. We work around that by using delay() and
 * delayMicroseconds() in tandem.
 */
void microDelay(int us) {
  delay(us / 1000);
  delayMicroseconds(us % 1000);
}

/**
 * See http://asasni.cs.up.ac.za/images/d/df/PWM_GJL_vr1.pdf
 * for details.
 */
float dBm(unsigned long rssiCounts) {
  float pwmRatio = 100.0 * rssiCounts / RSSI_T_PWM;
  return (10.24 * pwmRatio - 295) / 17.5;
}

float getSignalValue(float curdBm) {
  return (curdBm - mindBm) / (maxdBm - mindBm);
}

/**
 * This function lights up the pins listed in ledPins according to the
 * current signal strength. It will PWM exactly one of them, possibly at
 * zero brightness, to force a one-QUANTUM delay.
 */
void lightThings(float strength) {
  float numPinsToLight = numPins * strength;
  int numFullPins = min(numFullPins - 1, int(numPinsToLight));
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

/**
 * PWMs a single LED at the specified brightness (duty cycle).
 */
void pwm(int pin, float brightness) {
  int pulseOn = int(QUANTUM * brightness);
  int pulseOff = QUANTUM - pulseOn;
  digitalWrite(pin, HIGH);
  microDelay(pulseOn);
  digitalWrite(pin, LOW);
  microDelay(pulseOff);
}
