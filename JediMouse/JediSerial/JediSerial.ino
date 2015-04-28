/*
  Jedi Mouse

  A headtracker using a gyroscope and EEG input

  The circuit:

  * MPU6050 Accelerometer
  * EEG HeadSet
  * 3 push buttons
  * 3 leds
  * HC05 bluetooth device

  Created day month year
  By Vikram Kamath
  Modified day month year
  By author's name

  http://url/of/online/tutorial.cc

*/
#include "I2Cdev.h"
#include <Brain.h>
#include <SoftwareSerial.h>
#include "MPU6050_6Axis_MotionApps20.h"
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif


MPU6050 mpu;

// accelerometer
int16_t ax, ay, az;
int16_t gx, gy, gz;




//SoftwareSerial Genotronex(10, 11); // RX, TX

// Set up the software serial port on pins 10 (RX) and 11 (TX). We'll only actually hook up pin 10.
SoftwareSerial softSerial(A1, 4);

// Set up the brain reader, pass it the software serial object you want to listen on.
Brain brain(softSerial);


#define LED_PIN 13 // (Arduino is 13, Teensy is 11, Teensy++ is 6)
bool blinkState = false;

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

const int  switchPinMouseOn = 5;
const int  switchPinMouseClick = 6;
const int  switchPin3 = 7;


int next = 1;
int redPress = 0;
int bluePress = 0;
int greenPress = 0;

const int  debugPin = 8;

/*------- Data structures for system state -------*/

/* Structure to track and debounce the state of a push button switch */
typedef struct switchTracker {
    int lastReading;       // last raw value read
    long lastChangeTime;   // last time the raw value changed
    byte pin;              // the pin this is tracking changes on
    byte switchState;      // debounced state of the switch
} switchTrack;

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

void outputState(struct switchTracker &sw, int state)
{
    Serial.print(sw.pin);
    Serial.print(" is ");
    Serial.println(state);
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


// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================

volatile bool mpuInterrupt = false;
void dmpDataReady() {
    mpuInterrupt = true;
}

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup() {

    pinMode(debugPin, OUTPUT);
    initSwitchTrack(switchInputMouseOn, switchPinMouseOn);
    initSwitchTrack(switchInputMouseClick, switchPinMouseClick);
    initSwitchTrack(switchInput3, switchPin3);
    Serial.begin(9600);
    Serial.println(F("Initializing Jedi Mouse..."));
    softSerial.begin(9600);
    while (!Serial); // wait for Leonardo enumeration, others continue immediately
    mpu.initialize();
    devStatus = mpu.dmpInitialize();

    // Gyro Offset
    mpu.setXGyroOffset(222);
    mpu.setYGyroOffset(18);
    mpu.setZGyroOffset(12);
    mpu.setZAccelOffset(1219);

    // DMP init code
    // make sure it worked (returns 0 if so)
    if (devStatus == 0) {
        // turn on the DMP, now that it's ready
        Serial.println(F("Enabling DMP..."));
        mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection
        Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
        attachInterrupt(0, dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        Serial.println(F("DMP ready! Waiting for first interrupt..."));
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        Serial.print(F("DMP Initialization failed (code "));
        Serial.print(devStatus);
        Serial.println(F(")"));
    }

    // configure LED for output
    pinMode(LED_PIN, OUTPUT);
}



// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================


void loop() {


    // if programming failed, don't try to do anything
    if (!dmpReady) return;

    if (brain.update()) {
        // Serial.println(brain.readErrors());
        Serial.println(brain.readCSV());

        //    Genotronex.println(brain.readCSV());
    }


    if (switchChange(switchInputMouseOn) && switchInputMouseOn.switchState == 1 /* pressed */) {
        next ^= 1;
        outputState(switchInputMouseOn, 1);
        redPress = 1;


    }
    if (switchChange(switchInputMouseClick) && switchInputMouseClick.switchState == 1 /* pressed */) {
        next ^= 1;
        outputState(switchInputMouseClick, 1);
        bluePress = 1;


    }

    if (switchChange(switchInput3) && switchInput3.switchState == 1 /* pressed */) {
        next ^= 1;
        outputState(switchInput3, 1);
        greenPress = 1;


    }

    if (next == 1)
    {
        digitalWrite(debugPin, HIGH);
    }
    else {
        digitalWrite(debugPin, LOW);
    }

    // wait for MPU interrupt or extra packet(s) available
    while (!mpuInterrupt && fifoCount < packetSize) {

    }

    // reset interrupt flag and get INT_STATUS byte

    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();

    // get current FIFO count
    fifoCount = mpu.getFIFOCount();
    if ((mpuIntStatus & 0x10) || fifoCount == 1024) {

        mpu.resetFIFO();
        Serial.println(F("FIFO overflow!"));

    } else if (mpuIntStatus & 0x02) {
        // wait for correct available data length, should be a VERY short wait
        while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

        mpu.getFIFOBytes(fifoBuffer, packetSize);
        fifoCount -= packetSize;

        mpu.getRotation(&gx, &gy, &gz);

        //display tab-separated for mouse movemeny
        Serial.print("a/g:\t");
        Serial.print(ax); Serial.print("\t");
        Serial.print(ay); Serial.print("\t");
        Serial.print(az); Serial.print("\t");
        Serial.print(gx); Serial.print("\t");
        Serial.print(gy); Serial.print("\t");
        Serial.print(gz); Serial.print("\t");
        Serial.print(redPress); Serial.print("\t");
        Serial.print(bluePress); Serial.print("\t");
        Serial.print(greenPress); Serial.println("\t");

        /*
                Genotronex.print("a/g:\t");
                Genotronex.print(ax); Genotronex.print("\t");
                Genotronex.print(ay); Genotronex.print("\t");
                Genotronex.print(az); Genotronex.print("\t");
                Genotronex.print(gx); Genotronex.print("\t");
                Genotronex.print(gy); Genotronex.print("\t");
                Genotronex.print(gz); Genotronex.print("\t");
                Genotronex.print(brain.readAttention()); Genotronex.print("\t");
                Genotronex.println(brain.readSignalQuality());
        */

        blinkState = !blinkState;
        digitalWrite(LED_PIN, blinkState);
        redPress = 0;
        bluePress = 0;
        greenPress = 0;
    }

}


