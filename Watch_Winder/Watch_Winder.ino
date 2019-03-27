// Sebastian Kurniawan, Arduino 1.8.2
// DIY Watch Winder, 2 Feb 2019

// This program will turn a stepper motor a specified amount, after which it will
// stop, delay itself for a specified time, then turn the stepper motor the same
// amount in the opposite direction. Will continue indefinitely. Requires a DS 3231
// real-time clock and a stepper motor.

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


long initMin = 0;
long currentMin = 0;

int daysPerMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

long spinDirection = -30000;

boolean spin = true;

void setup() {
  //  stepper1.setSpeed(200);
  stepper1.setMaxSpeed(3000.0);
  stepper1.setAcceleration(300.0);

  stepper1.moveTo(-spinDirection);
  Serial.begin(9600);
  rtc.begin();

  t = rtc.getTime();

// Uncomment to set the date and time of the real-time clock.
//  rtc.setDOW(MONDAY);     // Set Day-of-Week to SUNDAY
//  rtc.setTime(18, 34, 0);     // Set the time to 12:00:00 (24hr format)
//  rtc.setDate(3, 9, 2018);   // day month year

}//--(end setup )---

void loop() {
  //Change direction when the stepper reaches the target position
  t = rtc.getTime();
  currentMin = calcUltimateMin(rtc.getMonthStr(), t.date, t.hour, t.min);
  
  if (currentMin > (initMin + 30)){
    stepper1.run();
  }

  if (stepper1.distanceToGo() == 0) {
    spinDirection = spinDirection * -1;
    stepper1.moveTo(spinDirection);
    initMin = calcUltimateMin(rtc.getMonthStr(), t.date, t.hour, t.min);
  }
}


// Calculates ultimate minute in a year using real-time clock data. Returns minute as a long.
long calcUltimateMin(String month, int days, int hr, int minute) {
  int ultDays = 0;
  int monthNum = getMonth(month);
  for (int i = 0; i < monthNum; i++) {
    ultDays += daysPerMonth[i];
  }
  ultDays += (days - 1); // e.g. for the 2nd @ 3am, it will be 1 day elapsed plus 3 hours
  return (ultDays * 24L) * 60 + (hr * 60) + minute;
}

// Takes a string specifying the month and returns a integer corresponding to each month.
// January = 0, February = 1, ... , December = 11
int getMonth(String month) {
  if (month.equals("January")) {
    return 0;
  } else if (month.equals("February")) {
    return 1;
  } else if (month.equals("March")) {
    return 2; 
  } else if (month.equals("April")) {
    return 3;
  } else if (month.equals("May")) {
    return 4; 
  } else if (month.equals("June")) {
    return 5;
  } else if (month.equals("July")) {
    return 6; 
  } else if (month.equals("August")) {
    return 7;
  } else if (month.equals("September")) {
    return 8; 
  } else if (month.equals("October")) {
    return 9; 
  } else if (month.equals("November")) {
    return 10; 
  } else { // if (month = December)
    return 11; 
  }
}
