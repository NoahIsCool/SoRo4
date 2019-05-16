#include <Servo.h> //Imports the servo libraries

Servo shoulder, elbow, wristPitch;
int masterAngle1 = A0;
int masterAngle2 = A2;
int masterAngle3 = A4;
int slaveAngle1 = A1;
int slaveAngle2 = A3; //Variable used for the POT that is the target
int slaveAngle3 = A5;

/*
   myservo -> the motor/motorcontroller/servo that you want to control for that joint
   master -> The amount of the potentiometer's range you want to move to
   slave -> the port that the wiper of the potentiometer is attached to
   power -> The speed you want the motor to go to that angle with (from 0% - 100%)
   idiotProof -> does it let the user go above 50% power? (DEFAULT is NO)

   Example:
   moveToAngle(servoPort, 90, 0, 50);
   will go to the 90/1024 of the range of the potentiometer using the analog 0 port for the POT and running the motor at 50% power
*/
int moveToAngle(Servo myservo, int masterPin, int slavePin, double power, int idiotProof = 1) {

  int lowLimit = 0; //The limit for the joint at the lower values of the potentiometer
  int highLimit = 1010; //The limit for the joint at the higher values of the potentiometer
  int angleTolerance = 40; //The amount out of 1024 that the arm is allowed to deviate from the correct angle

  if (idiotProof == 1) {
    if (power > 50) {
      Serial.println("DON'T PUT THE POWER AT >50% for safety concerns");
      power = 50;
    }
  }
  else {
    Serial.println("Safety mode disabled");
  }

  int rotationSpeed = power / 100.0 * 90.0;

  //The current master that the arm is at.
  int master = analogRead(masterPin);
  int slave = analogRead(slavePin);

  //If the difference in the angles is more than 40/1024
  if (abs(master - slave) > angleTolerance) {

    //While the joint is not far enough
    while (master > slave) {

      //Set the motor speed in the direction that gets us to the target
      myservo.write(90 + 90 * abs(master - slave) / 1024);

      //Reread the pins
      master = analogRead(masterPin);
      slave = analogRead(slavePin);

      Serial.print("Moving the arm forward    ");

      //Check if the arm is past the limits
      while (master > highLimit || master < lowLimit) {

        //Read the pins again
        master = analogRead(masterPin);

        //Stop the motors
        myservo.write(90);

        Serial.println("You are pushing the arm too far");
        Serial.print("master: ");
        Serial.print(master);
        Serial.print("  slave: ");
        Serial.println(slave);
      }

      Serial.print("master: ");
      Serial.print(master);
      Serial.print("  slave: ");
      Serial.println(slave);
    }

    //While the arm is too far past the master
    while (master < slave) {

      //Set the motor speed in the direction that gets the joint the the correct angle
      myservo.write(90 - 90 * abs(master - slave) / 1024);

      Serial.print("Moving the arm backward   ");

      //Reread the joint's angle
      master = analogRead(masterPin);

      slave = analogRead(slavePin);

      while (master > highLimit || master < lowLimit) {

        //Reread the angle
        master = analogRead(masterPin);

        //Set the motors to stop
        myservo.write(90);

        Serial.println("You are pushing the arm too far");

        Serial.println("You are pushing the arm too far move it back bucko!");
        Serial.print("master: ");
        Serial.print(master);
        Serial.print("  slave: ");
        Serial.println(slave);
      }

      Serial.print("master: ");
      Serial.print(master);
      Serial.print("  slave: ");
      Serial.println(slave);
    }
  }
  else {
    //If we are in the position we want, stop the motors
    myservo.write(90);
    Serial.print("Target Aquired!           ");
    Serial.print("master: ");
    Serial.print(master);
    Serial.print("  slave: ");
    Serial.println(slave);
  }
}

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//----------------------END OF THE FANCY FUNCTIONS-------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------

void setup() {
  //Start up a serial connection on Baud 9600
  Serial.begin(9600);

  //Attach the servo object to port 4
  shoulder.attach(4);
  elbow.attach(7);
  wristPitch.attach(8);

  Serial.println("Starting Program...");
}

void loop() {
  //moveToAngle(shoulder, masterAngle1, slaveAngle1, 5);
  //moveToAngle(elbow, masterAngle2, slaveAngle2, 5);
  moveToAngle(wristPitch, masterAngle3, slaveAngle3, 5);
}
