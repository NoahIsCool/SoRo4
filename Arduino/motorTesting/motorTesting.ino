#include <Servo.h>

Servo base;
Servo elbow;
Servo wrist;

int switchMotorPin = 5;

Servo *currentServo;

bool wasPositive = true;
bool wasNegative = true;

#define BASE 0
#define ELBOW 1
#define WRIST 2

int currentMotor = ELBOW;

void setup() {
  Serial.begin(9600);
  base.attach(2);
  elbow.attach(3);
  wrist.attach(4);

  //start with elbow
  currentServo = &elbow;
}

void loop() {
  //check to see if we should switch motors
  if(digitalRead(switchMotorPin)){
    if(currentMotor == BASE){
      currentMotor = ELBOW;
      currentServo = &elbow;
    }else if(currentMotor == ELBOW){
      currentMotor = WRIST;
      currentServo = &wrist;
    }else{
      currentMotor = BASE;
      currentServo = &base;
    }

    while(digitalRead(switchMotorPin));
  }
  
  int gradual = analogRead(A0);

  if(gradual < 501){
    if(wasPositive){
      wasNegative = true;
      wasPositive = false;
      currentServo->write(90);
      delay(125);
    }

    int gradualDown = map(gradual,500,15,88,20);
    currentServo->write(gradualDown);
    wasPositive = false;
  }else if(gradual > 510){
    if(wasNegative){
      wasNegative = false;
      wasPositive = true;
      currentServo->write(90);
      delay(125);
    }
    int gradualUp = map(gradual,510,1023,92,160);
    currentServo->write(gradualUp);
    wasPositive = true;
  }else{
    currentServo->write(90);
  }
}
