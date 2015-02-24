/*
  Blink
 Turns on an LED on for one second, then off for one second, repeatedly.

 This example code is in the public domain.
 */


int next = 1;
const int totalPins = 4;
const int redPin = 7;
const int bluePin = 5;
const int yellowPin = 4;
const int greenPin = 3;
const int debugPin1 = 13;
const int debugPin2 = 12;
const int redTimer = 2000;
const int blueTimer = 1000;
const int yellowTimer = 500;
const int greenTimer = 250;
const int  switchPin1 = 8;
const int  switchPin2 = 11;
int potPin = A5;
int thisPin = 0;
const int pins[] = {
  redPin, bluePin, yellowPin, greenPin
};
const int pinDelay[] = {
  redTimer, blueTimer, yellowTimer, greenTimer
};

/*------- Data structures for system state -------*/

/* Structure to track and debounce the state of a push button switch */
typedef struct switchTracker {
  int lastReading;       // last raw value read
  long lastChangeTime;   // last time the raw value changed
  byte pin;              // the pin this is tracking changes on
  byte switchState;      // debounced state of the switch
}
switchTrack;

// Note (v0.2b): due to some wierdness in the ordering of generated code, typdefs as above no longer
// fully work unless they are in .h files.  In particular, you can't do the above and then declare
// a function parameter using the type name.  Rather than introduce an additional file, we have just used
// the raw struct type in parameter declarations as a small work around.

/* Fill in a switchTrack structure to start tracking/debouncing a switch
 on the given pin.
 */
void initSwitchTrack(struct switchTracker &sw, int swPin) {
  pinMode(swPin, INPUT);
  sw.lastReading = digitalRead(swPin);
  sw.lastChangeTime = millis();
  sw.pin = swPin;
  sw.switchState = sw.lastReading;
}

switchTrack switchInput1;
switchTrack switchInput2;
void setup() {
  // initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards:
  pinMode(debugPin1, OUTPUT);
  pinMode(debugPin2, OUTPUT);
  Serial.begin(9600);
  for (int thisPin = 0; thisPin < totalPins; thisPin++)  {
    pinMode(pins[thisPin], OUTPUT);
  }
  initSwitchTrack(switchInput1, switchPin1);
  initSwitchTrack(switchInput2, switchPin2);

}

int swapPin(int i, int count)
{
  return (count > 0) ? nextPin(i) : prevPin(i);
}

int nextPin(int i)
{
  return (i == totalPins) ? 0 : i + 1;
}

int prevPin(int i)
{
  return (i == 0) ? totalPins : i - 1;

}

void loop() {

  int sensorValue = 0;
  sensorValue = analogRead(potPin);
  int delayTime = (500 + int((sensorValue - 450) * -0.7)) * 1.5;
  Serial.println(delayTime);

  digitalWrite(pins[thisPin], HIGH);
  unsigned long startTime = millis();
  while (millis() - startTime < delayTime) {

    if (switchChange(switchInput1) && switchInput1.switchState == 1 /* pressed */) {
      next = 1;
      digitalWrite(debugPin1, HIGH);
      digitalWrite(debugPin2, LOW);
    }
    else
    {
      if (switchChange(switchInput2) && switchInput2.switchState == 1 /* pressed */) {
        next = 0;
        digitalWrite(debugPin1, LOW);
        digitalWrite(debugPin2, HIGH);
      }
    }


  }
  digitalWrite(pins[thisPin], LOW);


  thisPin = swapPin(thisPin, next);


  //   delay(2000);
}

boolean switchChange(struct switchTracker &sw) {
  // switch must stay stable this long (in msec) to register
  const long debounceTime = 100;

  // default to no change until we find out otherwise
  boolean result = false;

  // get the current raw reading from the switch
  int reading = digitalRead(sw.pin);

  // if the raw reading has changed (by noise or actual press/release) reset change time to now
  if (reading != sw.lastReading) sw.lastChangeTime = millis();
  sw.lastReading = reading;

  // if time since the last change is longer than the required dwell
  if ((millis() - sw.lastChangeTime) > debounceTime) {
    // note whether we are changing
    result = (reading != sw.switchState);
    // in any case the value has been stable and so the reported state
    // should now match the current raw reading
    sw.switchState = reading;
  }
  return result;
}

