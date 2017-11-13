/**
   Name:      ZumoRobot.ino
   Created:   08/10/2017  13:00:00 PM
   Authors:   André Storaug, Christian Honningdal Leira,
                Tormod Lysvold Sjømæling, Eirik Thue,
                Vebjørn Tomren og Sander Joachim Skarmyr.
*/

/* Libraries */
#include <ZumoBuzzer.h>
#include <Pushbutton.h>
#include <ZumoMotors.h>
#include <QTRSensors.h>
#include <ZumoReflectanceSensorArray.h>


/* Global constants */
 int LED_PIN = 13;

// this might need to be tuned for different lighting conditions, surfaces, etc.
const int QTR_THRESHOLD     = 1800; //

// these might need to be tuned for different motor types
const int REVERSE_SPEED     = 200; // 0 is stopped, 400 is full speed
const int TURN_SPEED        = 200;
const int FORWARD_SPEED     = 100;
const int REVERSE_DURATION  = 200; // ms
const int TURN_DURATION     = 300; // ms


/* Global variables */
boolean debug = true;


/* Global objects */
ZumoBuzzer buzzer;
ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON); // pushbutton on pin 12


const int NUM_SENSORS = 2;
unsigned int sensor_values[NUM_SENSORS]; // Array to keep sensor values from ZumoReflectanceSensorArray.

byte pins[] = {4, 5};
ZumoReflectanceSensorArray sensors(pins, 2, 2000, QTR_NO_EMITTER_PIN); // 2000 = timeout after 2 ms; 



/**
   Waits for an button push, and starts countdown.
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


/**
   Drive forward and turn left or right when border is detected
   - Only reflectionsensor on pin 5 and 4 are used.
*/
void borderDetect() {
  sensors.read(sensor_values);

  if (sensor_values[0] > QTR_THRESHOLD) // Needs to be reversed if on black surface with white border.
  {
    // if leftmost sensor detects line, reverse and turn to the right
    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
    delay(REVERSE_DURATION);
    motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
    delay(TURN_DURATION);
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  }
  else if (sensor_values[1] > QTR_THRESHOLD) // Needs to be reversed if on black surface with white border.
  {
    // if rightmost sensor detects line, reverse and turn to the left
    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
    delay(REVERSE_DURATION);
    motors.setSpeeds(-TURN_SPEED, TURN_SPEED);
    delay(TURN_DURATION);
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  }
  else
  {
    // otherwise, go straight
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  }
}






/* ---------------------Main section------------------------------ */

/**
   Main setup function. Only runs ones.
*/
void setup() {
  if (debug) {
    Serial.begin(9600);
  }
  
  // Initiate ZumoReflectanceSensorArray sensors.
  //sensors.init;

  // Uncomment if necessary to correct motor directions
  //motors.flipLeftMotor(true);
  //motors.flipRightMotor(true);

  pinMode(LED_PIN, OUTPUT);

  waitForButtonAndCountDown(); // Wait for buttond and count down.
}




/**
   Main loop.
   Responsible for main program functionallity.
*/
void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED_PIN, HIGH);


  // Debugging
  if (debug == true) {
    /**
       Ability to start or stop edge detection.
    */
    if (button.isPressed())
    {
      // if button is pressed, stop and wait for another press to go again
      motors.setSpeeds(0, 0);
      button.waitForRelease();
      waitForButtonAndCountDown();
    }
  }

  borderDetect();


  /*
    int speed = 400;
    for (int time = 0; time <= 500; time++) {
      motors.setLeftSpeed(-speed);
      motors.setRightSpeed(speed);
      delay(2);
    }
  */
}
