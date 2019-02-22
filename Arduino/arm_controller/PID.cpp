#include "Arduino.h"
#include "PID.h"

/*
* Constructor
*/
PID::PID(const byte baseMotor, const byte shoulderMotor, const byte elbowMotor, const byte wristMotor)
{
	//set the minmode of the motor ports to be output
	pinMode(baseMotor, OUTPUT);
	pinMode(shoulderMotor, OUTPUT);
	pinMode(elbowMotor, OUTPUT);
	pinMode(wristMotor, OUTPUT);

	//set ports
	_baseMotor = baseMotor;
	_shoulderMotor = shoulderMotor;
	_elbowMotor = elbowMotor;
	_wristMotor = wristMotor;
	
	baseAngle = 0;
	shoulderAngle = 0;
	elbowAngle = 0;
	wristAngle = 0;

	//PID vars
	//TODO: can these be shared?
	previousTimeBase = 0;
	previousErrorBase = 0;
	integralBase = 0;

	previousTimeShoulder = 0;
	previousErrorShoulder = 0;
	integralShoulder = 0;

	previousTimeElbow = 0;
	previousErrorElbow = 0;
	integralElbow = 0;

	previousTimeWrist = 0;
	previousErrorWrist = 0;
	integralWrist = 0;
}

/*
* Destructor
*/
PID::~PID()
{
}


/*
* Reads in the message from the Jetson, splits into segments,
* and calls individual movement method for each joint in the arm
*/
void PID::adjustArm(long messageNumber)
{

	baseTarget = ;//first part of messageNumber
	shoulderTarget = ;//second part of messageNumber
	elbowTarget = ;//third part of messageNumber
	wristTarget = ;//fourth part of messageNumber
	moveBase(baseTarget);
	moveShoulder(shoulderTarget);
	moveElbow(elbowTarget);
	moveWrist(wristTarget);
}

/*
* Receives the movement info from the adjustArm method
* and converts it into a number the base motor can use.
* Has PID for control system
*/
private void PID::moveBase(int baseTargetPosition)
{
	if (sum < 0 || sum > 4095)
	{
		//TODO: throw error
	}

	//convert from 0-4095 to 0-270
	baseTargetAngle = (int)(CONVERSION_FACTOR * baseTargetPosition);

	unsigned long currentTime = millis();
	int dt = currentTime - previousTimeBase;//change in time
	previousTimeShoulder = currentTimeBase;//update old time

	//error between what it is and what we want
	int error = baseTargetPosition - basePosition;


	//integral/sum of error
	integralBase += error * dt / 1000.0;//divide by 1000 because dt is ms, adjust for seconds


	//derivative/rate of change of error
	double derivative = 1000.0*(error - previousErrorBase) / dt;

	int sum = (int)(KP_BASE * error + KI_BASE * integralBase + KD_BASE * derivative);

	previousErrorBase = error;

	//TODO:output to servo
	//working w/ full rotational servos
	//90 = No Movement, 0 full speed one direction, 180 full speed other direction

	int output = SERVO_CENTER + sum;

	if (output > 180)
	{
		output = 180;
	}
	if (output < 0)
	{
		output = 0;
	}

	//TODO: write to servo
	analogWrite(_baseMotor, output);
}

/*
* Receives the movement info from the adjustArm method
* and converts it into a number the shoulder motor can use.
* Has PID for control system.
*/
private void PID::moveShoulder(int shoulderTargetPosition)
{
	if (sum < 0 || sum > 4095)
	{
		//TODO: throw error
	}

	//convert from 0-4095 to 0-270
	shoulderTargetAngle = (int)(CONVERSION_FACTOR * shoulderTargetPosition);

	unsigned long currentTime = millis();
	int dt = currentTime - previousTimeShoulder;//change in time
	previousTimeShoulder = currentTimeShoulder;//update old time

	//error between what it is and what we want
	int error = shoulderTargetPosition - shoulderPosition;


	//integral/sum of error
	integralShoulder += error * dt / 1000.0;//divide by 1000 because dt is ms, adjust for seconds


	//derivative/rate of change of error
	double derivative = 1000.0*(error - previousErrorShoulder) / dt;

	int sum = (int)(KP_SHOULDER * error + KI_SHOULDER * integralShoulder + KD_SHOULDER * derivative);

	previousErrorShoulder = error;

	//TODO:output to servo
	//working w/ full rotational servos
	//90 = No Movement, 0 full speed one direction, 180 full speed other direction

	int output = SERVO_CENTER + sum;

	if (output > 180)
	{
		output = 180;
	}
	if (output < 0)
	{
		output = 0;
	}

	//TODO: write to servo
	analogWrite(_shoulderMotor, output);
}

/*
* Receives the movement info from the adjustArm method
* and converts it into a number the elbow motor can use.
* Has PID for control system.
*/
private void PID::moveElbow(int elbowTargetPosition)
{
	if (sum < 0 || sum > 4095)
	{
		//TODO: throw error
	}

	//convert from 0-4095 to 0-270
	elbowTargetAngle = (int)(CONVERSION_FACTOR * elbowTargetPosition);

	unsigned long currentTime = millis();
	int dt = currentTime - previousTimeElbow;//change in time
	previousTimeShoulder = currentTimeElbow;//update old time

	//error between what it is and what we want
	int error = elbowTargetPosition - elbowPosition;


	//integral/sum of error
	integralElbow += error * dt / 1000.0;//divide by 1000 because dt is ms, adjust for seconds


	//derivative/rate of change of error
	double derivative = 1000.0*(error - previousErrorElbow) / dt;

	int sum = (int)(KP_ELBOW * error + KI_ELBOW * integralElbow + KD_ELBOW * derivative);

	previousErrorElbow = error;

	//TODO:output to servo
	//working w/ full rotational servos
	//90 = No Movement, 0 full speed one direction, 180 full speed other direction

	int output = SERVO_CENTER + sum;

	if (output > 180)
	{
		output = 180;
	}
	if (output < 0)
	{
		output = 0;
	}

	//TODO: write to servo
	analogWrite(_elbowMotor, output);
}

/*
* Receives the movement info from the adjustArm method
* and converts it into a number the wrist motor can use.
* Has PID for control system.
*/
private void PID::moveWrist(int wristTargetPosition)
{
	if (sum < 0 || sum > 4095)
	{
		//TODO: throw error
	}

	//convert from 0-4095 to 0-270
	wristTargetAngle = (int)(CONVERSION_FACTOR * wristTargetPosition);

	unsigned long currentTime = millis();
	int dt = currentTime - previousTimeWrist;//change in time
	previousTimeShoulder = currentTimeWrist;//update old time

	//error between what it is and what we want
	int error = wristTargetPosition - wristPosition;


	//integral/sum of error
	integralWrist += error * dt / 1000.0;//divide by 1000 because dt is ms, adjust for seconds


	//derivative/rate of change of error
	double derivative = 1000.0*(error - previousErrorWrist) / dt;

	int sum = (int)(KP_WRIST * error + KI_WRIST * integralWrist + KD_WRIST * derivative);

	previousErrorWrist = error;

	//TODO:output to servo
	//working w/ full rotational servos
	//90 = No Movement, 0 full speed one direction, 180 full speed other direction

	int output = SERVO_CENTER + sum;

	if (output > 180)
	{
		output = 180;
	}
	if (output < 0)
	{
		output = 0;
	}

	//TODO: write to servo
	analogWrite(_wristMotor, output);
}