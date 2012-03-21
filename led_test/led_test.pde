int ledPins[] = {9, 10, 11, 13, -1};
int numPins = 0;
int signalValue = 0;
int dirValue = 1;
int maxSignalValue = 255;
int quantum = 20;

void setup() {
  int i = 0;
  for (; ledPins[i] != -1; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
  numPins = i;
}

void pwm(int pin, float brightness) {
  int pulseOn = max(1, (int)(quantum * brightness));
  int pulseOff = quantum - pulseOn;
  digitalWrite(pin, HIGH);
  delay(pulseOn);
  digitalWrite(pin, LOW);
  delay(pulseOff);
}

void lightThings() {
  float numPinsToLight = numPins * (signalValue / (1.0 * maxSignalValue));
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

void loop() {
  if (signalValue > maxSignalValue) {
    signalValue = maxSignalValue;
    dirValue = -1;
  } else if (signalValue < 0) {
    signalValue = 0;
    dirValue = 1; 
  }
  lightThings();
  signalValue += dirValue;
}
