// I2C device class (I2Cdev) demonstration Arduino sketch for MPU6050 class using DMP (MotionApps v2.0)
// 6/21/2012 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//      2013-05-08 - added seamless Fastwire support
//                 - added note about gyro calibration
//      2012-06-21 - added note about Arduino 1.0.1 + Leonardo compatibility error
//      2012-06-20 - improved FIFO overflow handling and simplified read process
//      2012-06-19 - completely rearranged DMP initialization code and simplification
//      2012-06-13 - pull gyro and accel data from FIFO packet instead of reading directly
//      2012-06-09 - fix broken FIFO read sequence and change interrupt detection to RISING
//      2012-06-05 - add gravity-compensated initial reference frame acceleration output
//                 - add 3D math helper file to DMP6 example sketch
//                 - add Euler output and Yaw/Pitch/Roll output formats
//      2012-06-04 - remove accel offset clearing for better results (thanks Sungon Lee)
//      2012-06-01 - fixed gyro sensitivity to be 2000 deg/sec instead of 250
//      2012-05-30 - basic DMP initialization working

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2012 Jeff Rowberg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================
*/

// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include <Brain.h>
#include <SoftwareSerial.h>
#include "MPU6050_6Axis_MotionApps20.h"
//#include "MPU6050.h" // not necessary if using MotionApps include file

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for SparkFun breakout and InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 mpu;
//MPU6050 mpu(0x69); // <-- use for AD0 high

/* =========================================================================
   NOTE: In addition to connection 3.3v, GND, SDA, and SCL, this sketch
   depends on the MPU-6050's INT pin being connected to the Arduino's
   external interrupt #0 pin. On the Arduino Uno and Mega 2560, this is
   digital I/O pin 2.
 * ========================================================================= */

/* =========================================================================
   NOTE: Arduino v1.0.1 with the Leonardo board generates a compile error
   when using Serial.write(buf, len). The Teapot output uses this method.
   The solution requires a modification to the Arduino USBAPI.h file, which
   is fortunately simple, but annoying. This will be fixed in the next IDE
   release. For more info, see these links:

   http://arduino.cc/forum/index.php/topic,109987.0.html
   http://code.google.com/p/arduino/issues/detail?id=958
 * ========================================================================= */



// uncomment "OUTPUT_READABLE_QUATERNION" if you want to see the actual
// quaternion components in a [w, x, y, z] format (not best for parsing
// on a remote host such as Processing or something though)
//#define OUTPUT_READABLE_QUATERNION

// uncomment "OUTPUT_READABLE_EULER" if you want to see Euler angles
// (in degrees) calculated from the quaternions coming from the FIFO.
// Note that Euler angles suffer from gimbal lock (for more info, see
// http://en.wikipedia.org/wiki/Gimbal_lock)
//#define OUTPUT_READABLE_EULER

// uncomment "OUTPUT_READABLE_YAWPITCHROLL" if you want to see the yaw/
// pitch/roll angles (in degrees) calculated from the quaternions coming
// from the FIFO. Note this also requires gravity vector calculations.
// Also note that yaw/pitch/roll angles suffer from gimbal lock (for
// more info, see: http://en.wikipedia.org/wiki/Gimbal_lock)
//#define OUTPUT_READABLE_YAWPITCHROLL

// uncomment "OUTPUT_READABLE_REALACCEL" if you want to see acceleration
// components with gravity removed. This acceleration reference frame is
// not compensated for orientation, so +X is always +X according to the
// sensor, just without the effects of gravity. If you want acceleration
// compensated for orientation, us OUTPUT_READABLE_WORLDACCEL instead.
//#define OUTPUT_READABLE_REALACCEL

// uncomment "OUTPUT_READABLE_WORLDACCEL" if you want to see acceleration
// components with gravity removed and adjusted for the world frame of
// reference (yaw is relative to initial orientation, since no magnetometer
// is present in this case). Could be quite handy in some cases.
//#define OUTPUT_READABLE_WORLDACCEL

// uncomment "OUTPUT_TEAPOT" if you want output that matches the
// format used for the InvenSense teapot demo
//#define OUTPUT_TEAPOT

int16_t ax, ay, az;
int16_t gx, gy, gz;




SoftwareSerial Genotronex(10, 11); // RX, TX

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

// packet structure for InvenSense teapot demo
uint8_t teapotPacket[14] = { '$', 0x02, 0,0, 0,0, 0,0, 0,0, 0x00, 0x00, '\r', '\n' };


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

void outputState(struct switchTracker &sw,int state)
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

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
    mpuInterrupt = true;
}

/*
void brains(){
if (brain.update()) {       
        Serial.println(brain.readCSV());
    }
}
*/

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

    // join I2C bus (I2Cdev library doesn't do this automatically)
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
        TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz)
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    // initialize serial communication
    // (115200 chosen because it is required for Teapot Demo output, but it's
    // really up to you depending on your project)
    softSerial.begin(9600);
    Genotronex.begin(9600);
    Genotronex.println("Bluetooth On please press 1 or 0 blink LED ..");
    while (!Serial); // wait for Leonardo enumeration, others continue immediately

    // NOTE: 8MHz or slower host processors, like the Teensy @ 3.3v or Ardunio
    // Pro Mini running at 3.3v, cannot handle this baud rate reliably due to
    // the baud timing being too misaligned with processor ticks. You must use
    // 38400 or slower in these cases, or use some kind of external separate
    // crystal solution for the UART timer.

    // initialize device
    //Serial.println(F("Initializing I2C devices..."));
    mpu.initialize();

    // verify connection
    //Serial.println(F("Testing device connections..."));
    //Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

    // wait for ready
    //Serial.println(F("\nSend any character to begin DMP programming and demo: "));
    delay(3000);
    //while (Serial.available() && Serial.read()); // empty buffer
    //while (!Serial.available());                 // wait for data
    //while (Serial.available() && Serial.read()); // empty buffer again

    // load and configure the DMP
    //Serial.println(F("Initializing DMP..."));
    devStatus = mpu.dmpInitialize();

    // supply your own gyro offsets here, scaled for min sensitivity
    mpu.setXGyroOffset(222);
    mpu.setYGyroOffset(18);
    mpu.setZGyroOffset(12);
    mpu.setZAccelOffset(1219); // 1688 factory default for my test chip

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
/*    FrequencyTimer2::disable();
  // Set refresh rate (interrupt timeout period)
  FrequencyTimer2::setPeriod(1);
  // Set interrupt routine to be called
  FrequencyTimer2::setOnOverflow(brains);
*/
}



// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================




void loop() {
 if (brain.update()) {
       // Serial.println(brain.readErrors());
        Serial.println(brain.readCSV());

    //    Genotronex.println(brain.readCSV());
    }

    // if programming failed, don't try to do anything
    if (!dmpReady) return;

    // wait for MPU interrupt or extra packet(s) available
    while (!mpuInterrupt && fifoCount < packetSize) {
        // other program behavior stuff here
        // .
        // .
        // .
        // if you are really paranoid you can frequently test in between other
        // stuff to see if mpuInterrupt is true, and if so, "break;" from the
        // while() loop to immediately process the MPU data
        // .
        // .
        // .
    }

     if (Genotronex.available()){
 Genotronex.println("LED  On D13 ON ! ");
    }




    // reset interrupt flag and get INT_STATUS byte
    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();

    // get current FIFO count
    fifoCount = mpu.getFIFOCount();

    // check for overflow (this should never happen unless our code is too inefficient)
    if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
        // reset so we can continue cleanly
        mpu.resetFIFO();
        Serial.println(F("FIFO overflow!"));

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
    } else if (mpuIntStatus & 0x02) {
        // wait for correct available data length, should be a VERY short wait
        while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

        // read a packet from FIFO
        mpu.getFIFOBytes(fifoBuffer, packetSize);
        
        // track FIFO count here in case there is > 1 packet available
        // (this lets us immediately read more without waiting for an interrupt)
        fifoCount -= packetSize;

        mpu.getRotation(&gx, &gy, &gz);
 
    //display tab-separated accel/gyro x/y/z values
        Serial.print("a/g:\t");
        Serial.print(ax); Serial.print("\t");
        Serial.print(ay); Serial.print("\t");
        Serial.print(az); Serial.print("\t");
        Serial.print(gx); Serial.print("\t");
        Serial.print(gy); Serial.print("\t");
        Serial.print(gz); Serial.print("\t");
        Serial.print(brain.readAttention()); Serial.print("\t");
        Serial.println(brain.readSignalQuality());
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
    //delay(500);
    }

    

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
}

