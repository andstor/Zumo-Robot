/*
  The sensor outputs provided by the library are the raw 16-bit values
  obtained by concatenating the 8-bit high and low accelerometer and
  magnetometer data registers. They can be converted to units of g and
  gauss using the conversion factors specified in the datasheet for your
  particular device and full scale setting (gain).

  Example: An LSM303D gives a magnetometer X axis reading of 1982 with
  its default full scale setting of +/- 4 gauss. The M_GN specification
  in the LSM303D datasheet (page 10) states a conversion factor of 0.160
  mgauss/LSB (least significant bit) at this FS setting, so the raw
  reading of -1982 corresponds to 1982 * 0.160 = 317.1 mgauss =
  0.3171 gauss.

  In the LSM303DLHC, LSM303DLM, and LSM303DLH, the acceleration data
  registers actually contain a left-aligned 12-bit number, so the lowest
  4 bits are always 0, and the values should be shifted right by 4 bits
  (divided by 16) to be consistent with the conversion factors specified
  in the datasheets.

  Example: An LSM303DLH gives an accelerometer Z axis reading of -16144
  with its default full scale setting of +/- 2 g. Dropping the lowest 4
  bits gives a 12-bit raw value of -1009. The LA_So specification in the
  LSM303DLH datasheet (page 11) states a conversion factor of 1 mg/digit
  at this FS setting, so the value of -1009 corresponds to -1009 * 1 =
  1009 mg = 1.009 g.
*/
#include <Wire.h>
#include <LSM303.h>
#include <ZumoMotors.h>




class Timer {
    unsigned long nextTimeout;
    boolean hasExpired;

  public:
    Timer(unsigned long duration) {
      nextTimeout = millis() + duration;
    }

    boolean expired() {
      hasExpired = false;
      if (millis() > nextTimeout) {
        hasExpired = true;
      }
      else {
        hasExpired = false;
      }
      return hasExpired;
    }

    void reset(unsigned long duration) {
      nextTimeout = millis() + duration;
      //Serial.println("Reset the timer");
    }
};



ZumoMotors motors;

LSM303 compass;

char report[80];
int FORWARD_SPEED = 400;
unsigned long duration1 = 120;
unsigned long duration2 = 2500;
Timer timer1(duration1);
Timer timer2(duration2);
Timer timer3(0);
Timer timer4(12500);

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  compass.init();
  compass.enableDefault();

delay(200);

}
bool drive = false;
void loop()
{

  compass.read();

  snprintf(report, sizeof(report), "A: %6d %6d %6d    M: %6d %6d %6d",
           compass.a.x, compass.a.y, compass.a.z,
           compass.m.x, compass.m.y, compass.m.z);
  if (timer3.expired()) {
    Serial.println(report);
  }

  /*
    Serial.print("timer1.expired() :");
    Serial.print(timer1.expired());
    Serial.print("      timer2.expired():::   ");
    Serial.println(timer2.expired());
  */
  if (!timer1.expired()) {
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
    drive = true;
  } else if (timer1.expired()) {
    motors.setSpeeds(0, 0);
  }

  if (timer1.expired() && timer2.expired()) {
    timer1.reset(duration1);
    timer2.reset(duration2);
  }

if (timer4.expired()) delay(999999999999);
}

/*
    Checks if the timer has expired. If the timer has expired,
    true is returned. If the timer has not yet expired,
    false is returned.

    @return true if timer has expired, false if not

*/


/**
  Starts the timer and set the timer to expire after the
  number of milliseconds given by the parameter duration.

  @param duration The number of milliseconds until the timer expires.
*/






