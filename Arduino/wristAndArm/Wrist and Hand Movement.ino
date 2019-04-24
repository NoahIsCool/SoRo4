// Code kill switch (done) and stow arm (stow arm away, take a lok at zack code)

#include <Servo.h>

const int y = A1; // analog pin connected to VRy
const int handOpen = 2; // Digital pin for button
const int handClose = 5; // Digital pin for button
const int kill = 6;
bool isDead = false;

bool wasPos = true;
bool wasNeg = true;

Servo wristServo;
Servo handServo;


/*For preventing jerky movement, make sure the difference between
   two read analog inputs does not exceed 2 or more
*/

/*
 * Setup for the folowing:
 *    button handOpen
 *    button handClose
 *    button kill
 *    wristServo
 *    handServo
 */
void setup() {
  pinMode(handOpen, INPUT);
  digitalWrite(handOpen, LOW);

  pinMode(handClose, INPUT);
  digitalWrite(handClose, LOW);

  pinMode(kill, INPUT);
  digitalWrite(kill, LOW);

  wristServo.attach(3);
  handServo.attach(4);

  Serial.begin(9600);
}

void hand() {
  if (digitalRead(handOpen) == HIGH) {
    handServo.write(95);
    }
  else if (digitalRead(handClose) == HIGH) {
    handServo.write(85);
  }
  else {
    handServo.write(90);
  }
}

void wrist() {
  int gradual = analogRead(y);

  if (gradual < 501) {

    if (wasPos == true)
    {
      wasNeg = true;
      wasPos = false;
      wristServo.write(90);
      delay(125);
    }

    int gradualDown = map(gradual, 500, 15, 88, 20 );
    wristServo.write(gradualDown);
    wasPos = false;

  }
  else if (gradual  > 510) {

    if (wasNeg == true)
    {
      wasNeg = false;
      wasPos = true;
      wristServo.write(90);
      delay(125);
    }

    int gradualUp = map(gradual, 510, 1023, 92, 160);
    wristServo.write(gradualUp);
    wasPos = true;

  }
  else {
    wristServo.write(90);
  }
}

void killSwitch(){
  if (digitalRead(kill) == HIGH and isDead == false){
    while(1){
      handServo.write(90);
      wristServo.write(90);
      isDead = true;
    }
  }
}


void loop() {
  wrist();
  hand();
  killSwitch();
}
