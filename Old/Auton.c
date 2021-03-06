#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, in1,    statusPort,     sensorAnalog)
#pragma config(Sensor, dgtl1,  ,               sensorDigitalIn)
#pragma config(Sensor, dgtl2,  shaftEncoder,   sensorQuadEncoder)
#pragma config(Sensor, dgtl4,  pneuVal,        sensorDigitalOut)
#pragma config(Sensor, dgtl5,  armLimit,       sensorDigitalIn)
#pragma config(Sensor, dgtl6,  blockPicker,    sensorDigitalOut)
#pragma config(Sensor, dgtl11, disableAut,     sensorDigitalIn)
#pragma config(Sensor, dgtl12, nextToSection,  sensorDigitalIn)
#pragma config(Sensor, I2C_1,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign)
#pragma config(Sensor, I2C_2,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign)
#pragma config(Motor,  port2,           frontRight,    tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port3,           backRight,     tmotorVex393_MC29, openLoop, driveRight, encoderPort, I2C_1)
#pragma config(Motor,  port4,           frontLeft,     tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port5,           backLeft,      tmotorVex393_MC29, openLoop, driveLeft, encoderPort, I2C_2)
#pragma config(Motor,  port6,           liftRight,     tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port7,           liftLeft,      tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port8,           arm,           tmotorVex393_MC29, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#pragma platform(VEX)
#pragma competitionControl(Competition)
#pragma autonomousDuration(15)
#pragma userControlDuration(105)

#include "VEX_Competition_Includes.c"

//Function Declarations
void turnMotors(int direction);
void verticalMotors(int direction);
void sideMotors(int direction);
void stopDrive();

void getNextSection();
void nextSkyrise();
void returnToStart();

void moveArmToPreload(int armSpeed);
void moveArmToScore(int armSpeed);
void elevateLift(int deltaHeight);
void returnLiftToPreload();
void liftMotors(int direction);
void stopLift();
void driveEncoderVertical(int encoderCounts);

const int speedArmAuton = 70;
const int speedArmUserControl = 70;
const int FIRST_HEIGHT = 190;
const int HEIGHT = 290;
const int FIRST_NEST_VAL = -246; //321+
const int NEST_VAL = -169;
const int FIRST_CLEAR_VAL = 156;
const int CLEAR_VAL = 180;

int scoredSections;
int direction;
bool killSwitch = false;

void pre_auton(){

}
task autonomous(){
	scoredSections = 0;
	SensorValue[pneuVal] = 0;
	//Score preload
	//SensorValue[blockPicker] = !SensorValue[blockPicker];
	if (SensorValue[disableAut] == 1){ //if auton sensor is not pressed
		bool firstTime = true;
		/*
			If on blue side, switch direction since preload is now on the other side
			and we can use the same code.
		*/
		direction = (SensorValue[nextToSection] == 0) ? -1 : 1;

		//Jerk the bot back and forth to release the preload onto the skyrise base
		if (direction == -1){
			driveEncoderVertical(-45);	//original: -70
			driveEncoderVertical(45);		//original: 45
		} else{
			driveEncoderVertical(-65);	//original: -45
			driveEncoderVertical(20);		//original: 20
		}
		wait1Msec(250);	//original: 500
		while(scoredSections < 2){
			if (firstTime){
				//Release the arm only on the first time

				//For blue side, move the arm back so it does not hit the section
				if (direction == -1){
					motor[arm] = speedArmAuton;
					wait1Msec(200);
					motor[arm] = 0;
				}
				//Raise the lift slightly to expand arm
				liftMotors(1);
				wait1Msec(600);
				//Open the clamp
				SensorValue[pneuVal] = 1;
				stopLift();

				/*
					Since the arm is fast, we need to pulse it back in the
					opposite direction so it does not knock into the preload.
				*/
				motor[arm] = -direction * speedArmAuton;
				wait1Msec(175);
				motor[arm] = 0;
				//driveEncoderVertical(10);
			}
			//Move arm to pick up section
			getNextSection();

			//Start lifting process
			//This needs to be our '0' spot so we come back to here
			SensorValue[shaftEncoder] = 0;
			//Regression Quadratic Equation
			int calculatedHeight =
				(firstTime) ? FIRST_HEIGHT : HEIGHT;
			elevateLift(calculatedHeight);

			//Move to scoring side
			moveArmToScore(-direction * speedArmAuton);

			//Bring down to score
			int nestValue =
				(firstTime) ? FIRST_NEST_VAL : NEST_VAL;
			elevateLift(nestValue);
			wait1Msec(150);

			//Open clamp
			SensorValue[pneuVal] = 1;

			//Raise lift to clear scored skyrise
			int clearValue = (firstTime) ? FIRST_CLEAR_VAL : CLEAR_VAL;
			elevateLift(clearValue);
			//Return to starting position
			returnToStart();

			scoredSections++;
			firstTime = false;
		}
	}
}
task usercontrol(){
	//In case autonomous was not run, figure out which side we are on again
	direction = (SensorValue[nextToSection] == 0) ? -1 : 1;
	// to check if autonomous was run, simply check: if scoredSections > 0
	while (true){
		//Get the drive values
		int strafeY = vexRT[Ch1];
		int strafeX = vexRT[Ch3];
		int rotate = -vexRT[Ch4];

		//Map joystick values to each of the drive motors
		motor[backRight] 	= (strafeY - rotate + strafeX);
		motor[backLeft] 	= (strafeY + rotate - strafeX);
		motor[frontRight] = (strafeY - rotate - strafeX);
		motor[frontLeft] 	= (strafeY + rotate + strafeX);

		//4Pneumatics Control
		SensorValue[pneuVal] =
			(vexRT[Btn7UXmtr2]) ? 1 :
			(vexRT[Btn7DXmtr2]) ? 0 : SensorValue[pneuVal];

		SensorValue[blockPicker] =
			(vexRT[Btn7U]) ? 1 :
			(vexRT[Btn7D]) ? 0 : SensorValue[blockPicker];


		//Lift Control
		motor[liftLeft] =
			(vexRT[Btn6D] || vexRT[Btn6DXmtr2]) ? 127 :
			(vexRT[Btn6U] || vexRT[Btn6UXmtr2]) ? -127 : 0;

		motor[liftRight] =
			(vexRT[Btn6D] || vexRT[Btn6DXmtr2]) ? -127 :
			(vexRT[Btn6U] || vexRT[Btn6UXmtr2]) ? 127 : 0;

		//Arm
		motor[arm] =
			(vexRT[Btn7L] || vexRT[Btn7LXmtr2]) ? (speedArmUserControl) :
			(vexRT[Btn7R] || vexRT[Btn7RXmtr2]) ? (-speedArmUserControl) : 0;

		//Button to pick up next skyrise (should be tested in all cases, but it should only be used after scoring the first one?
		if (vexRT[Btn8RXmtr2]){
			nextSkyrise();
			//Reset killswitch in case it was activated in this run
			killSwitch = false;
		}
	}
}
//Move arm to pick up and clamp section
void getNextSection(){
	if (!killSwitch){
		// TODO: This might change now because our limit switches might be in a different spot
		if (direction == -1){
			motor[arm] = -1 * speedArmAuton;
			wait1Msec(850);
			motor[arm] = 0;
		} else{
			moveArmToPreload(direction * speedArmAuton);
		}
		wait1Msec(150);
		//Close clamp on section
		SensorValue[pneuVal] = 0;
		SensorValue[shaftEncoder] = 0;
	}
}
//Bring lift down and prepare it to pick up the next skyrise
void returnToStart(){
	//get the arm going in that direction for a bit so it doesn't get caught on anything
	motor[arm] = (direction * speedArmAuton);
	wait1Msec(500);
	motor[arm] = 0;
	returnLiftToPreload();
}
//Pick up next skyrise section after clearing the last one
void nextSkyrise(){
	returnToStart();
	getNextSection();
	killSwitch = false;
	//returnLiftToPreload();
}

//Moves the arm to the preload side
void moveArmToPreload(int armSpeed){
	motor[arm] = armSpeed;
	wait1Msec(500); //Enough time to get it away from one side
	while (SensorValue[armLimit] == 1 && !killSwitch){
		motor[arm] = armSpeed;
		killSwitch = (vexRT[Btn8U] || vexRT[Btn8UXmtr2]) ? true : killSwitch;
	}
	motor[arm] = 0;
}

//Moves the arm to the scoring side
void moveArmToScore(int armSpeed){
	motor[arm] = armSpeed;
	wait1Msec(400); //Enough time to get it away from one side
	while (SensorValue[armLimit] == 1 && !killSwitch){
		motor[arm] = armSpeed;
		killSwitch = (vexRT[Btn8U] || vexRT[Btn8RXmtr2]) ? true : killSwitch;
	}
	//Keep motor running to account for the whiplash
	// TODO: Change wait times
	int waitTime = 700;
	wait1Msec(waitTime);
	motor[arm] = 0;
}

/*
	Raises or lowers the lift by a certain change in height,
	measured with the shaft encoder
*/
void elevateLift(int deltaHeight){
	int initialHeight = SensorValue[shaftEncoder];
	if (deltaHeight > 0){
		while (SensorValue[shaftEncoder] - initialHeight < deltaHeight){
			liftMotors(1);
		}
	} else {
		while (SensorValue[shaftEncoder] - initialHeight > deltaHeight){
			liftMotors(-1);
		}
	}
	stopLift();
}
//Returns the lift back until shaft encoder count is 0
//Requires "ground" position to be 0 for this to work!
void returnLiftToPreload(){
	while (abs(SensorValue[shaftEncoder]) > 45 && !killSwitch){
		liftMotors(-1);
		killSwitch = (vexRT[Btn8U] || vexRT[Btn8UXmtr2]) ? true : killSwitch;
	}
	stopLift();
}

//Activates the motors required for the lift
//@param direction: 1 is up, -1 is down
void liftMotors(int direction){
	motor[liftLeft] = -1 * direction * 127;
	motor[liftRight] = direction  * 127;
}

//Stops all lift motors
void stopLift(){
	motor[liftLeft] = 0;
	motor[liftRight] = 0;
}

//Unused, but kept just in case

//Activates the motors required to rotate robot
//@param direction: 1 is clockwise, -1 is counterclockwise
void turnMotors(int direction){
	motor[frontLeft] = -1 * direction * 127;
	motor[frontRight] = -1 * direction * 127;
	motor[backLeft] = direction * 127;
	motor[backRight] = direction * 127;
}
//Activates the motors required for vertical motion
//@param direciton: 1 is forward, -1 is backward
void verticalMotors(int direction){
	motor[frontLeft] = direction * 127;
	motor[frontRight] = -1 * direction * 127;
	motor[backLeft] = -1 * direction * 127;
	motor[backRight] = direction * 127;
}
//Activates the motors required to strafe robot
//@param direction: 1 is right, -1 is left
void sideMotors(int direction){
	motor[frontLeft] = -1 * direction * 127;
	motor[frontRight] = direction * 127;
	motor[backLeft] = direction * 127;
	motor[backRight] = -1 * direction * 127;
}

//Stops all drive motors
void stopDrive(){
	motor[frontLeft] = 0;
	motor[frontRight] = 0;
	motor[backLeft] = 0;
	motor[backRight] = 0;
}

//Back up
void driveEncoderVertical(int encoderCounts){
	int direction = (encoderCounts > 0) ? 1 : -1;
	int initialEncoder = getMotorEncoder(backRight);
	while (abs(getMotorEncoder(backRight) - initialEncoder) < abs(encoderCounts)){
		verticalMotors(direction);
	}
	stopDrive();
}
