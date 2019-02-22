#ifndef PID_h
#define PID_h

class PID
{
public:
	/*
	
	*/
	PID(const byte baseMotor, const byte shoulderMotor, const byte elbowMotor, const byte wristMotor);
	/*
	*Destructor
	*/
	~PID();

	/*
	*reads in the message from the Jetson, splits into segments,
	*and calls individual movement method for each joint in the arm
	*/
	void PID::adjustArm(long messageNumber);



private:

	//12 bit accuracy = 2 ^ 12 = 4096 ->0 to 4095
	const int MAX_ACCURACY = 4095
	//base of rover arm (parallel to ground)

	const int MAX_ANGLE = 270;
	const double CONVERSION_FACTOR = (double)MAX_ANGLE / (double)MAX_ACCURACY;

	//no movement on servo
	const int SERVO_CENTER = 180;

	//base angle (perpendicular to ground)
	int baseAngle;
	//0 = 0 degrees
	//4095 = 270 degrees
	//TODO: make these final or whatever
	//int MAX_BASE_ANGLE = 270;

	//shoulder angle (perpendicular to ground)
	int shoulderAngle;
	//0 = 0 degrees
	//4095 = 270
	//int MAX_SHOULDER_ANGLE = 270;

	//elbow angle (perpendicular to ground)
	int elbowAngle;
	//0 = 0
	//4095 = 270
	//int MAX_SHOULDER_ANGLE = 270;

	//wrist angle (perpendicular to ground)
	int wristAngle;
	//0 = 0
	//4095 = 270
	//int MAX_WRIST_ANGLE = 270;

	//PID constants
	//TODO: tune
	const double KP_BASE = 1;
	const double KI_BASE = 0;
	const double KD_BASE = 0;

	const double KP_SHOULDER = 1;
	const double KI_SHOULDER = 0;
	const double KD_SHOULDER = 0;

	const double KP_ELBOW = 1;
	const double KI_ELBOW = 0;
	const double KD_ELBOW = 0;

	const double KP_WRIST = 1;
	const double KI_WRIST = 0;
	const double KD_WRIST = 0;


	//PID vars
	//TODO: can these be shared?
	unsigned long previousTimeBase;
	int previousErrorBase;
	double integralBase;

	unsigned long previousTimeShoulder;
	int previousErrorShoulder;
	double integralShoulder;

	unsigned long previousTimeElbow;
	int previousErrorElbow;
	double integralElbow;

	unsigned long previousTimeWrist;
	int previousErrorWrist;
	double integralWrist;

	byte _baseMotor;//Pin for base motor
	byte _shoulderMotor;//Pin for shoulder motor
	byte _elbowMotor;//Pin for elbow motor
	byte _wristMotor;//Pin for wrist motor


	/*
	* Receives the movement info from the adjustArm method
	* and converts it into a number the base motor can use.
	* Has PID for control system
	*/
	void PID::moveBase(int baseTargetPosition);
	/*
	*Receives the movement info from the adjustArm method
	* and converts it into a number the shoulder motor can use.
	* Has PID for control system
	*/
	void PID::moveShoulder(int shoulderTargetPosition);
	/*
	* Receives the movement info from the adjustArm method
	* and converts it into a number the elbow motor can use.
	* Has PID for control system
	*/
	void PID::moveElbow(int elbowTargetPosition);
	/*
	* Receives the movement info from the adjustArm method
	* and converts it into a number the base motor can use.
	* Has PID for control system
	*/
	void PID::moveWrist(int wristTargetPosition);
};


#endif