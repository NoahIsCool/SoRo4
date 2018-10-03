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
int ID = 122; //<----------------SET THIS TO SOMETHING THAT WORKS --------------------------------------



//Port Numbers 
int buttons[4] = {0,1,2,3}; //rollLeft (CCW from the rover's perspective), rollRight (CW from the rover's perspective), let go, grab (digital ports connected to buttons)
int joints[4] = {0,1,2,3}; //yaw,shoulder, elbow, wrist pitch (the analog ports the potentiometers are connected to)

void setup() {
  //Start talking to the computer
  Serial.begin(9600);
}

void loop() {
  
  //this doesn't have to be in a seperate function I did this in case we want to add stuff not taking data later.
  sendData(); 
}

void sendData(){
  //data is used as a "packet" for when we send it over serial (don't worry about the floating point math the int typecast automatically converts it after but it will throw a warning oh well)
  //0.2490234375 is 255/1024 to convert the units of the potentiometer to fit in a byte
  //On the other side you must multiply by 270/255 (1.0588235294) to convert it to degrees and 0.0184799567 to convert to radians (don't use radians you weirdo thats for math not humans)
  int data[11] = {-127, ID, (int)analogRead(joints[0])*0.2490234375,(int)analogRead(joints[1])*0.2490234375,(int)analogRead(joints[2])*0.2490234375,(int)analogRead(joints[3])*0.2490234375,0,0,0,0};


  //Figure out if the buttons are being pressed or not
  for(int i = 0; i<4; i++){
    
    //Set the digital port i to pinmode of input so we can read data from it. (we don't ever need to set it to output I just did this for security)
    pinMode(buttons[i], INPUT);
    
    //If the button is pressed run this if statement
    if(digitalRead(buttons[i] = 0) == HIGH){
      
      //set the value of the array element to 1 for pressed (the +6 is to account for the start commands and joints)
      data[i+6]=1; 
      
    }
  }
  //Cycle throught all elements of the message and send
  sum = 0;
  for(int z = 0; z<11; z++){
    
    //Send the data array element by element (will be seen by reader as a series of integers)
    Serial.write(data[z]); 
    sum = sum + data[z];
  }
  Serial.write(sum/11); //NOT SURE IF I WANT TO AVERAGE THE START COMMAND AND ID OR NOT <----------------------------
}


