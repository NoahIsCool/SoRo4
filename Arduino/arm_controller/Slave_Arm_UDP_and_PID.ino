#include <Ethernet.h>
#include <EthernetUdp.h>
#include <Servo.h>

Servo myservo;

//UDP stuff

#define DEVICE_ID 1

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xAB, 0xCD, 0xEF, 0x01, 0x23, 0x45
};
IPAddress ip(192, 168, 1, 140);

unsigned int localPort = 2040;      // local port to listen on

// packet related variables
char packetBuffer[30];  // buffer to hold incoming packet
EthernetUDP Udp;

//End UDP stuff

//Arm stuff

// message data to store
int potYaw;
int potShoulder;
int potElbow;
int potWristPitch;
int potWristRoll;
bool buttonClawOpen;
bool buttonClawClose;

//12 bit accuracy = 2^12 = 4096 = 0-4096
//TODO: change to 10 bit
const int MAX_ACCURACY = 4095;
const int MAX_ANGLE = 180;
const double CONVERSION_FACTOR = (double)MAX_ANGLE / (double)MAX_ACCURACY;

//no movement on servo
const int SERVO_CENTER = 90;
const int ARDUINO_IN_VOLT = 1024;

//base angle (perp to ground)
int baseAngle = 135;
//shoulder angle (perpendicular to ground)
int shoulderAngle = 135;
//elbow angle (perpendicular to ground)
int elbowAngle = 135;
//wrist pitch angle (perpendicular to ground)
int wristPitchAngle = 135;


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


//pin values
//TODO: change
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



//End arm stuff

void setup() {
  //UDP stuff
  Ethernet.init(10);  // Most Arduino shields
  Ethernet.begin(mac, ip);

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  // start UDP
  Udp.begin(localPort);

  Serial.print("Max packet size:");
  Serial.println(UDP_TX_PACKET_MAX_SIZE);

  //End UDP stuff

  //Start arm stuff
  myservo.attach(4)

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

  //End arm stuff
}

void loop() {
  if (read_data())
  {
    // debug - print out the values
    Serial.println("Interpreted message data");
    Serial.print(potYaw);
    Serial.print(", ");
    Serial.print(potShoulder);
    Serial.print(", ");
    Serial.print(potElbow);
    Serial.print(", ");
    Serial.print(potWristPitch);
    Serial.print(", ");
    Serial.print(potWristRoll);
    Serial.print(", ");
    Serial.print(buttonClawOpen);
    Serial.print(", ");
    Serial.println(buttonClawClose);
    // TODO: make the values move stuff
  }
  delay(10);
}

bool read_data()
{
  // the message format (14 bytes total):
  // (-127) | Device ID (1) | yaw (high) | yaw (low) | shoulder (high) ...
  // ... | shoulder (low) | elbow (high) | elbow (low) | wrist pitch (high) ...
  // ... | wrist pitch (low) | wrist roll (high) | wrist roll (low) | buttons | hash
  
  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if(packetSize == 0)
  {
    return false;
  }
  
  // read the packet into packetBufffer
  Udp.read(packetBuffer, packetSize);
      
  // serial debug
  Serial.println("Raw data");
  for(int i = 0; i < packetSize; i++)
  {
    Serial.print((int)(packetBuffer[i]));
    Serial.print(", ");
  }
  Serial.print("\n");
  
  if(packetSize != 14)
  {
    Serial.println("Message is wrong length!");
    return false;
  }

  if(packetBuffer[0] != -127)
  {
    Serial.println("Message does not start with -127!");
    return false;
  }

  if(packetBuffer[1] != 1)
  {
    Serial.println("Wrong device id!");
    return false;
  }
  
  char hash = (packetBuffer[2] + packetBuffer[3] + packetBuffer[4] + packetBuffer[5] + packetBuffer[6] + packetBuffer[7] + packetBuffer[8] + packetBuffer[9] + packetBuffer[10] + packetBuffer[11] + packetBuffer[12]) / 11;
  if(hash != packetBuffer[13])
  {
    Serial.println("Wrong hash!");
    return false;
  }

  potYaw = (int)(packetBuffer[2] << 8) + (byte)packetBuffer[3];
  potShoulder = (int)(packetBuffer[4] << 8) + (byte)packetBuffer[5];
  potElbow = (int)(packetBuffer[6] << 8) + (byte)packetBuffer[7];
  potWristPitch = (int)(packetBuffer[8] << 8) + (byte)packetBuffer[9];
  potWristRoll = (int)(packetBuffer[10] << 8) + (byte)packetBuffer[11];

  buttonClawOpen = ((packetBuffer[12] & 0x01) > 0);  // 2^0 bit
  buttonClawClose = ((packetBuffer[12] & 0x02) > 0); // 2^1 bit

  moveBase(potYaw);
  moveShoulder(potShoulder);
  moveElbow(potElbow);
  moveWristPitch(potWristPitch);
  moveWristRoll(potWristRoll);



  return true;
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

  //convert from 0-4095 to 0-180
  int baseTargetAngle = (int)(CONVERSION_FACTOR * baseTargetPosition);

  unsigned long currentTimeBase = millis();
  int dt = currentTimeBase - previousTimeBase;//change in time
  previousTimeShoulder = currentTimeBase;//update old time

  //error between what it is and what we want
  int basePosition = analogRead(_basePot);
  int baseAngle = basePosition/ARDUINO_IN_VOLT;
  int error = baseTargetAngle - baseAngle;


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

  //convert from 0-4095 to 0-180
  int shoulderTargetAngle = (int)(CONVERSION_FACTOR * shoulderTargetPosition);

  unsigned long currentTimeShoulder = millis();
  int dt = currentTimeShoulder - previousTimeShoulder;//change in time
  previousTimeShoulder = currentTimeShoulder;//update old time

  //error between what it is and what we want
  int shoulderPosition = analogRead(_shoulderPot);
  int shoulderAngle = shoulderPosition/ARDUINO_IN_VOLT;
  int error = shoulderTargetAngle - shoulderAngle;


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

  //convert from 0-4095 to 0-180
  int elbowTargetAngle = (int)(CONVERSION_FACTOR * elbowTargetPosition);

  unsigned long currentTimeElbow = millis();
  int dt = currentTimeElbow - previousTimeElbow;//change in time
  previousTimeElbow = currentTimeElbow;//update old time

  //error between what it is and what we want
  int elbowPosition = analogRead(_elbowPot);
  int elbowAngle = elbowPosition/ARDUINO_IN_VOLT;
  int error = elbowTargetAngle - elbowAngle;


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

  //convert from 0-4095 to 0-180
  int wristPitchTargetAngle = (int)(CONVERSION_FACTOR * wristPitchTargetPosition);

  unsigned long currentTimeWristPitch = millis();
  int dt = currentTimeWristPitch - previousTimeWristPitch;//change in time
  previousTimeWristPitch = currentTimeWristPitch;//update old time

  //error between what it is and what we want
  int wristPitchPosition = analogRead(_wristPitchPot);
  int wristPitchAngle = wristPitchPosition/ARDUINO_IN_VOLT;
  int error = wristPitchTargetAngle - wristPitchAngle;


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

  //convert from 0-4095 to 0-180
  int wristRollTargetAngle = (int)(CONVERSION_FACTOR * wristRollTargetPosition);

  unsigned long currentTimeWristRoll = millis();
  int dt = currentTimeWristRoll - previousTimeWristRoll;//change in time
  previousTimeWristRoll = currentTimeWristRoll;//update old time

  //error between what it is and what we want
  int wristRollPosition = analogRead(_wristRollPot);
  int wristRollAngle = wristRollPosition/ARDUINO_IN_VOLT;
  int error = wristRollTargetAngle - wristRollAngle;


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
