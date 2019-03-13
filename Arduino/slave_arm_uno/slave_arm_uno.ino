#include <Ethernet.h>
#include <EthernetUdp.h>

#define DEVICE_ID 1

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xAB, 0xCD, 0xEF, 0x01, 0x23, 0x45
};
IPAddress ip(192, 168, 1, 140);

unsigned int localPort = 2040;      // local port to listen on

// packet related variables
char packetBuffer[30];  // buffer to hold incoming packet
EthernetUDP Udp;

// message data to store
int potYaw;
int potShoulder;
int potElbow;
int potWristPitch;
int potWristRoll;
bool buttonClawOpen;
bool buttonClawClose;

void setup() {
  Ethernet.init(10);  // Most Arduino shields
  Ethernet.begin(mac, ip);

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
  Udp.begin(localPort);
}

void loop() {
  if (read_data())
  {
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
    // TODO: make the values move stuff
  }
  delay(10);
}

bool read_data()
{
  // the message format (14 bytes total):
  // (-127) | Device ID (1) | yaw (high) | yaw (low) | shoulder (high) ...
  // ... | shoulder (low) | elbow (high) | elbow (low) | wrist pitch (high) ...
  // ... | wrist pitch (low) | wrist roll (high) | wrist roll (low) | buttons | hash
  
  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if(packetSize == 0)
  {
    return false;
  }
  
  // read the packet into packetBufffer
  Udp.read(packetBuffer, packetSize);
      
  // serial debug
  Serial.println("Raw data");
  for(int i = 0; i < packetSize; i++)
  {
    Serial.print((int)(packetBuffer[i]));
    Serial.print(", ");
  }
  Serial.print("\n");
  
  if(packetSize != 14)
  {
    Serial.println("Message is wrong length!");
    return false;
  }

  if(packetBuffer[0] != -127)
  {
    Serial.println("Message does not start with -127!");
    return false;
  }

  if(packetBuffer[1] != 1)
  {
    Serial.println("Wrong device id!");
    return false;
  }
  
  char hash = (packetBuffer[2] + packetBuffer[3] + packetBuffer[4] + packetBuffer[5] + packetBuffer[6] + packetBuffer[7] + packetBuffer[8] + packetBuffer[9] + packetBuffer[10] + packetBuffer[11] + packetBuffer[12]) / 11;
  if(hash != packetBuffer[13])
  {
    Serial.println("Wrong hash!");
    return false;
  }

  potYaw = (int)(packetBuffer[2] << 8) + (byte)packetBuffer[3];
  potShoulder = (int)(packetBuffer[4] << 8) + (byte)packetBuffer[5];
  potElbow = (int)(packetBuffer[6] << 8) + (byte)packetBuffer[7];
  potWristPitch = (int)(packetBuffer[8] << 8) + (byte)packetBuffer[9];
  potWristRoll = (int)(packetBuffer[10] << 8) + (byte)packetBuffer[11];

  buttonClawOpen = ((packetBuffer[12] & 0x01) > 0);  // 2^0 bit
  buttonClawClose = ((packetBuffer[12] & 0x02) > 0); // 2^1 bit

  return true;
}
