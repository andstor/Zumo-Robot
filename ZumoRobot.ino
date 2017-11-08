/**
 * Name:      ZumoRobot.ino
 * Created:   08/10/2017  13:00:00 PM
 * Authors:   André Storaug, Christian Honningdal Leira,
 *              Tormod Lysvold Sjømæling, Eirik Thue,
 *              Vebjørn Tomren og Sander Joachim Skarmyr.
 */

/* Libraries */
#include <ZumoBuzzer.h>
#include <Pushbutton.h>
#include <ZumoMotors.h>
#include <QTRSensors.h>
#include <ZumoReflectanceSensorArray.h>

/* Global constants */
const int LED_PIN = 13;

/* Global variables */
int var;

/**
 * Main setup function. Only runs ones.
 */
void setup() {
  // put your setup code here, to run once:
  pinMode(LED_PIN, OUTPUT);
}

/**
 * Main loop.
 * Responsible for main program functionallity.
 */
void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED_PIN, HIGH);


  for (int time = 0; time <= 500; time++) {
    int speed = 100;

    motors.setLeftSpeed(speed-50);
    motors.setRightSpeed(speed+50);
    delay(2);
  }

  for (int time = 0; time <= 500; time++) {
    int speed = -100;

    motors.setLeftSpeed(speed-50);
    motors.setRightSpeed(speed+50);
    delay(2);
  }
}
