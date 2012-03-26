#include <SoftwareSerial.h>
/*
 This code is from tigoe:
 https://github.com/tigoe/MakingThingsTalk2/blob/master/chapter8/project17/XbeeSignalStrengthReader/XbeeSignalStrengthReader.pde
 
 XBee Signal Strength Reader
 Context: Arduino (Lilypad)
 
 Reads a packet from an XBee radio and parses it.  The packet 
 should be 22 bytes long. It should be made up of the following:
 byte 1:     0x7E, the start byte value
 byte 2-3:   packet size, a 2-byte value  (not used here)
 byte 4:     API identifier value, a code that says what this response is (not used here)
 byte 5-6:   Sender's address
 byte 7:     RSSI, Received Signal Strength Indicator (not used here)
 byte 8:     Broadcast options (not used here)
 byte 9:     Number of samples to follow
 byte 10-11: Active channels indicator (not used here)
 byte 12-21: 5 10-bit values, each ADC samples from the sender 
 */

int packet[22];      // with 5 samples, the XBee packet is 22 bytes long
int byteCounter;     // keeps track of where you are in the packet
int rssi = 0;        // received signal strength
int address = 0;     // the sending XBee 's address
int lastReading = 0; // value of the previous incoming byte

SoftwareSerial Xbee(5,6);

void setup() {
  Serial.begin(9600);
  Xbee.begin(9600);
}

void loop() {
  if (Xbee.available()) {
    Serial.print("ALSO MY XBEE IS NOT FUCKING DEAD : ");
    // read a byte from the port:
    int thisByte = Xbee.read();
    Serial.println(thisByte);
    // if the byte = 0x7E, the value of a start byte, you have 
    // a new packet:
    if (thisByte == 0x7E) {   // start byte
      // parse the previous packet if there's data:
      if (packet[2] > 0) {
        // we have new data!  set the pretty lights!
        setPrettyLights(parseData());
      }
      // reset the byte counter:
      byteCounter = 0;
    }
    // put the current byte into the packet at the current position:
    packet[byteCounter] = thisByte;
    //  increment the byte counter:
    byteCounter++;
  }
}

/*
 We need to PWM (i.e. PWN) that shit.
 */
void setPrettyLights(int howMuch) {
  Serial.print("yayyyyyyyy ");
  Serial.println(howMuch);
  
  Serial.print("we are receiving from XBee with address ");
  Serial.println(address);
}


/*
 Once you've got a packet, you need to extract the useful data. 
 This method gets the address of the sender and RSSI.
 */
int parseData() {
  int result = -1;    // if you get no result, -1 will indicate that.

  // make sure you've got enough of a packet to read the data:
  if (byteCounter > 6) {
    // read the address. It's a two-byte value, so you
    // add the two bytes as follows:
    address = packet[5] + packet[4] * 256;
    // get RSSI:
    result = packet[6];
  }
  return result;
}

