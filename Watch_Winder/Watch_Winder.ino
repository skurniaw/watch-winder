/*
Sebastian Kurniawan, Arduino 1.8.9
DIY Watch Winder, 12 June 2022
v 2.0

This program will turn a stepper motor a specified number of rotations, after which it 
will stop, delay itself for a specified number of minutes, then turn the stepper motor the 
same number of rotations in the opposite direction. Will continue indefinitely. 
Requires a DS 3231 real-time clock and a stepper motor. 

Libraries used:
Patrick Wasp's AccelStepper library.
RinkyDinkElectronic's DS3231 library.

*/

#include <AccelStepper.h>
#include <DS3231.h> // Real-time clock library.

DS3231  rtc(SDA, SCL); // Constructs a clock.
Time  t;  // Time object t. Used to return date/hour/minute/sec etc.

#define HALFSTEP 8

// Motor pin definitions
#define motorPin1  3     // IN1 on the ULN2003 driver 1
#define motorPin2  4     // IN2 on the ULN2003 driver 1
#define motorPin3  5     // IN3 on the ULN2003 driver 1
#define motorPin4  6     // IN4 on the ULN2003 driver 1

// Initialize with pin sequence IN1-IN3-IN2-IN4 for using the AccelStepper with 28BYJ-48
AccelStepper stepper1(HALFSTEP, motorPin1, motorPin3, motorPin2, motorPin4);

// Modify these to change how long to wait between rotating, and how many rotations desired
int pauseMins = 30;
int rotations = 15;

long initMin = 0;
long currentMin = 0;
boolean spin = true;
long destinationPosition = 0;

void setup() {
  stepper1.setMaxSpeed(750.0);
  stepper1.setAcceleration(100.0);
  destinationPosition = calcSteps(rotations);
  stepper1.move(destinationPosition);
  rtc.begin();

// Uncomment to set the date and time of the real-time clock.
//  rtc.setDOW(TUESDAY);     // Set Day-of-Week to SUNDAY
//  rtc.setTime(12, 04, 0);     // Set the time to 12:00:00 (24hr format)
//  rtc.setDate(1, 2, 2022);   // day month year

}

void loop() {
  t = rtc.getTime();
  currentMin = calcMinOfDay(t.hour, t.min);

  if (currentMin < initMin && (1440 - initMin) + currentMin >= pauseMins) {
    // Overlapped into a new day
    stepper1.run();
  } else if (currentMin >= (initMin + pauseMins)) {
    stepper1.run();
  }

  if (stepper1.distanceToGo() == 0) {
    destinationPosition = -destinationPosition;
    stepper1.move(destinationPosition);
    initMin = calcMinOfDay(t.hour, t.min);
  }
}

/**
 * Returns an integer representing the minute of the day.
 * 
 * @param hr      an integer hour
 * @param minute  an integer minute
 * @return        the minute of the day corresponding to the hour and minute passed
 */
int calcMinOfDay(int hr, int minute) {
  return (hr * 60) + minute;
}

/**
 * Returns a long representing the number of steps the stepper motor must take for a
 * certain number of rotations.
 * 
 * @param rotations   integer of desired number of rotations for the watch winder between
 *                    pauses
 * @return            long of required steps corresponding to the rotations
 */
long calcSteps(int rotations) {
  long steps = (long) rotations * 4100;
  return steps;
}
