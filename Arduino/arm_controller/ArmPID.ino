#include <Servo.h>

Servo myservo;


//12 bit accuracy = 2 ^ 12 = 4096 ->0 to 4095
const int MAX_ACCURACY = 4095;
//base of rover arm (parallel to ground)

const int MAX_ANGLE = 270;
const double CONVERSION_FACTOR = (double)MAX_ANGLE / (double)MAX_ACCURACY;

//no movement on servo
const int SERVO_CENTER = 90;

//base angle (perpendicular to ground)
int baseAngle = 135;
//0 = 0 degrees
//4095 = 270 degrees
//TODO: make these final or whatever
//int MAX_BASE_ANGLE = 270;

//shoulder angle (perpendicular to ground)
int shoulderAngle = 135;
//0 = 0 degrees
//4095 = 270
//int MAX_SHOULDER_ANGLE = 270;

//elbow angle (perpendicular to ground)
int elbowAngle = 135;
//0 = 0
//4095 = 270
//int MAX_SHOULDER_ANGLE = 270;

//wrist pitch angle (perpendicular to ground)
int wristPitchAngle = 135;
//0 = 0
//4095 = 270
//int MAX_WRIST_ANGLE = 270;

//wrist roll angle (not perpendicular to ground)
int wristRollAngle = 135;
//0 = 0
//4095 = 270
//int MAX_WRIST_ANGLE = 270;

//PID constants
//TODO: tune
const double KP_BASE = 1;
const double KI_BASE = 0;
const double KD_BASE = 0;

const double KP_SHOULDER = 1;
const double KI_SHOULDER = 0;
const double KD_SHOULDER = 0;

const double KP_ELBOW = 1;
const double KI_ELBOW = 0;
const double KD_ELBOW = 0;

const double KP_WRIST_PITCH = 1;
const double KI_WRIST_PITCH = 0;
const double KD_WRIST_PITCH = 0;

const double KP_WRIST_ROLL = 1;
const double KI_WRIST_ROLL = 0;
const double KD_WRIST_ROLL = 0;

//PID vars
//TODO: can these be shared?
unsigned long previousTimeBase = 0;
int previousErrorBase = 0;
double integralBase = 0;

unsigned long previousTimeShoulder = 0;
int previousErrorShoulder = 0;
double integralShoulder = 0;

unsigned long previousTimeElbow = 0;
int previousErrorElbow = 0;
double integralElbow = 0;

unsigned long previousTimeWristPitch = 0;
int previousErrorWristPitch = 0;
double integralWristPitch = 0;

unsigned long previousTimeWristRoll = 0;
int previousErrorWristRoll = 0;
double integralWristRoll = 0;

char _baseMotor = A0;//Pin for base motor
char _shoulderMotor = A1;//Pin for shoulder motor
char _elbowMotor = A2;//Pin for elbow motor
char _wristPitchMotor = A3;//Pin for wrist pitch motor
char _wristRollMotor = A4;//Pin for wrist motor

char _basePot = A5;//Pin for base potentiometer
char _shoulderPot = A6;//Pin for shoulder potentiometer
char _elbowPot = A7;//Pin for elbow potentiometer
char _wristPitchPot = 0;//Pin for wrist pitch potentiometer
char _wristRollPot = 1;//Pin for wrist roll potentiometer


void setup() {

  Serial.begin(9600);
  myservo.attach(4);

  //set the pinmode of the motor ports to be output
  pinMode(_baseMotor, OUTPUT);
  pinMode(_shoulderMotor, OUTPUT);
  pinMode(_elbowMotor, OUTPUT);
  pinMode(_wristPitchMotor, OUTPUT);
  pinMode(_wristRollMotor, OUTPUT);

  //set the pinmode of the potentiometer ports to be input
  pinMode(_basePot, INPUT);
  pinMode(_shoulderPot, INPUT);
  pinMode(_elbowPot, INPUT);
  pinMode(_wristPitchPot, INPUT);
  pinMode(_wristRollPot, INPUT);

}

void loop() {
 //TODO: get message int
int message = 0;
 adjustArm(message);
}


/*
* Reads in the message from the Jetson, splits into segments,
* and calls individual movement method for each joint in the arm
*/
void adjustArm(char message[14])
{
  char baseMoreSig = message[2];
  char baseLessSig = message[3];
  int baseTarget = (baseMoreSig << 8) + baseLessSig;//first part of messageNumber
  
  char shoulderMoreSig = message[4];
  char shoulderLessSig = message[5];
  int shoulderTarget = (shoulderMoreSig << 8) + shoulderLessSig;//second part of messageNumber
  
  char elbowMoreSig = message[6];
  char elbowLessSig = message[7];
  int elbowTarget = (elbowMoreSig << 8) + elbowLessSig;//third part of messageNumber
  
  char wristPitchMoreSig = message[8];
  char wristPitchLessSig = message[9];
  int wristPitchTarget = (wristPitchMoreSig << 8) + wristPitchLessSig;//fourth part of messageNumber
  
  char wristRollMoreSig = message[10];
  char wristRollLessSig = message[11];
  int wristRollTarget = (wristRollMoreSig << 8) + wristRollLessSig;//

  moveBase(baseTarget);
  moveShoulder(shoulderTarget);
  moveElbow(elbowTarget);
  moveWristPitch(wristPitchTarget);
  moveWristRoll(wristRollTarget);
}

/*
* Receives the movement info from the adjustArm method
* and converts it into a number the base motor can use.
* Has PID for control system
*/
void moveBase(int baseTargetPosition)
{
  if (baseTargetPosition < 0 || baseTargetPosition > 4095)
  {
    //TODO: throw error
  }

  //convert from 0-4095 to 0-270
  int baseTargetAngle = (int)(CONVERSION_FACTOR * baseTargetPosition);

  unsigned long currentTimeBase = millis();
  int dt = currentTimeBase - previousTimeBase;//change in time
  previousTimeShoulder = currentTimeBase;//update old time

  //error between what it is and what we want
  int basePosition = analogRead(_basePot);
  int error = baseTargetPosition - basePosition;


  //integral/sum of error
  integralBase += error * dt / 1000.0;//divide by 1000 because dt is ms, adjust for seconds


  //derivative/rate of change of error
  double derivative = 1000.0*(error - previousErrorBase) / dt;

  int sum = (int)(KP_BASE * error + KI_BASE * integralBase + KD_BASE * derivative);

  previousErrorBase = error;

  //TODO:output to servo
  //working w/ full rotational servos
  //90 = No Movement, 0 full speed one direction, 180 full speed other direction

  int output = SERVO_CENTER + sum;

  if (output > 180)
  {
    output = 180;
  }
  if (output < 0)
  {
    output = 0;
  }

  //TODO: write to servo
  analogWrite(_baseMotor, output);
}

/*
* Receives the movement info from the adjustArm method
* and converts it into a number the shoulder motor can use.
* Has PID for control system.
*/
void moveShoulder(int shoulderTargetPosition)
{
  if (shoulderTargetPosition < 0 || shoulderTargetPosition > 4095)
  {
    //TODO: throw error
  }

  //convert from 0-4095 to 0-270
  int shoulderTargetAngle = (int)(CONVERSION_FACTOR * shoulderTargetPosition);

  unsigned long currentTimeShoulder = millis();
  int dt = currentTimeShoulder - previousTimeShoulder;//change in time
  previousTimeShoulder = currentTimeShoulder;//update old time

  //error between what it is and what we want
  int shoulderPosition = analogRead(_shoulderPot);
  int error = shoulderTargetPosition - shoulderPosition;


  //integral/sum of error
  integralShoulder += error * dt / 1000.0;//divide by 1000 because dt is ms, adjust for seconds


  //derivative/rate of change of error
  double derivative = 1000.0*(error - previousErrorShoulder) / dt;

  int sum = (int)(KP_SHOULDER * error + KI_SHOULDER * integralShoulder + KD_SHOULDER * derivative);

  previousErrorShoulder = error;

  //TODO:output to servo
  //working w/ full rotational servos
  //90 = No Movement, 0 full speed one direction, 180 full speed other direction

  int output = SERVO_CENTER + sum;

  if (output > 180)
  {
    output = 180;
  }
  if (output < 0)
  {
    output = 0;
  }

  //TODO: write to servo
  analogWrite(_shoulderMotor, output);
}

/*
* Receives the movement info from the adjustArm method
* and converts it into a number the elbow motor can use.
* Has PID for control system.
*/
void moveElbow(int elbowTargetPosition)
{
  if (elbowTargetPosition < 0 || elbowTargetPosition > 4095)
  {
    //TODO: throw error
  }

  //convert from 0-4095 to 0-270
  int elbowTargetAngle = (int)(CONVERSION_FACTOR * elbowTargetPosition);

  unsigned long currentTimeElbow = millis();
  int dt = currentTimeElbow - previousTimeElbow;//change in time
  previousTimeElbow = currentTimeElbow;//update old time

  //error between what it is and what we want
  int elbowPosition = analogRead(_elbowPot);
  int error = elbowTargetPosition - elbowPosition;


  //integral/sum of error
  integralElbow += error * dt / 1000.0;//divide by 1000 because dt is ms, adjust for seconds


  //derivative/rate of change of error
  double derivative = 1000.0*(error - previousErrorElbow) / dt;

  int sum = (int)(KP_ELBOW * error + KI_ELBOW * integralElbow + KD_ELBOW * derivative);

  previousErrorElbow = error;

  //TODO:output to servo
  //working w/ full rotational servos
  //90 = No Movement, 0 full speed one direction, 180 full speed other direction

  int output = SERVO_CENTER + sum;

  if (output > 180)
  {
    output = 180;
  }
  if (output < 0)
  {
    output = 0;
  }

  //TODO: write to servo
  analogWrite(_elbowMotor, output);
}

/*
* Receives the movement info from the adjustArm method
* and converts it into a number the wrist motor can use.
* Has PID for control system.
*/
void moveWristPitch(int wristPitchTargetPosition)
{
  if (wristPitchTargetPosition < 0 || wristPitchTargetPosition > 4095)
  {
    //TODO: throw error
  }

  //convert from 0-4095 to 0-270
  int wristPitchTargetAngle = (int)(CONVERSION_FACTOR * wristPitchTargetPosition);

  unsigned long currentTimeWristPitch = millis();
  int dt = currentTimeWristPitch - previousTimeWristPitch;//change in time
  previousTimeWristPitch = currentTimeWristPitch;//update old time

  //error between what it is and what we want
  int wristPitchPosition = analogRead(_wristPitchPot);
  int error = wristPitchTargetPosition - wristPitchPosition;


  //integral/sum of error
  integralWristPitch += error * dt / 1000.0;//divide by 1000 because dt is ms, adjust for seconds


  //derivative/rate of change of error
  double derivative = 1000.0*(error - previousErrorWristPitch) / dt;

  int sum = (int)(KP_WRIST_PITCH * error + KI_WRIST_PITCH * integralWristPitch + KD_WRIST_PITCH * derivative);

  previousErrorWristPitch = error;

  //TODO:output to servo
  //working w/ full rotational servos
  //90 = No Movement, 0 full speed one direction, 180 full speed other direction

  int output = SERVO_CENTER + sum;

  if (output > 180)
  {
    output = 180;
  }
  if (output < 0)
  {
    output = 0;
  }

  //TODO: write to servo
  analogWrite(_wristPitchMotor, output);
}

/*
* Receives the movement info from the adjustArm method
* and converts it into a number the wrist motor can use.
* Has PID for control system.
*/
void moveWristRoll(int wristRollTargetPosition)
{
  if (wristRollTargetPosition < 0 || wristRollTargetPosition > 4095)
  {
    //TODO: throw error
  }

  //convert from 0-4095 to 0-270
  int wristRollTargetAngle = (int)(CONVERSION_FACTOR * wristRollTargetPosition);

  unsigned long currentTimeWristRoll = millis();
  int dt = currentTimeWristRoll - previousTimeWristRoll;//change in time
  previousTimeWristRoll = currentTimeWristRoll;//update old time

  //error between what it is and what we want
  int wristRollPosition = analogRead(_wristRollPot);
  int error = wristRollTargetPosition - wristRollPosition;


  //integral/sum of error
  integralWristRoll += error * dt / 1000.0;//divide by 1000 because dt is ms, adjust for seconds


  //derivative/rate of change of error
  double derivative = 1000.0*(error - previousErrorWristRoll) / dt;

  int sum = (int)(KP_WRIST_ROLL * error + KI_WRIST_ROLL * integralWristRoll + KD_WRIST_ROLL * derivative);

  previousErrorWristRoll = error;

  //TODO:output to servo
  //working w/ full rotational servos
  //90 = No Movement, 0 full speed one direction, 180 full speed other direction

  int output = SERVO_CENTER + sum;

  if (output > 180)
  {
    output = 180;
  }
  if (output < 0)
  {
    output = 0;
  }

  //TODO: write to servo
  analogWrite(_wristRollMotor, output);
}
