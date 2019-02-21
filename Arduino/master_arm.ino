// Eric Rackelin
// 2/20/19

#include <EtherCard.h>
#include <IPAddress.h>
#include <Servo.h>

#define BUTTON_CLAW_OPEN 0
#define BUTTON_CLAW_CLOSE 1
#define BUTTON_ARM_DISABLE 2
#define BUTTON_ARM_STOW 3

#define POT_YAW A0
#define POT_SHOULDER A1
#define POT_ELBOW A2
#define POT_WRIST_PITCH A3 // joystick vertical axis
#define POT_WRIST_ROLL A4 // joystick horizontal axis

#define DEVICE_ID 1

byte message[14]; // the bytes we send out over UDP


// master address (this)
static byte myip[] = { 192, 168, 0, 69 };
const int srcPort PROGMEM = 4321;

// slave address
static byte dstIp[] = { 10, 0, 0, 102 };
const int dstPort PROGMEM = 1002;

// gateway ip address
static byte gwip[] = { 192, 168, 0, 1 };

// ethernet mac address - must be unique on your network
static byte mymac[] = { 0x69, 0x69, 0x69, 0x69, 0x69, 0x69 }; // nice

// tcp/ip send and receive buffer
byte Ethernet::buffer[500];

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
  Serial.begin(9600);
  if (ether.begin(sizeof Ethernet::buffer, mymac, 10) == 0)
    Serial.println(F("Failed to access Ethernet controller"));
  
  ether.staticSetup(myip, gwip);

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);
  ether.printIp("DNS: ", ether.dnsip);
}

void loop()
{  
  readData();

  // the message format (14 bytes total):
  // (-127) | Device ID (1) | yaw (high) | yaw (low) | shoulder (high) ...
  // ... | shoulder (low) | elbow (high) | elbow (low) | wrist pitch (high) ...
  // ... | wrist pitch (low) | wrist roll (high) | wrist roll (low) | hash
  
  //static void sendUdp (char *data,uint8_t len,uint16_t sport, uint8_t *dip, uint16_t dport);
  ether.sendUdp(message, sizeof(message), srcPort, dstIp, dstPort );
}

void readData()
{  
  message[0] = -127; // Start message byte
  message[1] = DEVICE_ID;

  // pot values are 2 bytes (most significant bits sent first)
  message[2] = (byte)(analogRead(POT_YAW) >> 8);
  message[3] = (byte)(analogRead(POT_YAW));

  message[4] = (byte)(analogRead(POT_SHOULDER) >> 8);
  message[5] = (byte)(analogRead(POT_SHOULDER));

  message[6] = (byte)(analogRead(POT_ELBOW) >> 8);
  message[7] = (byte)(analogRead(POT_ELBOW));

  message[8] = (byte)(analogRead(POT_WRIST_PITCH) >> 8);
  message[9] = (byte)(analogRead(POT_WRIST_PITCH));

  message[10] = (byte)(analogRead(POT_WRIST_ROLL) >> 8);
  message[11] = (byte)(analogRead(POT_WRIST_ROLL));
  
  // buttons - each bit is a different button
  message[12] = 0;
  message[12] = message[12] & digitalRead(BUTTON_CLAW_OPEN);  // 2^0 - open claw
  message[12] = message[12] & digitalRead(BUTTON_CLAW_CLOSE) << 1; // 2^1 - close claw

  // hash
  message[13] = (message[2] + message[3] + message[4] + message[5] + message[6] + message[7] + message[8] + message[9] + message[10] + message[11] + message[12]) / 11;
}
