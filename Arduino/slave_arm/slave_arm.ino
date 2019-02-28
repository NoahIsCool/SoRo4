#include <EtherCard.h>
#include <IPAddress.h>

#define DEVICE_ID 1

// ethernet interface ip address
static byte myip[] = { 192,168,0,200 };
// gateway ip address
static byte gwip[] = { 192,168,0,1 };
// ethernet mac address - must be unique on your network
static byte mymac[] = { 0xAB, 0xCD, 0xEF, 0x01, 0x23, 0x45 };

unsigned int localPort = 2040;      // local port to listen on

byte Ethernet::buffer[500]; // tcp/ip send and receive buffer

// message data to store
int potYaw;
int potShoulder;
int potElbow;
int potWristPitch;
int potWristRoll;
bool buttonClawOpen;
bool buttonClawClose;

void setup() {
  Serial.begin(9600);
  
  ether.begin(sizeof Ethernet::buffer, mymac, SS);
  ether.staticSetup(myip, gwip);
  ether.udpServerListenOnPort(&read_data, localPort);
}

void loop() {
  // this must be called for ethercard functions to work.
  ether.packetLoop(ether.packetReceive());
  
  // TODO: make the values move stuff
}

void read_data(uint16_t dest_port, uint8_t src_ip[IP_LEN], uint16_t src_port, const char *data, uint16_t len){
  IPAddress src(src_ip[0],src_ip[1],src_ip[2],src_ip[3]);
  
  // the message format (14 bytes total):
  // (-127) | Device ID (1) | yaw (high) | yaw (low) | shoulder (high) ...
  // ... | shoulder (low) | elbow (high) | elbow (low) | wrist pitch (high) ...
  // ... | wrist pitch (low) | wrist roll (high) | wrist roll (low) | buttons | hash
  
  // serial debug
  Serial.println("Raw data");
  for(int i = 0; i < len; i++)
  {
    Serial.print((int)(data[i]));
    Serial.print(", ");
  }
  Serial.print("\n");
  
  if(len != 14)
  {
    Serial.println("Message is wrong length!");
    return;
  }

  if(data[0] != -127)
  {
    Serial.println("Message does not start with -127!");
    return;
  }

  if(data[1] != 1)
  {
    Serial.println("Wrong device id!");
    return;
  }
  
  char hash = (data[2] + data[3] + data[4] + data[5] + data[6] + data[7] + data[8] + data[9] + data[10] + data[11] + data[12]) / 11;
  if(hash != data[13])
  {
    Serial.println("Wrong hash!");
    return;
  }

  // at this point the recieved message is good, so store the values
  potYaw = (int)(data[2] << 8) + (byte)data[3];
  potShoulder = (int)(data[4] << 8) + (byte)data[5];
  potElbow = (int)(data[6] << 8) + (byte)data[7];
  potWristPitch = (int)(data[8] << 8) + (byte)data[9];
  potWristRoll = (int)(data[10] << 8) + (byte)data[11];

  buttonClawOpen = ((data[12] & 0x01) > 0);  // 2^0 bit
  buttonClawClose = ((data[12] & 0x02) > 0); // 2^1 bit

  // debug - print out the values
  Serial.println("Interpreted message data");
  Serial.print(potYaw);
  Serial.print(", ");
  Serial.print(potShoulder);
  Serial.print(", ");
  Serial.print(potElbow);
  Serial.print(", ");
  Serial.print(potWristPitch);
  Serial.print(", ");
  Serial.print(potWristRoll);
  Serial.print(", ");
  Serial.print(buttonClawOpen);
  Serial.print(", ");
  Serial.println(buttonClawClose);
}
