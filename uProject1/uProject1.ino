/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  This example code is in the public domain.
 */

const int pins[] = {7, 5, 4, 3};
const int pinDelay[] = {2000, 1000, 500, 250};
const int next = 1;
const int totalPins = 4;
const int redPin = 7;
const int bluePin = 5;
const int yellowPin = 4;
const int greenPin = 3;
const int debugPin = 13;
const int redTimer = 2000;
const int blueTimer = 1000;
const int yellowTimer = 500;
const int greenTimer = 250;
void setup() {
  // initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards:
  pinMode(debugPin, OUTPUT);
  Serial.begin(9600);
  for (int thisPin = 0; thisPin < totalPins; thisPin++)  {
    pinMode(pins[thisPin], OUTPUT);
  }
  /*pinMode(redPin, OUTPUT);
  pinMode(yellowPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  piMode(greenPin, OUTPUT);*/
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
  for (int thisPin = 0; thisPin < totalPins; thisPin++)  {
    digitalWrite(pins[thisPin], HIGH);
    delay(pinDelay[thisPin]);
    digitalWrite(pins[thisPin], LOW);

  }
  delay(2000);
}
