#include <SoftwareSerial.h>

//README
/*
 * Red    - 5v power
 * Black  - ground
 * Blue   - lidar output (arduino rx pin) 3.3v
 * White  - lidar input (arduino tx pin) 3.3v
 * 
 * This should be good to go just do not plug the
 * white wire into the arduino. We dont need to write
 * anything to it and if you do with out a voltage
 * divider, it will most likly break the lidar
 * 
 */

SoftwareSerial lidar(8,9);

unsigned int lowerByte = 0;
unsigned int upperByte = 0;
unsigned int lowerStrength = 0;
unsigned int upperStrength = 0;
unsigned int averageDistance = 0;
unsigned int averageStrength = 0;
int numSampled = 0;

void setup() {
  Serial.begin(9600);
  delay(10);
  while(!Serial){}
  lidar.begin(115200);
  //lidar.listen();
}

// the loop function runs over and over again forever
void loop() {
  while(lidar.available()){
    if(lidar.read() == 0x59 && lidar.read() == 0x59){
        lowerByte = lidar.read();
        upperByte = lidar.read();
        lowerStrength = lidar.read();
        upperStrength = lidar.read();
        averageDistance += (upperByte << 8) + lowerByte;
        //averageStrength += (upperStrength << 8) + lowerStrength;
        numSampled++;
        lidar.read();
        lidar.read();
    }else{
      lidar.read();
    }
  }

  //0x03E8 == 1000cm
  //no idea what the strength is...
  //the higher the better?
  if(numSampled == 10){
    averageDistance /= 10;
    averageStrength /= 10;
    //cm = averageDistance * 4.310344828;
    //Serial.println(averageDistance,HEX);
    Serial.print(averageDistance);
    Serial.println(" cm");
    //Serial.println(averageStrength);
    averageDistance = 0;
    numSampled = 0;
  }
  //Serial.println;
}
