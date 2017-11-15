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



/* Global constants */
const int LED_PIN = 13;
const int EYE_SENSOR_LEFT = A1;
const int EYE_SENSOR_RIGHT = A2;


// this might need to be tuned for different lighting conditions, surfaces, etc.
const int QTR_THRESHOLD     = 1800; //

// these might need to be tuned for different motor types
const int REVERSE_SPEED     = 200; // 0 is stopped, 400 is full speed
const int TURN_SPEED        = 200;
const int FORWARD_SPEED     = 400;
const int SEARCH_SPEED      = 200;
const int REVERSE_DURATION  = 200; // ms
const int TURN_DURATION     = 300; // ms


// Constants representing the states in the state machine
const int S_SEARCHING = 0;
const int S_CHASING = 1;
const int S_COLLISION = 2;


/* Global variables */
bool debug = true;
int currentState = S_SEARCHING;
bool enemyDetected = false;
int leftDistance = 0;
int rightDistance = 0;
bool collision = false;


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





bool checkBorderDetection() {
  sensors.read(sensor_values);

  if (sensor_values[0] > QTR_THRESHOLD || sensor_values[1] > QTR_THRESHOLD) { // Needs to be reversed if on black surface with white border.
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




/*
  void enemyDetection(int LEFT, int RIGHT) {
  int leftDistance = analogRead(EYE_SENSOR_LEFT);
  int rightDistance = analogRead(EYE_SENSOR_RIGHT);

  int multiplyer = 2;

  if (!distance) {
    // No object detected.

    motors.setLeftSpeed(-SEARCH_SPEED);
    motors.setRightSpeed(SEARCH_SPEED);
  }
  else if (leftDistance == 0 && rightDistance != 0) {
    // Object detected on right side.

    motors.setLeftSpeed(SEARCH_SPEED);
    motors.setRightSpeed(SEARCH_SPEED);
  }
  else if (leftDistance != 0 && rightDistance == 0) {

  }
  else {
    // Object is right in front.
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);

  }


  }
*/

void readDistanceSensors() {
  leftDistance = analogRead(EYE_SENSOR_LEFT);
  rightDistance = analogRead(EYE_SENSOR_RIGHT);
}


void searchForEnemy() {
  checkEnemyPresence();
  motors.setLeftSpeed(SEARCH_SPEED);
  motors.setRightSpeed(-SEARCH_SPEED);
}


/**
   Checks for enemy.
   @return true if enemy is detected.
*/
bool checkEnemyPresence() {
  if (leftDistance != 0 || rightDistance != 0) {
    return true;
  }
  else {
    return false;
  }
}


void chaseEnemy() {
  int multiplyer = 2;

  if (leftDistance == 0 && rightDistance != 0) {
    // Object detected on right side.

    motors.setLeftSpeed(SEARCH_SPEED);
    motors.setRightSpeed(SEARCH_SPEED);
  }
  else if (leftDistance != 0 && rightDistance == 0) {

  }
  else {
    // Object is right in front.

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
  searchForEnemy();

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
     *  Ability to start or stop motors.
    */
    if (button.isPressed())
    {
      // if button is pressed, stop and wait for another press to go again
      motors.setSpeeds(0, 0);
      button.waitForRelease();
    }
  }


  // SUPERIOR FUNCTON TO THE STATE MACHINE
  //borderDetected(); // Possibly run with a timer.

  readDistanceSensors();




  // The state machine implemented using switch-case
  switch (currentState)
  {
    // State SEARCHING
    case S_SEARCHING:
      if (checkBorderDetection) {
        borderDetected();
        searchForEnemy();
        changeStateTo(S_SEARCHING);
      }
      else if (checkEnemyPresence() == true)
      {
        // SATRT CHASING function
        changeStateTo(S_CHASING);
      }
      break;

    // State CHASING
    case S_CHASING:
      if (checkBorderDetection) {
        borderDetected();
        searchForEnemy();
        changeStateTo(S_SEARCHING);
      }
      else if (collision) {
        // Start collision sequence
        changeStateTo(S_COLLISION);
      }
      break;

    // State COLLISION
    case S_COLLISION:
      if (checkBorderDetection) {
        borderDetected();
        searchForEnemy();
        changeStateTo(S_SEARCHING);
      }
      else if (checkEnemyPresence() == false) {
        // Start searching
        changeStateTo(S_SEARCHING);
      }
  }
}






/**
 * Prints the state to Serial Monitor as a text, based
 * on the state-constant provided as the parameter state
 *
 * @param state The state to print the tekst-representation for.
 */
void printState(int state)
{
  switch (state)
  {
    case S_SEARCHING:
      Serial.print("S_SEARCHING");
      break;

    case S_CHASING:
      Serial.print("S_CHASING");
      break;

    case S_COLLISION:
      Serial.print("S_COLLISION");
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





/**
    Checks if the timer has expired. If the timer has expired,
    true is returned. If the timer has not yet expired,
    false is returned.

    @return true if timer has expired, false if not


  boolean timerHasExpired()
  {
  boolean hasExpired = false;
  if (millis() > nextTimeout)
  {
    hasExpired = true;
  }
  else
  {
    hasExpired = false;
  }
  return hasExpired;
  }

  /**
   Starts the timer and set the timer to expire after the
   number of milliseconds given by the parameter duration.

   @param duration The number of milliseconds until the timer expires.

  void startTimer(unsigned long duration)
  {
  nextTimeout = millis() + duration;
  }


*/
