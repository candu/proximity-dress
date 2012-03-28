int BOARD_LED_PIN = 13;

void setup() {
  Serial.begin(9600);	// opens serial port, sets data rate to 9600 bps
  
  pinMode(BOARD_LED_PIN, OUTPUT);
}

void ping() {
  // every short while, just write a ping to the dress XBee
  Serial.write("!");
  Serial.flush();
}

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
