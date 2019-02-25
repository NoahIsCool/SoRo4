// Eric Rackelin
// 2/21/19
// arduino uno shield version

#include <Ethernet.h>
#include <EthernetUdp.h>
#include <Servo.h>

#define BUTTON_CLAW_OPEN 2
#define BUTTON_CLAW_CLOSE 3
#define BUTTON_ARM_DISABLE 4
#define BUTTON_ARM_STOW 5

#define POT_YAW A0
#define POT_SHOULDER A1
#define POT_ELBOW A2
#define POT_WRIST_PITCH A3 // joystick vertical axis
#define POT_WRIST_ROLL A4 // joystick horizontal axis

#define DEVICE_ID 1

char message[14]; // the bytes we send out over UDP

// master address (this)
IPAddress myip(192, 168, 1, 69);
const int srcPort PROGMEM = 4321;

// ethernet mac address - must be unique on your network
static byte mymac[] = { 0x69, 0x69, 0x69, 0x69, 0x69, 0x69 };

// slave address
IPAddress dstip(192, 168, 1, 140);
const int dstPort PROGMEM = 2040;

EthernetUDP Udp;

void setup()
{
  // buttons
  pinMode(BUTTON_CLAW_OPEN, INPUT);
  pinMode(BUTTON_CLAW_CLOSE, INPUT);
  pinMode(BUTTON_ARM_DISABLE, INPUT);
  pinMode(BUTTON_ARM_STOW, INPUT);
  
  // potentiometers
  pinMode(POT_YAW, INPUT);
  pinMode(POT_SHOULDER, INPUT);
  pinMode(POT_ELBOW, INPUT);
  pinMode(POT_WRIST_PITCH, INPUT);
  pinMode(POT_WRIST_ROLL, INPUT);

  // ethernet
  Ethernet.init(10);
  
  Ethernet.begin(mymac, myip);

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  // start UDP
  Udp.begin(srcPort);
}

void loop()
{  
  delay(1000);
  readData();

  // the message format (14 bytes total):
  // (-127) | Device ID (1) | yaw (high) | yaw (low) | shoulder (high) ...
  // ... | shoulder (low) | elbow (high) | elbow (low) | wrist pitch (high) ...
  // ... | wrist pitch (low) | wrist roll (high) | wrist roll (low) | buttons | hash
  
  Udp.beginPacket(dstip, dstPort);
  for(int i = 0; i < 14; i++)
  {
    Udp.write(message[i]);
  }
  Udp.endPacket();
}

void readData()
{  
  message[0] = -127; // Start message byte
  message[1] = DEVICE_ID;

  // pot values are 2 bytes (most significant bits sent first)
  message[2] = (char)(analogRead(POT_YAW) >> 8);
  message[3] = (char)(analogRead(POT_YAW));

  message[4] = (char)(analogRead(POT_SHOULDER) >> 8);
  message[5] = (char)(analogRead(POT_SHOULDER));

  message[6] = (char)(analogRead(POT_ELBOW) >> 8);
  message[7] = (char)(analogRead(POT_ELBOW));

  message[8] = (char)(analogRead(POT_WRIST_PITCH) >> 8);
  message[9] = (char)(analogRead(POT_WRIST_PITCH));

  message[10] = (char)(analogRead(POT_WRIST_ROLL) >> 8);
  message[11] = (char)(analogRead(POT_WRIST_ROLL));
  
  // buttons - each bit is a different button
  message[12] = 0;
  if(digitalRead(BUTTON_CLAW_OPEN)) // 2^0 - open claw
  {
    message[12] = message[12] | 0x01;
  }
  if(digitalRead(BUTTON_CLAW_CLOSE)) // 2^1 - close claw
  {
    message[12] = message[12] | 0x02;
  }

  // hash
  message[13] = (message[2] + message[3] + message[4] + message[5] + message[6] + message[7] + message[8] + message[9] + message[10] + message[11] + message[12]) / 11;
}
