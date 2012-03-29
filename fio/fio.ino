/**
 * Proximity Dress - Fio
 *
 * @author candu (Evan Stratford)
 * @author valkyriesavage (Valkyrie Savage)
 *
 * Controls the Fio and XBee router for Proximity Dress.
 */

int BOARD_LED_PIN = 13;

void setup() {
  Serial.begin(9600);	// opens serial port, sets data rate to 9600 bps
  
  pinMode(BOARD_LED_PIN, OUTPUT);
}

/**
 * Write out a '!' over XBee serial to be received by the XBee coordinator
 * connected to the Lilypad. Pinging forces the XBee RSSI pin to be set when
 * data is received; we then use that to read off the signal strength on the
 * Lilypad end.
 */
void ping() {
  Serial.write('!');
  Serial.flush();
}

/**
 * Kill time while providing visual feedback that yes, we are sending data.
 */
void ledBlink() {
   digitalWrite(BOARD_LED_PIN, HIGH);
   delay(500);
   digitalWrite(BOARD_LED_PIN, LOW);
   delay(500);
}

void loop() {
  ping();
  ledBlink();
}
