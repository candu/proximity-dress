void setup() {
  Serial.begin(9600);	// opens serial port, sets data rate to 9600 bps
}
 
void loop() {
  // every short while, just write a ping to the dress XBee
  Serial.write("!");
  delay(50);
}
