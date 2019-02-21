#include <Servo.h>
int targetAngle = 90;
String buffer;
Servo myservo;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  myservo.attach(4);
  Serial.println("REV MY ENGINES VROOM VROOM");
}

void loop() {
  // put your main code here, to run repeatedly:
  targetAngle = analogRead(A1);
  int angle = analogRead(A0);

  
  
  if(abs(angle-targetAngle)>40){
    
     while(angle>targetAngle){
        
        myservo.write(90+90*abs(angle-targetAngle)/1024);
        Serial.println("4TH");
        angle = analogRead(A0);
        targetAngle = analogRead(A1);
        //Serial.println("Target: %d",targetAngle);
        //Serial.println("Current: %d",angle);

        while(angle>1010 || angle<100){
          angle = analogRead(A0);
          myservo.write(90);
          Serial.println("Daddy stop it");
          
        }

     }

     while(angle<targetAngle){
      myservo.write(90-90*abs(angle-targetAngle)/1024);
      Serial.println("bACK");
      angle = analogRead(A0);
      targetAngle = analogRead(A1);
      Serial.print("Target: ");
      Serial.println(targetAngle);
      //Serial.println("Current: %d",angle);
      
      while(angle>1010 || angle<600){
        angle = analogRead(A0);
        myservo.write(90);
        Serial.println("Daddy stop it");
        
      }
     
     }
  }
  else{
    myservo.write(90);
    Serial.println("We goood gurl");
  }
  
}
