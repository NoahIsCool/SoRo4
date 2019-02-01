#include <Servo.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

// pin mapping
/*
     [],pin  front   [],pin
     0,4     0-|-0   3,5
               |
     1,3     0-|-0   4,6
               |
     2,7     0-|-0   5,2

     gimbal pan: 8
     gimbal tilt: 9
*/

const char DEVICE_ID = 0;

Servo wheel[6];
Servo gimbal_pan, gimbal_tilt;
Servo disk;
const int diskID = 16;// TBD
char myHash = 0;
char serialHash = 0;
char pan = 0;
char tiltByte = 0;
char leftWheels = 0;
char rightWheels = 0;
char overdrive = 0;
char tilt = 90;

char incomingByte;
int bytesRead = 0;

char serResp[] = " 1,190,200!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";

bool inTransmission = false;

//ethernet data
byte mac[] = {
  0x69,0xFF,0xAD,0x9F,0x45,0x92
};
//IPaddress. lets use a static address
IPAddress ip(10,0,0,10);
EthernetUDP udp;
unsigned int port = 4547;
//storage for jetson ip
IPAddress jetson;
unsigned int jetsonPort = 4548;
char udpPacket[7];

//setup wheels
void setup() {
  wheel[0].attach(2);
  wheel[1].attach(3);
  wheel[2].attach(4);
  wheel[3].attach(5);
  wheel[4].attach(6);
  wheel[5].attach(7);
  // 8 is an evil number, avoid at all cost
  gimbal_pan.attach(10);
  gimbal_tilt.attach(9);
  disk.attach(diskID);
  //Serial.begin(9600);
  //need to figure out the CS pin for the shield but lets say 10
  //may not need this
  //Ethernet.init(10);
  //start up udp socket
  Ethernet.begin(mac,ip);
  udp.begin(port);
  
  //wait for the init message
  bool connected = false;
  int packetSize = udp.parsePacket();
  jetson = udp.remoteIP();
  while(!connected){
    udp.read(udpPacket,packetSize);
    if(serResp == "ready"){
      connected = true;
      udp.beginPacket(jetson,jetsonPort);
      udp.write("ready");
      udp.endPacket();
    }
  }
  delay(10);
}

// serial transmission blueprint:

// [start transmission = -127 or 255] [device id] [overdrive] [left wheels]...
// ...[right wheels] [gimble tilt] [gimble pan] [hash]

// hash = (sum of data bytes--no start or id) / (num of bytes)

// all message bytes will be within the range [-90,90]

// device id #'s:
// drive    0
// arm      1
// science  2

void loop() {
  int packetSize = 0;
  int pos = 0;
  //if(Serial.available()) // trying to read from an empty buffer usually breaks stuff
  if(udp.available()){
    packetSize = udp.parsePacket();
    udp.read(udpPacket,packetSize);
  }
  while(pos < packetSize)
  {
    //incomingByte = Serial.read();
    incomingByte = udpPacket[pos];
    if(incomingByte == -127 && !inTransmission)
    {
      inTransmission = true;
      bytesRead = 0;
    }
    else if(inTransmission)
    {
      switch(bytesRead)
      {
        case 0: // device id
          if(incomingByte != DEVICE_ID)
          {
            // transmission is invalid, clear buffer and then reply with device id
            //Serial.flush();
            //Serial.write(-126);
            //Serial.write(DEVICE_ID);
            char message[2] = {-126,DEVICE_ID};
            pos = packetSize;
            udp.beginPacket(jetson,jetsonPort);
            udp.write(message);
            udp.endPacket();
            
            inTransmission = false;
          }
          break;
        case 1: // overdrive
          overdrive = incomingByte;
          break;
        case 2: // left wheels
          leftWheels = incomingByte;
          break;
        case 3: // right wheels
          rightWheels = incomingByte;
          break;
        case 4: // gimble tilt
          tiltByte = incomingByte;
          tilt += tiltByte;
          if(tilt <40 || tilt >= 127)
              tilt -= tiltByte;
          break;
        case 5: // gimble pan
          pan = incomingByte;
          break;
        case 6: // hash (last byte recieved in transmission)
          serialHash = incomingByte;
          inTransmission = false;
          myHash = (pan + tiltByte + leftWheels + rightWheels + overdrive)/5;
          if(myHash == serialHash)
          {
            //sprintf(serResp, "%d\t%d\t%d\t%d\t%d\t%d", overdrive, leftWheels, rightWheels, tilt, pan, serialHash);
            //Serial.println(serResp);
            updateServos();
          } else {
            //sprintf(serResp, "%d\t%d\t%d\t%d\t%d\t%d!!!!!!!!!!!!!!!!!!!", overdrive, leftWheels, rightWheels, tilt, pan, serialHash);
            //Serial.println(serResp);
            
          }
          break;
      }
      bytesRead++;
    }
    pos++;
  }
  Ethernet.maintain();
}

void updateServos(){
   if((overdrive & 1)){
      disk.write(0);
   }else{
      disk.write(180);
   }
   if(overdrive & 8){
       gimbal_pan.write(90);
       gimbal_tilt.write(90);
   }else{
       gimbal_pan.write(93 + pan);
       gimbal_tilt.write(tilt);
   }
   if((overdrive & 2) && ( overdrive & 4)){
   	wheel[0].write(90 +leftWheels);
	wheel[3].write(90 + rightWheels);
	wheel[2].write(90 + leftWheels);
	wheel[5].write(90 + rightWheels);
   }
   if(overdrive & 2){
	wheel[0].write(90 + leftWheels);
	wheel[3].write(90 + rightWheels);
	return;
   }
   if(overdrive & 4){
       wheel[2].write(90 - leftWheels);
       wheel[5].write(90 - leftWheels);
       return;
   }

 /*  if(overdrive) {
      wheel[0].write(90 + leftWheels);
      wheel[1].write(90 + leftWheels);
      wheel[2].write(90 - leftWheels);
      wheel[3].write(90 + rightWheels);
      wheel[4].write(90 - rightWheels);
      wheel[5].write(90 - rightWheels);
   }*/ else {
      wheel[0].write(90 + (leftWheels));// * 1/2));
      wheel[1].write(90 + (leftWheels));
      wheel[2].write(90 - (leftWheels));
      wheel[3].write(90 + (rightWheels));
      wheel[4].write(90 - (rightWheels));
      wheel[5].write(90 - (rightWheels));  
   }
   
}
