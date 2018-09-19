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
 */

//Serial Communication ID so the computer knows what device this is.
int ID = 69;



//Port Numbers 
int buttons[6] = {0,1,2,3,4,5}; //pitchUp, pitchDown, rollLeft (CCW from the rover's perspective), rollRight (CW from the rover's perspective), let go, grab (digital ports connected to buttons)
int joints[2] = {0,1}; //shoulder, Elbow (the analog ports the potentiometers are connected to)

void setup() {
  //Start talking to the computer
  Serial.begin(9600);
}

void loop() {
  sendData(); //function is seperate so we can expand this program if needed
}

void sendData(){
  //data is used as a "packet" for when we send it over serial (don't worry about the floating point math the int typecast automatically converts it after but it will throw a warning oh well)
  int data[11] = {-127, ID, (int)analogRead(joints[0])*0.263671875,(int)analogRead(joints[1])*0.263671875,0,0,0,0,0,0 };


  //Figure out if the buttons are being pressed or not
  for(int i = 0; i<6; i++){
    pinMode(buttons[i], INPUT);
    if(digitalRead(buttons[i] = 0) == HIGH){
      data[i+3]=1; //set the value of the array element to 1 for pressed (the +3 is to account for the start commands and joints)
    }
  }

  for(int z = 0; z<11; z++){
    Serial.write(data[z]); //Send the data array element by element (will be seen by reader as a series of integers)
  }
}


