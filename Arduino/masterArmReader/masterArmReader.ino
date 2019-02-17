/* Uses potentiometers and buttons on a master arm to control
 *  the slave arm on the rover.
 * This program is just for collecting the position from the master
 * The data is sent over USB serial to a computer (probably an UPboard)
 * 
 * ORDERING:
 * START | ID | shoulderPosition | elbowPosition | pitchDown | pitchUP | rollLeft/CCW | rollRight/CW | closeHand | openHand | average of all values
 * 
 * Notes:
 * -Buttons just report if the button is pressed or not, movement must be controlled at the master
 * -ID must be known by the serial master (computer)
 * -Uses 9600 baud
 * -If the average is disagreed by serial master and slave something went wrong
 * 
 * Protocol Used:
 *
 *  (-127) | USERID | DATA | DATA | ... | Average of all sent values |
 *  -127 is the start message 
 *  USERID is an int used to identify which serial device is running the program
 *  Average of all sent values is to ensure no data loss and for debugging
 *  
 *  Potentiometer Values:
 *  The output to serial is in units of degrees and is sent as a 16 bit integer (your programming language should handle the bits you just need to know it's an integer)
 *  
 *  Buttons:
 *  The buttons are stored in the 7th byte of the message. (data[6])
 *  The byte has this structure (button 3)(button 2)(Button 1)(Button 0)(Button 3)(Button 2)(Button 1)(Button 0) 
 *  where 0 is unpressed and 1 is pressed. 
 *  
 *  To parse this byte I'd recomend using this method:
 *  To test if a specific bit is 0 or 1 the formula (byte) XOR (2**n) where n = 0 is the least significant bit
 *  will return an integer 2**n smaller than the original integer if the bit is 0. If the output of this XOR is the same number then that bit is not a 1.
 */

#include <EtherCard.h>
#include <IPAddress.h>

//Serial Communication ID so the computer knows what device this is.
int ID = 1; 

//lists the data positions for the udp message
enum DATA_POS : int{
  START = 0,
  ID_POS,
  STOW,
  DISABLE,
  BASE,
  SHOULDER,
  ELBOW,
  WRIST,
  WRIST_ROTATE_SPEED,
  CLAW,
  HASH
};
//packet size: HASH+1 because hash is the last in the message
int packetSize = HASH+1;

//Port Numbers 
int buttons[4] = {0,1,2,3}; //rollLeft (CCW from the rover's perspective), rollRight (CW from the rover's perspective), let go, grab (digital ports connected to buttons)
#define BASE_POT 0
#define SHOULDER_POT 1
#define ELBOW_POT 2
#define WRIST_POT 3
#define JOYSTICK 4
int joints[5] = {0,1,2,3,4}; //base, shoulder, elbow, wrist pitch (the analog ports the potentiometers are connected to)

//Ethernet stuff
static byte ip[] = {10,0,0,105};
static byte gw[] = {10,0,0,1};
static byte mac[] = {0x69,0xFF,0x3D,0x9A,0x88,0x12};
static byte roverIP[] = {10,0,0,103};
byte Ethernet::buffer[500];
uint8_t port = 5505;
uint8_t slavePort= 1001;

//cannot use pins 13 - 11 and 8
//for digital control pins
#define CLAW_CLOSE_PIN 2
#define CLAW_OPEN_PIN 3
#define STOW_ARM 4
#define DISABLE_ARM 5

//for analog reading pins
#define BASE_ANALOG A0
#define SHOULDER_ANALOG A1
#define ELBOW_ANALOG A2
#define WRIST_ANALOG A3
#define JOYSTICK A4

//speeds for controlling the claw
#define HOLD 90
#define OPEN_CLAW 120
#define CLOSE_CLAW 60

void handleMessage(uint16_t dest_port, uint8_t src_ip[IP_LEN], uint16_t src_port, const char *data, uint16_t len){

}

void setup() {
  //Start talking to the computer
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
}

void loop() {
  ether.packetLoop(ether.packetReceive());
  
  //this doesn't have to be in a seperate function I did this in case we want to add stuff not taking data later.
  sendData(); 
}

void sendData(){
  /*
  data is used as a "packet" for when we send it over serial (don't worry about the floating point math the int typecast automatically converts it after but it will throw a warning oh well)
  0.263671875 is the value 270°/1024 to convert the potentiometer output to degrees (5v/1024 represents 270°)
  NOTE: The converstion to a int restricts our accuracy to a minimum of ~4° of accuracy and to type on a keyboard we need 1.2°)
  */
  char data[packetSize];
  data[START] = -127; //Start command to tell the computer we are sending data
  data[ID_POS] = ID; //ID number to identify the arduino this is coming from.
  data[BASE] = (char)analogRead(joints[BASE_POT]) / 1024.0;//*0.263671875; //Get the data from the Yaw Pot. (rotate arm horizontally)
  data[SHOULDER] = (char)analogRead(joints[SHOULDER_POT]) / 1024.0;//*0.263671875; //Get the data from the Shoulder Pot. (the bottom joint)
  data[ELBOW] = (char)analogRead(joints[ELBOW_POT]) / 1024.0;//*0.263671875; //Get the data from the Elbow Pot. (the second from the bottom joint)
  data[WRIST] = (char)analogRead(joints[WRIST_POT]) / 1024.0;//*0.263671875; //Get the data from the Wrist Pitch Pot. (rotate the finger/hand pitch)
  data[WRIST_ROTATE_SPEED] = (char)analogRead(joints[JOYSTICK]) / 1024;
  if(digitalRead(CLAW_OPEN_PIN) == HIGH){
    data[CLAW] = OPEN_CLAW;
  }else if(digitalRead(CLAW_CLOSE_PIN) == HIGH){
    data[CLAW] = CLOSE_CLAW;
  }else{
    data[CLAW] = HOLD;
  }
  //data[6] = 0; //Set all the button data to default to 0 (low)
  if(digitalRead(STOW_ARM) == HIGH){
    data[STOW] = 1;
  }
  if(digitalRead(DISABLE_ARM) == HIGH){
    data[DISABLE] = 1;
  }
  
  //Set this variable to 0 so we can average it at the end
  int sum = 0;
  
  //Cycle throught all elements of the message and send
  for(int z = 0; z<packetSize; z++){
    
    //Send the data array element by element (will be seen by reader as a series of integers)
    Serial.print(data[z]);
    Serial.print(" ");

    
    //Increase the sum by the data value so we can average it out at the end
    sum = sum + data[z];
  }
  Serial.println(sum/10);
  data[HASH] = sum / 10;
  ether.sendUdp(data, sizeof(data), port, roverIP, slavePort);

}
