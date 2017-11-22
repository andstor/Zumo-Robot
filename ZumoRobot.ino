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
#include <SharpDistSensor.h>


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


/* Global constants */
const int LED_PIN = 13;
const int EYE_SENSOR_LEFT = A2;
const int EYE_SENSOR_RIGHT = A1;

// Window size of the median filter (odd number, 1 = no filtering)
const byte mediumFilterWindowSize = 5;

const int LEFT = 0;
const int RIGHT = 1;

// this might need to be tuned for different lighting conditions, surfaces, etc.
const int QTR_THRESHOLD        = 1800; //
const int DISTANCE_THRESHOLD   = 500; //
const int EMENY_LOST_TIMER     = 500;

// these might need to be tuned for different motor types
const int REVERSE_SPEED     = 400; // 0 is stopped, 400 is full speed
const int TURN_SPEED        = 400;
const int FORWARD_SPEED     = 320;
const int MAX_SPEED         = 400;
const int SEARCH_SPEED      = 200;
const int REVERSE_DURATION  = 200; // ms
const int TURN_DURATION     = 300; // ms


// Constants representing the states in the state machine
const int S_DRIVE_RANDOM = 0;
const int S_SEARCHING = 1;
const int S_CHASING = 2;



/* Global variables */
bool debug = false;
int currentState = S_DRIVE_RANDOM;
bool enemyDetected = false;
int leftDistance = 0;
int rightDistance = 0;
bool collision = false;
int directionTarget = 0;

/* Global objects */
ZumoBuzzer buzzer;
ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON); // pushbutton on pin 12
Timer timerEnemyDetected(EMENY_LOST_TIMER);

// Create an object instance of the SharpDistSensor class
SharpDistSensor sensorLeft(EYE_SENSOR_LEFT, mediumFilterWindowSize);
SharpDistSensor sensorRight(EYE_SENSOR_RIGHT, mediumFilterWindowSize);


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
}





bool checkBorderDetection() {
  sensors.read(sensor_values);

  if (sensor_values[0] < QTR_THRESHOLD || sensor_values[1] < QTR_THRESHOLD) { // Needs to be reversed if on black surface with white border.
    if (debug) {
      Serial.println("Warning: Border detected!");
    }

    return true;
  }
  else {
    return false;
  }
}


/**
   Drive forward and turn left or right when border is detected
   - Only reflectionsensor on pin 5 and 4 are used.
*/
void borderDetected() {
  sensors.read(sensor_values);

  if (sensor_values[0] > QTR_THRESHOLD && sensor_values[1] > QTR_THRESHOLD) {

    // otherwise, go straight backwards
    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
    delay(REVERSE_DURATION);
    motors.setSpeeds(-TURN_SPEED, TURN_SPEED);
    delay(TURN_DURATION);

  }
  else if (sensor_values[0] > QTR_THRESHOLD) // Needs to be reversed if on black surface with white border.
  {
    // if leftmost sensor detects line, reverse and turn to the right
    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
    delay(REVERSE_DURATION);
    motors.setSpeeds(-TURN_SPEED, TURN_SPEED);
    delay(TURN_DURATION);
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);

    // Update direction target.
    directionTarget = RIGHT;
    Serial.println(directionTarget);
  }
  else if (sensor_values[1] >  QTR_THRESHOLD) // Needs to be reversed if on black surface with white border.
  {
    // if rightmost sensor detects line, reverse and turn to the left
    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
    delay(REVERSE_DURATION);
    motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
    delay(TURN_DURATION);
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);

    // Update direction target.
    directionTarget = LEFT;
    Serial.println(directionTarget);
  }
  motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
}


void readDistanceSensors() {
  leftDistance = sensorLeft.getDist();
  rightDistance = sensorRight.getDist();
  // Get distance from sensor
}


void searchForEnemy(int dir) {
  checkEnemyPresence();
  switch (dir)
  {
    case 0:
      // Left
      motors.setLeftSpeed(-SEARCH_SPEED);
      motors.setRightSpeed(SEARCH_SPEED);
      break;

    case 1:
      // Right

      motors.setLeftSpeed(SEARCH_SPEED);
      motors.setRightSpeed(-SEARCH_SPEED);
      break;

    default:
      Serial.print("Error in searchForEnemy(): No direction provided!");
      break;
  }

}


/**
   Checks for enemy.
   @return true if enemy is detected.
*/
bool checkEnemyPresence() {
  // Update distance sensor.
  readDistanceSensors();

  if (leftDistance < DISTANCE_THRESHOLD || rightDistance < DISTANCE_THRESHOLD) {
    return true;
  }
  else {
    return false;
  }
}


void chaseEnemy() {
  int multiplyer = 200;
  // SET LAST SEEN TARGET DIRECTION!!!!!!!!!!!!!!!!!!
  if (leftDistance > DISTANCE_THRESHOLD && rightDistance < DISTANCE_THRESHOLD) {
    // Object detected on right side.
    directionTarget = RIGHT;
    if (debug) {
      Serial.println("Info: Enemey seen to the right");
      Serial.println(rightDistance);

    }
    // buzzer.playNote(NOTE_G(4), 200, 15);

    motors.setLeftSpeed(FORWARD_SPEED);
    motors.setRightSpeed(FORWARD_SPEED - multiplyer);
  }
  else if (leftDistance < DISTANCE_THRESHOLD && rightDistance > DISTANCE_THRESHOLD) {
    // Object detected on left side.
    directionTarget = LEFT;
    if (debug) {
      Serial.println("Info: Enemey seen to the left");
      Serial.println(leftDistance);
    }
    // buzzer.playNote(NOTE_G(3), 200, 15);

    motors.setLeftSpeed(FORWARD_SPEED - multiplyer);
    motors.setRightSpeed(FORWARD_SPEED);
  }
  else if (leftDistance < DISTANCE_THRESHOLD && rightDistance < DISTANCE_THRESHOLD) {
    // Object is right in front.
    if (debug) {
      Serial.println("Info: Enemey up front");

    }
    // buzzer.playNote(NOTE_G(2), 200, 15);

    motors.setLeftSpeed(FORWARD_SPEED);
    motors.setRightSpeed(FORWARD_SPEED);
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

  pinMode(LED_PIN, OUTPUT);

  waitForButtonAndCountDown(); // Wait for buttond and count down.
  searchForEnemy(directionTarget);
  motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
}




/**
   Main loop.
   Responsible for core program functionallity.
*/
void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED_PIN, HIGH);


  // Debugging
  if (debug == true) {
    /**
        Ability to start or stop motors.
    */
    if (button.isPressed())
    {
      // if button is pressed, stop and wait for another press to go again
      motors.setSpeeds(0, 0);
      button.waitForRelease();
    }
  }



  // The state machine implemented using switch-case
  switch (currentState)
  {
    // State RANDOM DRIVING
    case S_DRIVE_RANDOM:

      if (checkBorderDetection()) {
        borderDetected();
      }
      else if (checkEnemyPresence() == true) {
        // SATRT CHASING function
        chaseEnemy();
        changeStateTo(S_CHASING);
      }

      break;

    // State SEARCHING
    case S_SEARCHING:
      if (checkBorderDetection()) {
        borderDetected();
      }
      else if (checkEnemyPresence() == true)
      {
        // SATRT CHASING function
        chaseEnemy();
        changeStateTo(S_CHASING);
      } else if (timerEnemyDetected.expired() == true) {
        motors.setSpeeds(MAX_SPEED, MAX_SPEED);
        changeStateTo(S_DRIVE_RANDOM);

      }
      break;

    // State CHASING
    case S_CHASING:
      //Run chasing function
      if (checkBorderDetection()) {
        borderDetected();
      }
      else if (checkEnemyPresence() == true) {
        chaseEnemy();
        timerEnemyDetected.reset(EMENY_LOST_TIMER);
      }

      else if (checkEnemyPresence() == false)
      {
        searchForEnemy(directionTarget);
        changeStateTo(S_SEARCHING);
      }
      break;


  }
}






/**
   Prints the state to Serial Monitor as a text, based
   on the state-constant provided as the parameter state

   @param state The state to print the tekst-representation for.
*/
void printState(int state)
{
  switch (state)
  {

    case S_DRIVE_RANDOM:
      Serial.print("S_DRIVE_RANDOM");
      break;

    case S_SEARCHING:
      Serial.print("S_SEARCHING");
      break;

    case S_CHASING:
      Serial.print("S_CHASING");
      break;


    default:
      Serial.print("!!UNKNOWN!!");
      break;
  }
}


void changeStateTo(int newState)
{
  // At this point, we now what the current state is (the value
  // of the global variable currentState), and we know what the
  // next state is going to be, given by the parameter newState.
  if (debug == true) {
    // By using the printState()-funksjon, we can now print the
    // full debug-message:
    Serial.print("State changed from ");
    printState(currentState);
    Serial.print(" to ");
    printState(newState);
    Serial.println(); // To add a new line feed
  }
  // And finally, set the current state to the new state
  currentState = newState;
}

