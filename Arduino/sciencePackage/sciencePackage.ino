/***
* Code for the CO2 Sensor from: https://www.dfrobot.com/wiki/index.php/Gravity:_Analog_Infrared_CO2_Sensor_For_Arduino_SKU:_SEN0219#Tutorial
* 
* fan and drill are using talon
* 
* linear actuator is using motor controller
*/

#include <Wire.h>

#include <Servo.h>
#include <SPI.h>
#include <EtherCard.h>
#include <IPAddress.h>
// Read Data from Grove - Multichannel Gas Sensor
#include "MutichannelGasSensor.h"
//Temperature and Moisture Sensor libs
#include "dht11.h"

dht11 DHT11;
#define DHT11PIN 2
#define CO2_PIN 4

// Need MAC and IP address of the Arduino
static byte ip[] = {192,168,1,105};//{10,0,0,105};
static byte gw[] = {192,168,1,1};//{10,0,0,1};
static byte dnsip[] = { 8,8,8,8 };
static byte netmask[] = { 255,255,255,0 };
static byte mac[] = {0x69,0xFF,0x3D,0x9A,0x88,0x12};
static byte mcIP[] = {192,168,1,102};//{10,0,0,103};
byte Ethernet::buffer[500];
int mcPort = 4444;
int localPort = 4400;

//data positions for the gas data to be sent back
//all of these are floats
#define GAS_DATA_POS 0
#define T_AND_M_DATA_POS (8 * 4)
#define CONCENTRATION_POS (T_AND_M_DATA_POS + (7 * 4))
#define HASH_POS (CONCENTRATION_POS + 4)

//packet size is the gasData + TandMData + CO2 concentration + commas separating all data
//gasData and TandMData and concentration are all floats or 4 bytes
//the packet size should be the position of the last byte of data + 1
#define packetSize HASH_POS + 1
unsigned char message[packetSize];
char nack = "nack";
int header = -127;
char hash = 0;

//recieving data positions
#define HEADER_POS 0
#define ACTUATOR_DIRECTION_POS 1
#define DRILL_SPEED_POS 2
#define FAN_SPEED_POS 3
#define HASH_POS 4

//union struct to help with copying floats into message
union floatStruct{
  float f;
  uint8_t data[4];
};

//used for both actuator and drill
//extend is to move the drill clockwise or to move the drill down
//retract is to move the drill counterclockwise or to move the drill up
#define EXTEND 1
#define NOTHING 0
#define RETRACT -1

//actuator is using a motor controller or an hbridge
#define actuatorSideA 5
#define actuatorSideB 3
int actuatorDirection = NOTHING;
Servo fan;
#define fanSpeed 175
int runFan = 0;
#define fanPin 7
Servo drill;
#define drillPin 6
int drillDirection = NOTHING;
int drillSpeed = 70;

//the fan will be controlled using a burst mode
//time is in milliseconds
#define burstDiration 5000
long burstStartTime = 0;
bool runningBurst = false;

void handleMessage(uint16_t dest_port, uint8_t src_ip[IP_LEN], uint16_t src_port, const char *data, uint16_t len){
  if(data[HEADER_POS] != header){
    Serial.print("bad message: ");
    Serial.println((int)data[HEADER_POS]);
    ether.sendUdp(nack,sizeof(nack), localPort, mcIP, mcPort);
  }else{
    //Serial.println(data);
    actuatorDirection = data[ACTUATOR_DIRECTION_POS];
    //no longer used
    //drillStuff[DRILL_ACTUATOR_SPEED] = data[ACTUATOR_SPEED_POS];
    drillDirection = data[DRILL_SPEED_POS];
    //maybe wont need this anymore
    //drillStuff[DRILL_OVERDRIVE] = data[OVERDRIVE_POS];
    runFan = data[FAN_SPEED_POS];
    hash = (data[ACTUATOR_DIRECTION_POS] + data[DRILL_SPEED_POS] + data[FAN_SPEED_POS]) / 3;

    //if burst enabled start the time
    if(runFan == 1 && !runningBurst){
      runningBurst = true;
      burstStartTime = millis();
      Serial.println("starting burst");
    }

    readGasSensor();
    readTandMSensor();

    //write gas sensor data to mc
    //need to pack it?
    //gasData
    /*int nextPos = GAS_DATA_POS;
    for(int i = 0; i < 8; i++){
      nextPos = GAS_DATA_POS + (i * 4);
      message[nextPos] = ((uint32_t)gasData[i]);
      message[nextPos + 1] = ((uint32_t)gasData[i]) << 8;
      message[nextPos + 2] = ((uint32_t)gasData[i]) << 16;
      message[nextPos + 3] = ((uint32_t)gasData[i]) << 24;
    }
    for(int i = 0; i < 7; i++){
      nextPos = T_AND_M_DATA_POS + (i * 4);
      message[nextPos] = ((uint32_t)TandMData[i]);
      message[nextPos + 1] = ((uint32_t)TandMData[i]) << 8;
      message[nextPos + 2] = ((uint32_t)TandMData[i]) << 16;
      message[nextPos + 3] = ((uint32_t)TandMData[i]) << 24;
    }*/
    
    //now read CO2 sensor data
    //read CO2 sensor
    int co2Pos = 60;
    int voltage = analogRead(CO2_PIN) * (5000/1024.0);
    if (voltage == 0) {
      Serial.println("CO2 Error");
    } else {
      float concentration = (voltage - 400) * 50.0/16.0;
      //Serial.println(concentration);
      message[CONCENTRATION_POS] = ((uint32_t)concentration);
      message[CONCENTRATION_POS + 1] = ((uint32_t)concentration) << 8;
      message[CONCENTRATION_POS + 2] = ((uint32_t)concentration) << 16;
      message[CONCENTRATION_POS + 3] = ((uint32_t)concentration) << 24;
    }

    // check hash and actually move things
    if (data[HASH_POS] == hash ) {
      moveDrill();
      spinDrill();
      spinFan();
    } else {
      Serial.print("Data Corrupted");
      Serial.print((int)data[HASH_POS]);
      Serial.print(" ");
      Serial.println((int)hash);
      ether.sendUdp(nack,sizeof(nack), localPort, mcIP, mcPort);
      return;
    }

    ether.sendUdp(message,sizeof(message), localPort, mcIP, mcPort);
    /*Serial.println("sent message: ");
    for(int i = 0; i < sizeof(message); i++){
      Serial.println((unsigned int)message[i]);
    }*/
  }
}

void setup() {
  // Start gas sensor
  //DONT FORGET ME: for testing leave this commented. For actaully getting data, uncomment
  //gas.begin(0x04);//the default I2C address of the slave is 0x04
  //gas.powerOn();
  delay(1000);

  //setup the hardware
  drill.attach(drillPin);
  //fan uses an hbridge so one side will always be low and the other
  //will control speed
  fan.attach(fanPin);
  //actuator also uses an hbridge
  pinMode(actuatorSideA,OUTPUT);
  pinMode(actuatorSideB,OUTPUT);
  digitalWrite(actuatorSideA,LOW);
  digitalWrite(actuatorSideB,LOW);
  
  Serial.begin(115200);
  //Serial.setTimeout(1000);
  Serial.println("NH3,CO,NO2,C3H8,C4H10,CH4,H2,C2H5OH,Flag,Humidity,Tempature( C ),Fahrenheit,Kelvin,Dew Point,Dew Point,Hash");
  drill.attach(drillPin);

  if (ether.begin(sizeof Ethernet::buffer, mac, 10) == 0)
    Serial.println("Failed to access Ethernet controller");
  ether.staticSetup(ip, gw, dnsip, netmask);

  ether.udpServerListenOnPort(&handleMessage, localPort);

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);
  ether.printIp("DNS: ", ether.dnsip);

  //analogReference(DEFAULT);

  //delay while the CO2 sensor warms up
  /*int voltage = analogRead(CO2_PIN) * (5000/1024.0);
  char *reheatingMessage = "CO2 Preheating";
  while (voltage < 400) {
    //Serial.println("CO2 Preheating");
    ether.sendUdp(reheatingMessage, sizeof(reheatingMessage), localPort, mcIP, port);
    voltage = analogRead(CO2_PIN) * (5000/1024.0);
  }*/
  Serial.println("all setup");
}

void loop() {
  ether.packetLoop(ether.packetReceive());

  //FIXME: enable after inital testing
  //eventually need to check for burst time
  if(runningBurst){
    if(millis() - burstStartTime > burstDiration){
      runFan = 0;
      runningBurst = false;
      spinFan();
      Serial.println("finished burst");
    }
  }
}


void stow()
{
  // Move all servos to stowed positions
}

void spinDrill() {
  switch(drillDirection){
    case EXTEND:
      Serial.println("spinning drill");
      drill.write(90+drillSpeed);
    break;
    case NOTHING:
      drill.write(90);
    break;
    case RETRACT:
      drill.write(90-drillSpeed);
    break;
  };
}

void readTandMSensor()
{
  // Flag,Humidity,Tempature( C ),Fahrenheit,Kelvin,Dew Point,Dew Point  ),
  int chk = DHT11.read(DHT11PIN);
  /*switch (chk)
  {
    case DHTLIB_OK:
      header = -127;
      break;
    case DHTLIB_ERROR_CHECKSUM:
      header = -120;//"Checksum error");
      break;
    case DHTLIB_ERROR_TIMEOUT:
      header = -119;// "Time out error");
      break;
    default:
      header = -118;//"Unknown error");
      break;
  }*/
  /*TandMData[0] = flag;
  TandMData[1] = (float)DHT11.humidity;// Humidity (%)
  TandMData[2] = (float)DHT11.temperature; // Temperature (°C)
  TandMData[3] = Fahrenheit(DHT11.temperature);
  TandMData[4] = Kelvin(DHT11.temperature);
  TandMData[5] = dewPoint(DHT11.temperature, DHT11.humidity);// Dew Point (°C):
  TandMData[6] =  dewPointFast(DHT11.temperature, DHT11.humidity); // Dew PointFast (°C)??? TODO: use one or the other*/

  int nextPos = T_AND_M_DATA_POS;
  floatStruct nextReading;
  char *messagePointer = message;
  messagePointer += nextPos;
  messagePointer[0] = header;
  messagePointer++;
  nextReading.f = 69.69;//(float)DHT11.humidity;
  memcpy(messagePointer, nextReading.data, sizeof(nextReading));
  messagePointer += 4;
  nextReading.f = 69.69;//(float)DHT11.temperature;
  memcpy(messagePointer, nextReading.data, sizeof(nextReading));
  messagePointer += 4;
  nextReading.f = 69.69;//Fahrenheit(DHT11.temperature);
  memcpy(messagePointer, nextReading.data, sizeof(nextReading));
  messagePointer += 4;
  nextReading.f = 69.69;//Kelvin(DHT11.temperature);
  memcpy(messagePointer, nextReading.data, sizeof(nextReading));
  messagePointer += 4;
  nextReading.f = 69.69;//dewPoint(DHT11.temperature, DHT11.humidity);
  memcpy(messagePointer, nextReading.data, sizeof(nextReading));
  nextReading.f = 69.69;//dewPointFast(DHT11.temperature, DHT11.humidity);
  memcpy(messagePointer, nextReading.data, sizeof(nextReading));
}

void readGasSensor()
{
  // NH3,CO,NO2,C3H8,C4H10,CH4,H2,C2H5OH
  /*gasData[0] = gas.measure_NH3();
  gasData[1] = gas.measure_CO();
  gasData[2] = gas.measure_NO2();
  gasData[3] = gas.measure_C3H8();
  gasData[4] = gas.measure_C4H10();
  gasData[5] = gas.measure_CH4();
  gasData[6] = gas.measure_H2();
  gasData[7] = gas.measure_C2H5OH();*/

  //sweet-ass memory trick. Props the only time you would ever use a union. The float and char array share the same memory so you dont have to do bit shifting
  //the message pointer allows me to increment the address to the next address or the next byte in the message
  int nextPos = GAS_DATA_POS;
  floatStruct nextReading;
  char *messagePointer = message;
  messagePointer += nextPos;
  nextReading.f = 69.69;//gas.measure_NH3();
  memcpy(messagePointer, nextReading.data, sizeof(nextReading));
  messagePointer += 4;
  nextReading.f = 69.69;//gas.measure_CO();
  memcpy(messagePointer, nextReading.data, sizeof(nextReading));
  messagePointer += 4;
  nextReading.f = 69.69;//gas.measure_NO2();
  memcpy(messagePointer, nextReading.data, sizeof(nextReading));
  messagePointer += 4;
  nextReading.f = 69.69;//gas.measure_C3H8();
  memcpy(messagePointer, nextReading.data, sizeof(nextReading));
  messagePointer += 4;
  nextReading.f = 69.69;//gas.measure_C4H10();
  memcpy(messagePointer, nextReading.data, sizeof(nextReading));
  messagePointer += 4;
  nextReading.f = 69.69;//gas.measure_CH4();
  memcpy(messagePointer, nextReading.data, sizeof(nextReading));
  messagePointer += 4;
  nextReading.f = 69.69;//gas.measure_H2();
  memcpy(messagePointer, nextReading.data, sizeof(nextReading));
  messagePointer += 4;
  nextReading.f = 69.69;//gas.measure_C2H5OH();
  memcpy(messagePointer, nextReading.data, sizeof(nextReading));
  
  /*message[nextPos] = nextReading;
  message[nextPos + 1] = nextReading << 8;
  message[nextPos + 2] = nextReading << 16;
  message[nextPos + 3] = nextReading << 24;
  nextPos += 4;
  
  nextReading = (uint32_t)gas.measure_CO();
  message[nextPos] = nextReading;
  message[nextPos + 1] = nextReading << 8;
  message[nextPos + 2] = nextReading << 16;
  message[nextPos + 3] = nextReading << 24;
  nextPos += 4;
  
  nextReading = (uint32_t)gas.measure_NO2();
  message[nextPos] = nextReading;
  message[nextPos + 1] = nextReading << 8;
  message[nextPos + 2] = nextReading << 16;
  message[nextPos + 3] = nextReading << 24;
  nextPos += 4;

  nextReading = (uint32_t)gas.measure_C3H8();
  message[nextPos] = nextReading;
  message[nextPos + 1] = nextReading << 8;
  message[nextPos + 2] = nextReading << 16;
  message[nextPos + 3] = nextReading << 24;
  nextPos += 4;

  nextReading = (uint32_t)gas.measure_C4H10();
  message[nextPos] = nextReading;
  message[nextPos + 1] = nextReading << 8;
  message[nextPos + 2] = nextReading << 16;
  message[nextPos + 3] = nextReading << 24;
  nextPos += 4;

  nextReading = (uint32_t)gas.measure_CH4();
  message[nextPos] = nextReading;
  message[nextPos + 1] = nextReading << 8;
  message[nextPos + 2] = nextReading << 16;
  message[nextPos + 3] = nextReading << 24;
  nextPos += 4;

  nextReading = (uint32_t)gas.measure_H2();
  message[nextPos] = nextReading;
  message[nextPos + 1] = nextReading << 8;
  message[nextPos + 2] = nextReading << 16;
  message[nextPos + 3] = nextReading << 24;
  nextPos += 4;

  nextReading = (uint32_t)gas.measure_C2H5OH();
  message[nextPos] = nextReading;
  message[nextPos + 1] = nextReading << 8;
  message[nextPos + 2] = nextReading << 16;
  message[nextPos + 3] = nextReading << 24;*/
}

double Fahrenheit(double celsius)
{
  return 1.8 * celsius + 32;
}

double Kelvin(double celsius)
{
  return celsius + 273.15;
}

// dewPoint function NOAA
// reference (1) : http://wahiduddin.net/calc/density_algorithms.htm
// reference (2) : http://www.colorado.edu/geography/weather_station/Geog_site/about.htm
//
double dewPoint(double celsius, double humidity)
{
  // (1) Saturation Vapor Pressure = ESGG(T)
  double RATIO = 373.15 / (273.15 + celsius);
  double RHS = -7.90298 * (RATIO - 1);
  RHS += 5.02808 * log10(RATIO);
  RHS += -1.3816e-7 * (pow(10, (11.344 * (1 - 1 / RATIO ))) - 1) ;
  RHS += 8.1328e-3 * (pow(10, (-3.49149 * (RATIO - 1))) - 1) ;
  RHS += log10(1013.246);

  // factor -3 is to adjust units - Vapor Pressure SVP * humidity
  double VP = pow(10, RHS - 3) * humidity;

  // (2) DEWPOINT = F(Vapor Pressure)
  double T = log(VP / 0.61078); // temp var
  return (241.88 * T) / (17.558 - T);
}

// delta max = 0.6544 wrt dewPoint()
// 6.9 x faster than dewPoint()
// reference: http://en.wikipedia.org/wiki/Dew_point
double dewPointFast(double celsius, double humidity)
{
  double a = 17.271;
  double b = 237.7;
  double temp = (a * celsius) / (b + celsius) + log(humidity * 0.01);
  double Td = (b * temp) / (a - temp);
  return Td;
}

void spinFan() {
  if(runningBurst){
    Serial.println("spinning fan");
    fan.write(fanSpeed);
  }else{
    fan.write(90);
  }
}

void moveDrill() {
  switch(actuatorDirection){
    case EXTEND:
      Serial.println("extending drill");
      analogWrite(actuatorSideA,200);
      analogWrite(actuatorSideB,0);
    break;
    case NOTHING:
      analogWrite(actuatorSideA,0);
      analogWrite(actuatorSideB,0);
    break;
    case RETRACT:
      Serial.println("retracting drill");
      analogWrite(actuatorSideA,0);
      analogWrite(actuatorSideB,200);
    break;
  };
}
