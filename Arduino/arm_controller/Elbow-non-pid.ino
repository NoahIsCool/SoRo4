#include <EtherCard.h>
#include <IPAddress.h>
#include <Servo.h>

//NOTE: UDP values are 0 to 1023
//               (may be changed to 0 to 4095)

//Old: Potentiometer values are 0 to 1023
//Elbow potentiometer ranges from 479(stowed) to 401(fully extended)
//Elbow pin A2


//Servo values are 0 to 180

//TODO: Wrist pitch - changing directions too quickly causes sparking. Change PID or find
//something else so that it doesn't change direction to quickly (say every 1/4 second)
//Consider something like this:
//int speed = 100;
//void changeSpeed(int newSpeed)
//{
//   while(speed != newSpeed)
//   {
//       if(newSpeed > speed)
//            speed += 1;
//       else
//            speed -= 1;
//       sleep(10);
//   }
//}


Servo yaw,shoulder,elbow,wristPitch;

//UDP stuff

#define DEVICE_ID 1

// ethernet interface ip address
static byte myip[] = { 192,168,0,200 };
// gateway ip address
static byte gwip[] = { 192,168,0,1 };
// ethernet mac address - must be unique on your network
static byte mymac[] = { 0xAB, 0xCD, 0xEF, 0x01, 0x23, 0x45 };

unsigned int localPort = 2040;      // local port to listen on

byte Ethernet::buffer[500]; // tcp/ip send and receive buffer

//End UDP stuff

//Arm stuff

// message data to store
int potYaw;//Base
int potShoulder;
int potElbow;
int potWristPitch;
int potWristRoll;
bool buttonClawOpen;
bool buttonClawClose;

//10 bit accuracy = 2^10 = 1024 = 0-1023
//TODO: may change to 12 bit
const int MAX_ACCURACY = 1023;
const int MAX_ANGLE = 180;
const double CONVERSION_FACTOR = (double)MAX_ANGLE / (double)MAX_ACCURACY;

//no movement on servo
const int SERVO_CENTER = 90;
const int ARDUINO_IN_VOLT = 1024;

//yaw (base) angle (perp to ground)
int yawAngle = 135;
//shoulder angle (perpendicular to ground)
int shoulderAngle = 135;
//elbow angle (perpendicular to ground)
int elbowAngle = 135;
//wrist pitch angle (perpendicular to ground)
int wristPitchAngle = 135;


//PID constants
//TODO: tune
const double KP_YAW = 1;
const double KI_YAW = 0;
const double KD_YAW = 0;

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
unsigned long previousTimeYaw = 0;
int previousErrorYaw = 0;
double integralYaw = 0;

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
char _yawMotor = A0;//Pin for yaw (base) motor
char _shoulderMotor = A1;//Pin for shoulder motor
char _elbowMotor = 4;//Pin for elbow motor
char _wristPitchMotor = A3;//Pin for wrist pitch motor
char _wristRollMotor = A4;//Pin for wrist motor

char _yawPot = A5;//Pin for yaw (base) potentiometer
char _shoulderPot = A6;//Pin for shoulder potentiometer
char _elbowPot = A2;//Pin for elbow potentiometer
char _wristPitchPot = 0;//Pin for wrist pitch potentiometer
char _wristRollPot = 1;//Pin for wrist roll potentiometer



//End arm stuff

void setup() {
  Serial.begin(9600);
  
  //UDP stuff
  ether.begin(sizeof Ethernet::buffer, mymac, SS);
  ether.staticSetup(myip, gwip);
  ether.udpServerListenOnPort(&read_data, localPort);
  //End UDP stuff

  //Start arm stuff
  yaw.attach(1);//TODO: update this line
  shoulder.attach(4);
  elbow.attach(7);
  wristPitch.attach(8);

  //set the pinmode of the motor ports to be output
  pinMode(_yawMotor, OUTPUT);
  pinMode(_shoulderMotor, OUTPUT);
  pinMode(_elbowMotor, OUTPUT);
  pinMode(_wristPitchMotor, OUTPUT);
  pinMode(_wristRollMotor, OUTPUT);

  //set the pinmode of the potentiometer ports to be input
  pinMode(_yawPot, INPUT);
  pinMode(_shoulderPot, INPUT);
  pinMode(_elbowPot, INPUT);
  pinMode(_wristPitchPot, INPUT);
  pinMode(_wristRollPot, INPUT);

  //End arm stuff
}

void loop() {
   // this must be called for ethercard functions to work.
   ether.packetLoop(ether.packetReceive());
}

void read_data(uint16_t dest_port, uint8_t src_ip[IP_LEN], uint16_t src_port, const char *data, uint16_t len)
{
  IPAddress src(src_ip[0],src_ip[1],src_ip[2],src_ip[3]);
  
  // the message format (14 bytes total):
  // (-127) | Device ID (1) | yaw(base) (high) | yaw(base) (low) | shoulder (high) ...
  // ... | shoulder (low) | elbow (high) | elbow (low) | wrist pitch (high) ...
  // ... | wrist pitch (low) | wrist roll (high) | wrist roll (low) | buttons | hash
      
  // serial debug
  Serial.println("Raw data");
  for(int i = 0; i < len; i++)
  {
    Serial.print((int)(data[i]));
    Serial.print(", ");
  }
  Serial.print("\n");
  
  if(len != 14)
  {
    Serial.println("Message is wrong length!");
    return;
  }

  if(data[0] != -127)
  {
    Serial.println("Message does not start with -127!");
    return;
  }

  if(data[1] != 1)
  {
    Serial.println("Wrong device id!");
    return;
  }
  
  char hash = (data[2] + data[3] + data[4] + data[5] + data[6] + data[7] + data[8] + data[9] + data[10] + data[11] + data[12]) / 11;
  if(hash != data[13])
  {
    Serial.println("Wrong hash!");
    return;
  }

  potYaw = (int)(data[2] << 8) + (byte)data[3];
  potShoulder = (int)(data[4] << 8) + (byte)data[5];
  potElbow = (int)(data[6] << 8) + (byte)data[7];
  potWristPitch = (int)(data[8] << 8) + (byte)data[9];
  potWristRoll = (int)(data[10] << 8) + (byte)data[11];

  buttonClawOpen = ((data[12] & 0x01) > 0);  // 2^0 bit
  buttonClawClose = ((data[12] & 0x02) > 0); // 2^1 bit

  //moveYaw(potYaw);
  //moveShoulder(potShoulder);
  moveElbow(potElbow);
  //moveWristPitch(potWristPitch);
  //moveWristRoll(potWristRoll);



  return true;
}

/*
* Receives the movement info from the adjustArm method
* and converts it into a number the yaw (base) motor can use.
* Has PID for control system
*/
void moveYaw(int yawTargetPosition)
{
  if (yawTargetPosition < 0 || yawTargetPosition > 1023)
  {
    //TODO: throw error
  }

  //convert from UDP to servo (0-1023 to 0-180)
  int yawTargetAngle = (int)(CONVERSION_FACTOR * yawTargetPosition);

  unsigned long currentTimeYaw = millis();
  int dt = currentTimeYaw - previousTimeYaw;//change in time
  previousTimeYaw = currentTimeYaw;//update old time

  //error between what it is and what we want
  int yawPosition = analogRead(_yawPot);
  int yawAngle = yawPosition/ARDUINO_IN_VOLT;
  int error = yawTargetAngle - yawAngle;


  //integral/sum of error
  integralYaw += error * dt / 1000.0;//divide by 1000 because dt is ms, adjust for seconds


  //derivative/rate of change of error
  double derivative = 1000.0*(error - previousErrorYaw) / dt;

  int sum = (int)(KP_YAW * error + KI_YAW * integralYaw + KD_YAW * derivative);

  previousErrorYaw = error;

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
  analogWrite(_yawMotor, output);
}

/*
* Receives the movement info from the adjustArm method
* and converts it into a number the shoulder motor can use.
* Has PID for control system.
*/
void moveShoulder(int shoulderTargetPosition)
{
  if (shoulderTargetPosition < 0 || shoulderTargetPosition > 1023)
  {
    //TODO: throw error
  }

  //convert from 0-1023 to 0-180
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
  if (elbowTargetPosition < 0 || elbowTargetPosition > 1023)
  {
    //TODO: throw error
  }

//  if(elbowTargetPosition < 401){
//    elbowTargetPosition = 401;
//  }
//  if(elbowTargetPosition > 479){
//    elbowTargetPosition = 479;
//  }

  //old convert from 0-1023 to 0-180
  //int elbowTargetAngle = (int)(CONVERSION_FACTOR * elbowTargetPosition);

  //convert from 0-1023 to 479-401

  int elbowConversionFactor = (int) -26/341;
  int elbowOffset = 479;

  int desiredPotValue = elbowConversionFactor*elbowTargetPosition + elbowOffset;
  int currentPotValue = analogRead(_shoulderPot);

  int diff = desiredPotValue - currentPotValue;

  int output = 90;
 
  if (diff > 10 && desiredPotValue < 480 && currentPotValue < 480)
  {
     output = 120;
  }
  else if (diff < 10 && desiredPotValue > 400 && currentPotValue > 400)
  {
     output = 60;
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
  if (wristPitchTargetPosition < 0 || wristPitchTargetPosition > 1023)
  {
    //TODO: throw error
  }

  //convert from 0-1023 to 0-180
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
  if (wristRollTargetPosition < 0 || wristRollTargetPosition > 1023)
  {
    //TODO: throw error
  }

  //convert from 0-1023 to 0-180
  /*int wristRollTargetAngle = (int)(CONVERSION_FACTOR * wristRollTargetPosition);

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
  }*/

  //TODO: write to servo
  analogWrite(_wristRollMotor, wristRollTargetPosition);
}
