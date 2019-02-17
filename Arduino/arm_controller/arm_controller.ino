#include <Servo.h>
#include <EtherCard.h>
#include <IPAddress.h>

enum DATA_POS : int{
  START = 0,
  ID,
  DISABLE,
  BASE,
  SHOULDER,
  ELBOW,
  WRIST,
  WRIST_ROTATE_SPEED,
  CLAW,
  HASH
};

const char DEVICE_ID = 1;

// servos, pins and positions;
Servo base;
Servo shoulder;
Servo elbow;
Servo wristPitch;
Servo wristRotate;
Servo claw;

//cannot use pins 13 - 11 and 8
#define BASE_PIN 2
#define SHOULDER_PIN 3
#define ELBOW_PIN 4
#define WRIST_PIN 5
#define WRIST_ROTATE_PIN 6
#define CLAW_PIN 7
#define CLAW_STOPPER_PIN 18

//for analog reading pins
#define BASE_ANALOG A0
#define SHOULDER_ANALOG A1
#define ELBOW_ANALOG A2
#define WRIST_ANALOG A3

int base_pos = 0;
int shoulder_pos = 135;
int elbow_pos = 135;
int wrist_pos = 0;
//FIXME: eventually will get input from a joystick
int wristRotateSpeed = 0;
#define HOLD 90
#define CLOCKWISE 120
#define CCLOCKWISE 60
//claw positions
//also uses hold
#define OPEN_CLAW 120
#define CLOSE_CLAW 60
int clawDir = HOLD;

//Ethernet stuff
static byte ip[] = {10,0,0,103};
static byte gw[] = {10,0,0,1};
static byte mac[] = {0x69,0xFF,0x3D,0x9A,0x88,0x12};
byte Ethernet::buffer[500];
int port = 1001;
//messages
char ack[] = "ack";
char nack[] = "nack";
char myHash = 90;

//setup wheels
void setup() {
  base.attach(BASE_PIN);
  shoulder.attach(SHOULDER_PIN);
  elbow.attach(ELBOW_PIN);
  wristPitch.attach(WRIST_PIN);
  wristRotate.attach(WRIST_ROTATE_PIN);

  //set to default positions
  shoulder.write(shoulder_pos);
  elbow.write(elbow_pos);
  wristPitch.write(wrist_pos);
  wristRotate.write(wristRotateSpeed);
  claw.write(clawDir);

  Serial.begin(9600);

  //setup ethernet
  // Change 'SS' to your Slave Select pin, if you aren't using the default pin
  // Our ethernet card uses pin 10. SS is 8 by default. --Eric
  if(ether.begin(sizeof Ethernet::buffer, mac, 10) == 0)
    Serial.println("failed to access Ethernet controller");
  ether.staticSetup(ip,gw);

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);
  ether.printIp("DNS: ", ether.dnsip);

  ether.udpServerListenOnPort(&handleMessage,port);
  
  delay(10);
}

void handleMessage(uint16_t dest_port, uint8_t src_ip[IP_LEN], uint16_t src_port, const char *data, uint16_t len){
    if(data[START] == -127){
      base_pos = data[BASE];
      shoulder_pos = data[SHOULDER];
      elbow_pos = data[ELBOW];
      wrist_pos = data[WRIST];
      wristRotateSpeed = data[WRIST_ROTATE_SPEED];
      clawDir = data[CLAW];
      char hash = data[HASH];
      myHash = (base_pos + shoulder_pos + elbow_pos/* + wrist_pos + claw_dir*/) / 3;//5;
      if(hash == myHash){
        ether.sendUdp(ack, sizeof(ack), src_port, src_ip, dest_port );
        updateServos();
      }else{
        ether.sendUdp(nack, sizeof(nack), src_port, src_ip, dest_port );
      }
    }
}

void loop() {
  ether.packetLoop(ether.packetReceive());
}

void updateServos() {

  base.write(base_pos);
  shoulder.write(shoulder_pos);
  elbow.write(elbow_pos);
  wristPitch.write(wrist_pos);
  wristRotate.write(wristRotateSpeed);
  claw.write(clawDir);
}

void calibrate() {

}

