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
int debug = true;

/* Global objects */
ZumoBuzzer buzzer;
ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON); // pushbutton on pin 12
 
#define NUM_SENSORS 6
unsigned int sensor_values[NUM_SENSORS];
 
ZumoReflectanceSensorArray sensors(QTR_NO_EMITTER_PIN);



/**
 * Main setup function. Only runs ones.
 */
void setup() {
  if (debug) {
      Serial.begin(9600);
  }
  
  // put your setup code here, to run once:
  pinMode(LED_PIN, OUTPUT);
  
  waitForButtonAndCountDown();
}

/**
 * Main loop.
 * Responsible for main program functionallity.
 */
void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED_PIN, HIGH);
  
/*  
  int speed = 400;
  for (int time = 0; time <= 500; time++) {
    motors.setLeftSpeed(-speed);
    motors.setRightSpeed(speed);
    delay(2);
  }
*/
}

/**
 * Waits for an button push, and starts countdown.
 */
void waitForButtonAndCountDown()
{
  digitalWrite(LED_PIN, HIGH);
  button.waitForButton();
  digitalWrite(LED_PIN, LOW);
   
  // play audible countdown
  for (int i = 0; i < 3; i++)
  {
    delay(1000);
    buzzer.playNote(NOTE_G(3), 200, 15);
  }
  delay(1000);
  buzzer.playNote(NOTE_G(4), 500, 15);  
  delay(1000);

  if (debug) {
    Serial.println();
  }
}
