/***
* Code for the CO2 Sensor from: https://www.dfrobot.com/wiki/index.php/Gravity:_Analog_Infrared_CO2_Sensor_For_Arduino_SKU:_SEN0219#Tutorial
*/

#include <Wire.h>

#include <Servo.h>
#include <SPI.h>
#include <EtherCard.h>
#include <IPAddress.h>
// Read Data from Grove - Multichannel Gas Sensor
#include <Wire.h>
#include "MutichannelGasSensor.h"
//Temperature and Moisture Sensor libs
#include "dht11.h"

//#include <Adafruit_MotorShield.h>

#define FAN_PIN 10
#define ACTUATOR_PIN_A 3
#define ACTUATOR_PIN_B 5
#define DRILL_PIN 12
dht11 DHT11;
#define DHT11PIN 2
#define CO2_PIN 4

//float gasData[8];
//float TandMData[7];
int flag = -127;

int drillStuff[] = { 2, 0, 0 };
int fanSpeed = 0;
int clientHash = 0;
int myHash = 0;
int hash = 0;
// Need MAC and IP address of the Arduino
static byte ip[] = {10,0,0,105};
static byte gw[] = {10,0,0,1};
static byte mac[] = {0x69,0xFF,0x3D,0x9A,0x88,0x12};
static byte mcIP[] = {10,0,0,103};
byte Ethernet::buffer[500];
uint8_t port = 21;
int localPort = 4400;

//sending data positions
#define GAS_DATA_POS 0
#define T_AND_M_DATA_POS (8 * 4)
#define CONCENTRATION_POS (T_AND_M_DATA_POS + (7 * 4))
#define HASH_POS (CONCENTRATION_POS + 4)

//packet size is the gasData + TandMData + CO2 concentration + commas separating all data
//gasData and TandMData and concentration are all floats or 4 bytes
//the packet size should be the position of the last byte of data + 1
#define packetSize HASH_POS + 1//((8 + 7 + 1) * 4)
char message[packetSize];
char nack = "nack";
int header = -127;

//recieving data positions
#define HEADER 0
#define BASE_POS 1
#define SHOULDER_POS 2
#define ELBOW_POS 3
#define OVERDRIVE_POS 4
#define FAN_SPEED_POS 5
#define HASH_POS 6

//union struct to help with copying floats into message
union floatStruct{
  float f;
  uint8_t data[4];
};

Servo myservo;
Servo drill;
int drillSpeed = 0;

void handleMessage(uint16_t dest_port, uint8_t src_ip[IP_LEN], uint16_t src_port, const char *data, uint16_t len){
  if(data[HEADER] != header){
    ether.sendUdp(nack,sizeof(nack), localPort, mcIP, port);
  }else{
    drillStuff[0] = data[BASE_POS];
    drillStuff[1] = data[SHOULDER_POS];
    drillStuff[2] = data[ELBOW_POS];
    drillStuff[3] = data[OVERDRIVE_POS];
    drillStuff[4] = data[FAN_SPEED_POS];
    hash = data[HASH_POS];
    myHash = (drillStuff[0] + drillStuff[1] + drillStuff[2] + drillStuff[3] + drillStuff[4]) / 5;

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
      Serial.println(concentration);
      message[CONCENTRATION_POS] = ((uint32_t)concentration);
      message[CONCENTRATION_POS + 1] = ((uint32_t)concentration) << 8;
      message[CONCENTRATION_POS + 2] = ((uint32_t)concentration) << 16;
      message[CONCENTRATION_POS + 3] = ((uint32_t)concentration) << 24;
    }

    // check hash and actually move things
    if (data[HASH_POS] == myHash ) {
      moveDrill();
      spinOfDeath();
      spinFan();
    } else {
      Serial.print("Data Corrupted");
      ether.sendUdp(nack,sizeof(nack), localPort, mcIP, port);
    }

    ether.sendUdp(message,sizeof(message), localPort, mcIP, port);
  }
}

void setup() {
  // Start gas sensor
  gas.begin(0x04);//the default I2C address of the slave is 0x04
  gas.powerOn();
  delay(1000);
  myservo.attach(FAN_PIN);
  Serial.begin(9600);
  Serial.setTimeout(1000);
  Serial.println("NH3,CO,NO2,C3H8,C4H10,CH4,H2,C2H5OH,Flag,Humidity,Tempature( C ),Fahrenheit,Kelvin,Dew Point,Dew Point,Hash");
  drill.attach(DRILL_PIN);

  //analogReference(DEFAULT);

  //delay while the CO2 sensor warms up
  int voltage = analogRead(CO2_PIN) * (5000/1024.0);
  char *reheatingMessage = "CO2 Preheating";
  while (voltage < 400) {
    //Serial.println("CO2 Preheating");
    ether.sendUdp(reheatingMessage, sizeof(reheatingMessage), localPort, mcIP, port);
    voltage = analogRead(CO2_PIN) * (5000/1024.0);
  }
}

void loop() {
  ether.packetLoop(ether.packetReceive());
}


void stow()
{
  // Move all servos to stowed positions
}

void spinOfDeath() {
  // Spin the drill
  int val = drillStuff[2];
  if (drillStuff[2] == 0 ) {
    drill.write(92);
    return;
  }
  if (drillStuff[3]) {//overDrive is off (remember c++ is semi boolean) this is to protect motors
    drill.write(90 + val);
    //afms.setPWM(1, (val * 2048/255) + 2045 );
  }
  else {
    if (drillStuff[2] < 0) {
      drill.write(84 + (val * 18 / 24));
      //afms.setPWM(1, (2045 - (val * 2048/255)));
    } else {
      drill.write(90 + (val * 18 / 24));
    }
  }
}

void readTandMSensor()
{
  // Flag,Humidity,Tempature( C ),Fahrenheit,Kelvin,Dew Point,Dew Point  ),
  int chk = DHT11.read(DHT11PIN);
  switch (chk)
  {
    case DHTLIB_OK:
      flag = -127;
      break;
    case DHTLIB_ERROR_CHECKSUM:
      flag = -120;//"Checksum error");
      break;
    case DHTLIB_ERROR_TIMEOUT:
      flag = -119;// "Time out error");
      break;
    default:
      flag = -118;//"Unknown error");
      break;
  }
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
  messagePointer[0] = flag;
  messagePointer++;
  nextReading.f = (float)DHT11.humidity;
  memcpy(messagePointer, nextReading.data, sizeof(nextReading));
  messagePointer += 4;
  nextReading.f = (float)DHT11.temperature;
  memcpy(messagePointer, nextReading.data, sizeof(nextReading));
  messagePointer += 4;
  nextReading.f = Fahrenheit(DHT11.temperature);
  memcpy(messagePointer, nextReading.data, sizeof(nextReading));
  messagePointer += 4;
  nextReading.f = Kelvin(DHT11.temperature);
  memcpy(messagePointer, nextReading.data, sizeof(nextReading));
  messagePointer += 4;
  nextReading.f = dewPoint(DHT11.temperature, DHT11.humidity);
  memcpy(messagePointer, nextReading.data, sizeof(nextReading));
  nextReading.f = dewPointFast(DHT11.temperature, DHT11.humidity);
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
  nextReading.f = gas.measure_NH3();
  memcpy(messagePointer, nextReading.data, sizeof(nextReading));
  messagePointer += 4;
  nextReading.f = gas.measure_CO();
  memcpy(messagePointer, nextReading.data, sizeof(nextReading));
  messagePointer += 4;
  nextReading.f = gas.measure_NO2();
  memcpy(messagePointer, nextReading.data, sizeof(nextReading));
  messagePointer += 4;
  nextReading.f = gas.measure_C3H8();
  memcpy(messagePointer, nextReading.data, sizeof(nextReading));
  messagePointer += 4;
  nextReading.f = gas.measure_C4H10();
  memcpy(messagePointer, nextReading.data, sizeof(nextReading));
  messagePointer += 4;
  nextReading.f = gas.measure_CH4();
  memcpy(messagePointer, nextReading.data, sizeof(nextReading));
  messagePointer += 4;
  nextReading.f = gas.measure_H2();
  memcpy(messagePointer, nextReading.data, sizeof(nextReading));
  messagePointer += 4;
  nextReading.f = gas.measure_C2H5OH();
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
    myservo.write(90 + fanSpeed);
}

void moveDrill() {

  switch (drillStuff[0]) {
    case 0:
      analogWrite(ACTUATOR_PIN_A, drillStuff[1]);
      analogWrite(ACTUATOR_PIN_B, 0);
      break;
    case 1:
      analogWrite(ACTUATOR_PIN_A, 0);
      analogWrite(ACTUATOR_PIN_B, drillStuff[1]);
      break;
    case 2:
      analogWrite(ACTUATOR_PIN_A, 0);
      analogWrite(ACTUATOR_PIN_B, 0);
      break;
  }
}
