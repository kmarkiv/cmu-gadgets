#import <Arduino.h>
/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the Uno and
  Leonardo, it is attached to digital pin 13. If you're unsure what
  pin the on-board LED is connected to on your Arduino model, check
  the documentation at http://arduino.cc

  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald
 */

const int  switchPinMouseOn = 5;
const int  switchPinMouseClick = 6;
const int  switchPin3 = 7;


int next = 1;

const int  debugPin = 8;

/*------- Data structures for system state -------*/

/* Structure to track and debounce the state of a push button switch */
typedef struct switchTracker {
	int lastReading;       // last raw value read
	long lastChangeTime;   // last time the raw value changed
	byte pin;              // the pin this is tracking changes on
	byte switchState;      // debounced state of the switch
} switchTrack;

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

switchTrack switchInputMouseOn;
switchTrack switchInputMouseClick;
switchTrack switchInput3;

// the setup function runs once when you press reset or power the board
void setup() {
	// initialize digital pin 13 as an output.
	pinMode(debugPin, OUTPUT);
	initSwitchTrack(switchInputMouseOn, switchPinMouseOn);
	initSwitchTrack(switchInputMouseClick, switchPinMouseClick);
	initSwitchTrack(switchInput3, switchPin3);
	Serial.begin(115200);
	Serial.println(F("Initializing Jedi Mouse..."));
}

void outputState(struct switchTracker &sw,int state)
{
Serial.print(sw.pin);
Serial.print(" is ");
Serial.println(state);
}

// the loop function runs over and over again forever
void loop() {

	if (switchChange(switchInputMouseOn) && switchInputMouseOn.switchState == 1 /* pressed */) {
		next ^= 1;
		outputState(switchInputMouseOn,1);
		

	}
	if (switchChange(switchInputMouseClick) && switchInputMouseClick.switchState == 1 /* pressed */) {
		next ^= 1;
		outputState(switchInputMouseClick,1);


	}

	if (switchChange(switchInput3) && switchInput3.switchState == 1 /* pressed */) {
		next ^= 1;
		outputState(switchInput3,1);


	}

	if (next == 1)
	{
		digitalWrite(debugPin, HIGH);
	}
	else {
		digitalWrite(debugPin, LOW);
	}

	// wait for a second
}
