#include <EtherCard.h>
#include <IPAddress.h>
#include <Servo.h>

//NOTE: UDP values are 0 to 1023
//               (may be changed to 0 to 4095)

//Old: Potentiometer values are 0 to 1023
//Elbow potentiometer ranges from 479(stowed) to 401(fully extended)
//TODO: find the max ranges for all other potentiometers and update this in every function
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

//TODO: write everything for the claw


Servo yaw,shoulder,elbow,wristPitch;

//UDP stuff

#define DEVICE_ID 1

// ethernet interface ip address
static byte myip[] = { 192,168,1,200 };
// gateway ip address
static byte gwip[] = { 192,168,1,1 };
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
int wristRoll; //no pot for this right now. TODO: add the pot back in for this when its added for auton
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

//Not sure if these are still used for this specific program
//yaw (base) angle (perp to ground)
int yawAngle = 135;
//shoulder angle (perpendicular to ground)
int shoulderAngle = 135;
//elbow angle (perpendicular to ground)
int elbowAngle = 135;
//wrist pitch angle (perpendicular to ground)
int wristPitchAngle = 135;

//pin values
//TODO: change
char _yawMotor = D2;//Pin for yaw (base) motor
char _shoulderMotor = D3;//Pin for shoulder motor
char _elbowMotor = D4;//Pin for elbow motor
char _wristPitchMotor = D7;//Pin for wrist pitch motor
char _wristRollMotor = D6;//Pin for wrist motor

char _yawPot = A0;//Pin for yaw (base) potentiometer
char _shoulderPot = A1;//Pin for shoulder potentiometer
char _elbowPot = A2;//Pin for elbow potentiometer
char _wristPitchPot = A3;//Pin for wrist pitch potentiometer
//char _wristRollPot = 1;//Pin for wrist roll potentiometer



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
  /*Serial.println("Raw data");
  for(int i = 0; i < len; i++)
  {
    Serial.print((int)(data[i]));
    Serial.print(", ");
  }
  Serial.print("\n");*/
  
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
  wristRoll = (int)(data[10] << 8) + (byte)data[11];

  buttonClawOpen = ((data[12] & 0x01) > 0);  // 2^0 bit
  buttonClawClose = ((data[12] & 0x02) > 0); // 2^1 bit

  moveYaw(potYaw);
  moveShoulder(potShoulder);
  moveElbow(potElbow);
  moveWristPitch(potWristPitch);
  moveWristRoll(wristRoll);



  return true;
}

/*
* Receives the movement info from the adjustArm method
* and converts it into a number the yaw (base) motor can use.
*/
void moveYaw(int yawTargetPosition)
{
  if (yawTargetPosition < 0 || yawTargetPosition > 1023)
  {
    //TODO: throw error
    Serial.print("ERROR: yaw target position is out of range");
    return;
  }

  //convert. TODO: add the conversion factor, the offset and change the 180 and 0 below to the max and min positions
  float yawConversionFactor;
  int yawOffset;
  
  int yawTargetAngle = yawConversionFactor * yawTargetPosition + yawOffset;
  int currentPotValue = analogRead(_yawPot);

  int diff = desiredPotValue - currentPotValue;

  int output = 90;

  Serial.print("desired: ");
  Serial.print(desiredPotValue);
  Serial.print(" actual: ");
  Serial.println(currentPotValue);
  if (diff > 10 && desiredPotValue < 180 && currentPotValue < 180)
  {
     output = 120;
     Serial.println("moving clockwise");
  }
  else if (diff < -10 && desiredPotValue > 0 && currentPotValue > 0)
  {
     output = 60;
     Serial.println("moving counter clockwise");
  }
  else{
    Serial.println("perfectly ballanced. As all things should be");
  }

  //TODO: write to servo
  analogWrite(_yawMotor, output);
}

/*
* Receives the movement info from the adjustArm method
* and converts it into a number the shoulder motor can use.
*/
void moveShoulder(int shoulderTargetPosition)
{
  if (shoulderTargetPosition < 0 || shoulderTargetPosition > 1023)
  {
    //TODO: throw error
    Serial.print("ERROR: shoulder target position is out of range");
    return;
  }

  //convert. TODO: add the conversion factor, the offset and change the 480 and 400 below to the max and min positions
  float shoulderConversionFactor;
  float shoulderOffset;
  
  int desiredPotValue = shoulderConversionFactor*shoulderTargetPosition + shoulderOffset;
  int currentPotValue = analogRead(_shoulderPot);
  
  int diff = desiredPotValue - currectPotValue;
  
  int output = 90;
  
  Serial.print("desired: ");
  Serial.print(desiredPotValue);
  Serial.print(" actual: ");
  Serial.println(currentPotValue);
  if (diff > 10 && desiredPotValue < 480 && currentPotValue < 480)
  {
     output = 120;
     Serial.println("moving clockwise");
  }
  else if (diff < -10 && desiredPotValue > 400 && currentPotValue > 400)
  {
     output = 60;
     Serial.println("moving counter clockwise");
  }
  else{
    Serial.println("perfectly balanced. As all things should be");
  }

  //TODO: write to servo
  analogWrite(_shoulderMotor, output);
}

/*
* Receives the movement info from the adjustArm method
* and converts it into a number the elbow motor can use.
*/
void moveElbow(int elbowTargetPosition)
{
  if (elbowTargetPosition < 0 || elbowTargetPosition > 1023)
  {
    //TODO: throw error
    Serial.print("ERROR: elbow target position is out of range");
    return;
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

  float elbowConversionFactor = -26.0/341;
  int elbowOffset = 190;//479;

  int desiredPotValue = elbowConversionFactor*elbowTargetPosition + elbowOffset;
  int currentPotValue = analogRead(_elbowPot);

  int diff = desiredPotValue - currentPotValue;

  int output = 90;

  Serial.print("desired: ");
  Serial.print(desiredPotValue);
  Serial.print(" actual: ");
  Serial.println(currentPotValue);
  if (diff > 10 && desiredPotValue < 480 && currentPotValue < 480)
  {
     output = 120;
     Serial.println("moving clockwise");
  }
  else if (diff < -10 && desiredPotValue > 400 && currentPotValue > 400)
  {
     output = 60;
     Serial.println("moving counter clockwise");
  }
  else{
    Serial.println("perfectly balanced. As all things should be");
  }

  //TODO: write to servo
  analogWrite(_elbowMotor, output);
}

/*
* Receives the movement info from the adjustArm method
* and converts it into a number the wrist motor can use.
*/
void moveWristPitch(int wristPitchTargetPosition)
{
  if (wristPitchTargetPosition < 0 || wristPitchTargetPosition > 1023)
  {
    //TODO: throw error
    Serial.print("ERROR: wrist target position is out of range");
    return;
  }

  //convert. TODO: add the conversion factor, the offset and change the 180 and 0 below to the max and min positions
  float wristPitchConversionFactor;
  int wristPitchOffset;
  int desiredPotValue = wristPitchConversionFactor * wristPitchTargetPosition + wristPitchOffset;
  int currentPotValue = analogRead(_wristPitchPot);
  
  int diff = desiredPotValue - currentPotValue;

  int output = 90;

  Serial.print("desired: ");
  Serial.print(desiredPotValue);
  Serial.print(" actual: ");
  Serial.println(currentPotValue);
  if (diff > 10 && desiredPotValue < 180 && currentPotValue < 180)
  {
     output = 120;
     Serial.println("moving clockwise");
  }
  else if (diff < -10 && desiredPotValue > 0 && currentPotValue > 0)
  {
     output = 60;
     Serial.println("moving counter clockwise");
  }
  else{
    Serial.println("perfectly balanced. As all things should be");
  }


  //TODO: write to servo
  analogWrite(_wristPitchMotor, output);
}

/*
* Receives the movement info from the adjustArm method
* and converts it into a number the wrist motor can use.
*/
void moveWristRoll(int wristRollTargetSpeed)
{
  if (wristRollTargetPosition < 0 || wristRollTargetSpeed > 1023)
  {
    //TODO: throw error
    Serial.print("ERROR: wrist target position is out of range");
    return;
  }

  //convert from 0-1023 to 0-180
  int wristRollTargetSpeed = (int)(CONVERSION_FACTOR * wristRollTargetSpeed);
  int currentSpeed = analogRead(_wristRollMotor);
  if(currentSpeed > 90 && wristRollTargetSpeed < 90 || currentSpeed < 90 && wristRollTargetSpeed > 90)
  {
    analogWrite(_wristRollMotor, 90);
    sleep(100);
  }

  //TODO: write to servo
  analogWrite(_wristRollMotor, wristRollTargetSpeed);
}
