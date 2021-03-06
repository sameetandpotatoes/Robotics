#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, in1,    statusPort,     sensorAnalog)
#pragma config(Sensor, dgtl1,  ,               sensorDigitalIn)
#pragma config(Sensor, dgtl2,  shaftEncoder,   sensorQuadEncoder)
#pragma config(Sensor, dgtl4,  pneuVal,        sensorDigitalOut)
#pragma config(Sensor, dgtl5,  armLimit,       sensorDigitalIn)
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
void returnLiftToGround();
void turnMotors(int direction);
void verticalMotors(int direction);
void sideMotors(int direction);
void stopDrive();
void moveArmToPreload(int armSpeed);
void moveArmToScore(int armSpeed);
void clearSkyrise();
void pickUpNextSkyrise();
void pickUpNextSection();
void elevateLift(int height);
void returnLiftToPreload();
void liftMotors(int direction);
void stopLift();
int armSpeedAutonomous = 60;
int armSpeedUserControl = 70;
int scoredSections = 0;
int direction = 1;
bool killSwitchActivated = false;

void pre_auton(){
}
task autonomous(){
	scoredSections = 0;
	direction = 1;
	SensorValue[pneuVal] = 0;
	if (SensorValue[disableAut] == 1){
		//If we are on the blue side, preload and scoring are switched
		//To fix this, switch the direction of all horizontal motor movements
		if (SensorValue[nextToSection] == 0){ //If its plugged in
			//Switch direction so preload is now on the other side and we can use the same code
			direction = -1;
		}
		bool firstTime = true;
		while(scoredSections < 6){
			if (firstTime){
				//Release the arm only on the first time
				if (direction == -1){
					motor[arm] = armSpeedAutonomous;
					wait1Msec(200);
					motor[arm] = 0;
				}
				//Raise the lift slightly (for 0.8 sec) to expand arm
				liftMotors(1);
				wait1Msec(600);
				//Open the clamp
				SensorValue[pneuVal] = 1;
				stopLift();

				motor[arm] = direction * -armSpeedAutonomous;
				wait1Msec(250);
				motor[arm] = 0;
			}
			//Move arm to pick up section
			pickUpSection();

			//Start lifting process
			//This needs to be our '0' spot so we come back to here
			SensorValue[shaftEncoder] = 0;
			//Regression Quadratic Equation
			int calculatedHeight;
			if (firstTime){
				calculatedHeight = 235;
				} else{
				calculatedHeight = (289.0 * scoredSections) + 85.33;
			}
			elevateLift(calculatedHeight);

			//Scoring
			moveArmToScore(direction * -1 * armSpeedUserControl);
			//if (scoredSections >= 2){
			//	wait1Msec(100);
			//}
		int nestValue = (firstTime) ? -330 : (int) (-867/7);
			elevateLift(nestValue);
			wait1Msec(50);
			SensorValue[pneuVal] = 1;
		int clearValue = (firstTime) ? 200 : 230;
			firstTime = false;
			elevateLift(clearValue);

			//Return to starting position
			//get the arm going in that direction for a bit so it doesn't get caught on anything
			//motor[arm] = (direction * armSpeedAutonomous);
			//wait1Msec(400);
			//motor[arm] = 0;
			clearSkyrise();
			returnLiftToPreload();

			scoredSections++;
		}
	}
}
task usercontrol(){
	while (true){
		//Get the drive values from the joystick
		int strafeY = vexRT[Ch1];
		int strafeX = vexRT[Ch3];
		int rotate = -vexRT[Ch4];

		//Map joystick values to each of the drive motors
		motor[backRight] 	= (strafeY - rotate + strafeX);
		motor[backLeft] 	= (strafeY + rotate - strafeX);
		motor[frontRight] = (strafeY - rotate - strafeX);
		motor[frontLeft] 	= (strafeY + rotate + strafeX);

		//Pneumatics
	SensorValue[pneuVal] = (vexRT[Btn7U] || vexRT[Btn7UXmtr2]) ? 1 :
	(vexRT[Btn7D] || vexRT[Btn7DXmtr2]) ? 0 : SensorValue[pneuVal];

		//Lift Control
		motor[liftLeft] =
	((vexRT[Btn6D] || vexRT[Btn6DXmtr2])|| ((vexRT[Btn6D] || vexRT[Btn6DXmtr2]))) ? 127 :
	(vexRT[Btn6U] || vexRT[Btn6UXmtr2]) ? -127 : 0;

		motor[liftRight] =
	((vexRT[Btn6D] || vexRT[Btn6DXmtr2])|| ((vexRT[Btn6D] || vexRT[Btn6DXmtr2]))) ? -127 :
	(vexRT[Btn6U] || vexRT[Btn6UXmtr2]) ? 127 : 0;

		//Arm
		motor[arm] =
	(vexRT[Btn7L] || vexRT[Btn7LXmtr2]) ? (armSpeedUserControl) :
	(vexRT[Btn7R] || vexRT[Btn7RXmtr2]) ? (-armSpeedUserControl) : 0;

		if ((vexRT[Btn8D] || vexRT[Btn8DXmtr2])){
			returnLiftToGround();
		}
	}
}
// Pick up next section
void pickUpNextSection(){
	if (direction == -1){
		motor[arm] = -1 * armSpeedAutonomous;
		wait1Msec(1500);
		motor[arm] = 0;
		} else{
		moveArmToPreload(direction * armSpeedAutonomous);
	}
	wait1Msec(250);
	//Close clamp on section
	SensorValue[pneuVal] = 0;
}
//Clear arm away from skyrise
void clearSkyrise(){
	motor[arm] = (direction * armSpeedAutonomous);
	wait1Msec(400);
	motor[arm] = 0;
}

//Picks up next skyrise section after scoring the next one
void pickUpNextSkyrise(){
	clearSkyrise()
	returnLiftToPreload();
	moveArmToPreload();
}

//Uses the button sensor
void returnLiftToGround(){
	while (SensorValue[shaftEncoder] > 0){ //While its not pressed
		liftMotors(-1);
	}
	stopLift();
}

//Moves the arm to the preload side
void moveArmToPreload(int armSpeed){
	motor[arm] = armSpeed;
	wait1Msec(400); //Enough time to get it away from one side
	while (SensorValue[armLimit] == 1){ //While its not pressed
		motor[arm] = armSpeed;
	}
	motor[arm] = 0;
}
//Moves the arm to the scoring side
void moveArmToScore(int armSpeed){
	motor[arm] = armSpeed;
	wait1Msec(400); //Enough time to get it away from one side
	while (SensorValue[armLimit] == 1){ //While its not pressed
		motor[arm] = armSpeed;
	}
	//Keep motor running to account for the whiplash
	motor[arm] = armSpeed;
int waitTime = (scoredSections >= 2) ? 1200 : 950;
	if (scoredSections == 0){
		waitTime = 600;
	}
	if (direction == -1 && scoredSections >= 1){
		waitTime = 450;
	}
	wait1Msec(waitTime);
	motor[arm] = 0;
}
//Raises or lowers the lift by a certain change in height,
//measured with the shaft encoder
void elevateLift(int height){
	height *= -1;
	int target = SensorValue[shaftEncoder] + height;
	if (height < 0){
		while (SensorValue[shaftEncoder] > target){
			liftMotors(1);
		}
		stopLift();
		} else {
		while (SensorValue[shaftEncoder] < target){
			liftMotors(-1);
		}
		stopLift();
	}
	stopLift();
}
//Returns the lift back until shaft encoder count is 0
//Requires "ground" position to be 0 for this to work!
void returnLiftToPreload(){
	while (abs(SensorValue[shaftEncoder]) > 45){
		liftMotors(-1);
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

//Stops all motors associated with the drive
void stopDrive(){
	motor[frontLeft] = 0;
	motor[frontRight] = 0;
	motor[backLeft] = 0;
	motor[backRight] = 0;
}
