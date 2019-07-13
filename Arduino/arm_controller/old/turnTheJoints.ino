

#include <Servo.h> //Imports the servo libraries

Servo shoulder,elbow,wristPitch;

int targetAngle; //Variable used for the POT that is the target


/*
 * myservo -> the motor/motorcontroller/servo that you want to control for that joint
 * targetAngle -> The amount of the potentiometer's range you want to move to
 * anglePort -> the port that the wiper of the potentiometer is attached to
 * power -> The speed you want the motor to go to that angle with (from 0% - 100%)
 * idiotProof -> does it let the user go above 50% power? (DEFAULT is NO)
 * 
 * Example:
 * moveToAngle(servoPort, 90, 0, 50);
 * will go to the 90/1024 of the range of the potentiometer using the analog 0 port for the POT and running the motor at 50% power
 */
int moveToAngle(Servo myservo, int targetAngle, int anglePort, double power, int idiotProof = 1) {

  int lowLimit = 100; //The limit for the joint at the lower values of the potentiometer
  int highLimit = 1010; //The limit for the joint at the higher values of the potentiometer
  int angleTolerance = 40; //The amount out of 1024 that the arm is allowed to deviate from the correct angle
  
  if(idiotProof == 1){
    if(power>50){
      Serial.println("DON'T PUT THE POWER AT >50% for safety concerns");
      power = 50;
    }
  }
  else{
    Serial.println("Safety mode disabled");
  }

  int rotationSpeed = power/100.0 * 90.0;
  //The current angle that the arm is at.
  int angle = analogRead(anglePort); 

  //If the difference in the angles is more than 40/1024
  if(abs(angle-targetAngle)>40){ 

     //While the joint is not far enough
     while(angle>targetAngle){

        //Set the motor speed in the direction that gets us to the target
        myservo.write(90+rotationSpeed);

        
        //Reread the angle
        angle = analogRead(anglePort);
        
        
        
        Serial.println("Moving the arm forward");

        //Check if the arm is past the limits
        while(angle>highLimit || angle<lowLimit){ 

          

          //Stop the motors
          myservo.write(90);

          //Read the angle again
          angle = analogRead(anglePort);
          
          Serial.println("You are pushing the arm too far");
          
        }

     }

     //While the arm is too far past the angle
     while(angle<targetAngle){

      //Set the motor speed in the direction that gets the joint the the correct angle
      myservo.write(90-90*abs(angle-targetAngle)/1024);

      
      Serial.println("Moving the arm backward");

      //Reread the joint's angle
      angle = analogRead(anglePort);

      while(angle>highLimit || angle<lowLimit){

        //Reread the angle
        angle = analogRead(anglePort);

        //Set the motors to stop
        myservo.write(90);

        
        Serial.println("You are pushing the arm too far");
        
      }
     
     }
  }
  else{

    //If we are in the position we want, stop the motors
    myservo.write(90);
    
    Serial.println("Target Aquired!");
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


void loop(){
  //=====================================THESE ARE RANDOM VALUES FOR EXAMPLE PURPOSES, ADD CODE TO RETRIEVE THESE FROM THE MASTER ARM===================================
  int targetShoulder = 90; 
  int targetElbow = 70; 
  int targetWristPitch = 120; 
  //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^THESE ARE RANDOM VALUES FOR EXAMPLE PURPOSES, ADD CODE TO RETRIEVE THESE FROM THE MASTER ARM^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

  
  int lowLimit = 100; //The limit for the joint at the lower values of the potentiometer
  int highLimit = 1010; //The limit for the joint at the higher values of the potentiometer
  int angleTolerance = 40; //The amount out of 1024 that the arm is allowed to deviate from the correct angle

  
  moveToAngle(shoulder, targetAngle, 0, 5);
  moveToAngle(elbow, targetAngle, 0, 5);
  moveToAngle(wristPitch, targetAngle, 0, 5);
  
}
