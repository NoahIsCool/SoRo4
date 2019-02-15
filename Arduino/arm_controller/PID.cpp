#include "Arduino.h"

//12 bit accuracy = 2 ^ 12 = 4096
//0 to 4095
int MAX_ACCURACY = 4095


//base of rover arm (parallel to ground)
int baseAngle = 0;
//0 = 0 degrees
//4095 = 270 degrees
//TODO: make these final or whatever
//int MAX_BASE_ANGLE = 270;
int MAX_ANGLE = 270;
double CONVERSION_FACTOR = (double)MAX_ANGLE / (double)MAX_ACCURACY;

int SERVO_CENTER = 180;

//shoulder angle (perpendicular to ground)
int shoulderAngle = 0;
//0 = 0 degrees
//4095 = 270
//int MAX_SHOULDER_ANGLE = 270;

//elbow angle (perpendicular to ground)
int elbowAngle = 0;
//0 = 0
//4095 = 270
//int MAX_SHOULDER_ANGLE = 270;

//wrist angle (perpendicular to ground)
int wristAngle = 0;
//0 = 0
//4095 = 270
//int MAX_WRIST_ANGLE = 270;

//PID constants
//TODO: values
double kpBase = ;
double kiBase = ;
double kdBase = ;

double kpShoulder = ;
double kiShoulder = ;
double kdShoulder = ;

double kpElbow = ;
double kiElbow = ;
double kdElbow = ;

double kpWrist = ;
double kiWrist = ;
double kdWrist = ;

//PID vars
//TODO: can these be shared?
unsigned long previousTimeBase = 0;
int previousErrorBase = 0;
double integralBase = 0;

unsigned long previousTimeShoulder = 0;
int previousErrorShoulder = 0;
double integralShoulder = 0;

unsigned long previousTimeElbow = 0;
int previousErrorElbow = 0;
double integralElbow = 0;

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


private void PID::moveBase(int baseTargetPosition)
{
	if (sum < 0 || sum > 4095)
	{
		//TODO: throw error
	}

	//convert from 0-4095 to 0-270
	baseTargetAngle = (int)(CONVERSION_FACTOR * baseTargetPosition);

	unsigned long currentTime = millis();
	int dt = currentTime - previousTimeShoulder;//change in time
	previousTimeShoulder = currentTimeShoulder;//update old time

	//error between what it is and what we want
	int error = baseTargetPosition - basePosition;


	//integral/sum of error
	integralBase += error * dt / 1000.0;//divide by 1000 because dt is ms, adjust for seconds


	//derivative/rate of change of error
	double derivative = 1000.0*(error - previousErrorBase) / dt;

	int sum = (int)(kpBase * error + kiBase * integralBase + kdBase * derivative);

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

}



