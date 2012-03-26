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

int JUMP = 75;

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
    setPrettyLights(rssi_counts);
  }
}

/*
 We need to PWM (i.e. PWN) that shit.
 */
void setPrettyLights(unsigned long howMuch) {
  Serial.print("yayyyyyyyy ");
  Serial.println(howMuch);
  
  analogWrite(INNER_LED_PIN, (howMuch - JUMP*2)*3);
  analogWrite(MIDDLE_LED_PIN, (howMuch - JUMP)*1.5);
  analogWrite(OUTER_LED_PIN, howMuch);
}

