//ELEC 299 Final Project Teams 16, 17, 18
#include <Servo.h>
#include <EEPROM.h> 
#include <QSerial.h>
Servo servo1Pan, servo2Tilt, servo3Grip;

/*#define bumpL 2
#define bumpR 3
#define M2 4
#define E2 5
#define E1 6
#define M1 7
#define grip 9
#define tilt 10
#define pan 11
#define LEL 12
#define LER 13 */

//Digital Pins
#define bumpL    2
#define bumpR    3
#define motorL   4
#define speedL   5
#define motorR   6
#define speedR   7
#define button   8
#define encodeL  12
#define encodeR  13

//Analog Pins
#define IR       A0
#define grip     A1
#define frontIR  A2
#define leftIR   A3
#define centreIR A4
#define rightIR  A5

//ASCII Character Definitions
#define L        76
#define C        67
#define R        82

//Initial Values for Team 17
#define LTHRESH 900
#define CTHRESH 900
#define RTHRESH 900
byte  left_speed  = 127;
byte  right_speed = 127;
bool speed_locked;
#define DELTA     5

//Initial Variables
int   grip_angle  = 40; 
int   theChar     = 0;
int   start_position = 0;

//Make robot go backwards
void backwards() {
  digitalWrite(motorR, LOW);
  analogWrite(speedR, 155);
  digitalWrite(motorL, LOW);
  analogWrite(speedL, 150);
  if(leftIR == LTHRESH && centreIR == CTHRESH && rightIR == RTHRESH){
  analogWrite(speedL, 0);
  analogWrite(speedR, 0);
  }
}

//Make robot go forwards
void forward() {
  digitalWrite(motorR, HIGH);
  analogWrite(speedR, left_speed);
  digitalWrite(motorL, HIGH);
  analogWrite(speedL, right_speed);
  if(leftIR == LTHRESH && centreIR == CTHRESH && rightIR == RTHRESH){
  analogWrite(speedL, 0);
  analogWrite(speedR, 0);
  }
}

//Make robot go left
//void left() {
//  digitalWrite(motorR, HIGH);
//  analogWrite(speedR, left_speed);
//  digitalWrite(motorL, LOW);
//  analogWrite(speedL, 0);
//  if(leftIR == LTHRESH && centreIR == CTHRESH && rightIR == RTHRESH){
//  analogWrite(speedL, 0);
//  analogWrite(speedR, 0);
//  }
//}
//
////Make robot go right
//void right() {
//  digitalWrite(motorR, LOW);
//  analogWrite(speedR, 0);
//  digitalWrite(motorL, HIGH);
//  analogWrite(speedL, right_speed);
//  if(leftIR == LTHRESH && centreIR == CTHRESH && rightIR == RTHRESH){
//  analogWrite(speedL, 0);
//  analogWrite(speedR, 0);
//  }
//}

//Make the robot take a 180 degree turn, needs to be completed
void fullTurn(){
  digitalWrite(motorR, LOW);
  analogWrite(speedR, right_speed);
  digitalWrite(motorL, HIGH);
  analogWrite(speedL, left_speed);
  //delay until sometime and continue
}

//Collision Detection Code
/*void ReactBumpers() { //Need to revise for the competition
  if (!digitalRead(bumpL) && !digitalRead(bumpR)) {
    backwards();
    delay(1000);
    digitalWrite(motorL, HIGH);
    digitalWrite(motorR, LOW);
    analogWrite(speedR, 105);
    analogWrite(speedL, 100);
    delay(1500);
  } 
  else if (digitalRead(bumpL) && !digitalRead(bumpR)) {
    backwards();
    delay(1000);
    digitalWrite(motorL, LOW);
    digitalWrite(motorR, HIGH);
    analogWrite(speedR, 105);
    analogWrite(speedL, 100);
    delay(1000);
  } 
  else if (!digitalRead(bumpL) && digitalRead(bumpR)){
    backwards();
    delay(1000);
    digitalWrite(motorL, HIGH);
    digitalWrite(motorR, LOW);
    analogWrite(speedR, 105);
    analogWrite(speedL, 100);
    delay(1000);
  }
  else{}
}*/

//Determine the correct motor speed for your robot
void adjustSpeeds()
{
  if(!digitalRead(bumpL) || !digitalRead(bumpR))
    delay(500); //give user time to get the other bumper pressed too
  if(!digitalRead(bumpL) && digitalRead(bumpR))
  {
    left_speed-=DELTA;
    right_speed+=DELTA;
    Serial.println("increasing right, decreasing left");
  }
  else if(digitalRead(bumpL) && !digitalRead(bumpR))
  {
    left_speed+=DELTA;
    right_speed-=DELTA;
    Serial.println("increasing left, decreasing right");
  }
  //user indicates we're going straight now
  else if(!digitalRead(bumpL) && !digitalRead(bumpR))
  {
    
    Serial.println("comitting speed changes");
    EEPROM.write(0, left_speed);  //save left speed to EEPROM address 0
    EEPROM.write(1, right_speed); //save right speed to EEPROM address 1
    right_speed = left_speed = 0; //set wheel speeds to zero to stop Bot
    delay(100);
  }
}

//Waits for a button to be pressed, and then released. Used as start button for robot
void waitButton(){
   int state = 0;
   while(state <= 1)
  {
    if (!digitalRead(button))
    {
      delay(100);
      if (!digitalRead(button))
      state = 1;
    }
    else if (state==1)
    {
      if (digitalRead(button))
      state=2;
    }
  }
}

//Grabs an object, and then waits until both bumpers are hit to release the ball
void grab() {
  grip_angle++;
  servo3Grip.write(grip_angle);
  if(digitalRead(grip) >= 500){ //Need to change the value to compare -- just need to check if the gripper is sensing something
    while(bumpL == 0 || bumpR ==0){
    }
   grip_angle == 180;
   servo3Grip.write(grip_angle);
   delay(25);
  }
  delay(25);
 
  if( grip_angle >= 170){
    while(grip_angle >= 40){
      grip_angle--;
      servo3Grip.write(grip_angle);
      delay(20);
    }
  }
}

/*Determines the starting position of the robot, need to write
int whichStart() {
  int no_start = 0;
  while(no_start == 0){
   if IR sensor reads L
      start_position = 1;
      no_start = 1;
   else if if IR sensor reads C
      start_position = 2;
      no_start = 1;
   else if IR sensor reads R
      start_position = 3;
      no_start = 1;
  return start_position;
  }
}*/




#define convert 11.25


void turn(int deg) // negative 180 to 180 (left is neg.)
{
  deg = deg/convert;
  deg = deg*3;
  int count = 0;

  bool left, templeft;
  bool right, tempright;
  if (deg <0)
  {
    digitalWrite(motorR, LOW);
    digitalWrite(motorL, HIGH);
  }
  else
  {
    digitalWrite(motorR, HIGH);
    digitalWrite(motorL, LOW);
  }
  
  analogWrite(speedR, right_speed);
  analogWrite(speedL, left_speed);
    
  while (deg > count)
  {
    left = digitalRead(encodeL);
    right = digitalRead(encodeR);
    if (left != templeft)
    {
      count++;
    }
    if (right != tempright)
    {
      count++;
    }
    templeft = left;
    tempright = right;
  } 
  analogWrite(speedR, 0);
  analogWrite(speedL, 0);
}


#define up 160
#define down 70


void grabbb()
{
  //Hit wall -- Don't need
  //back up
  //lower grabber
  //grab
  //raise grabber
  //turn around?!?!?

  backup();
  int n = 0, m = 0, left, right, templeft, tempright;
  while (n < 6 || m < 6)
  {
    left = digitalRead(encodeL);
    right = digitalRead(encodeR);
    if (left != templeft)
    {
      n++;
    }
    if (right != tempright)
    {
      m++;
    }
    templeft = left;
    tempright = right;
  }
  servo2Tilt.write(down);
  int strength = analogRead(grip);
  int counttttt = 0;

  while (strength < 800)
  {
    counttttt++;
    if (counttttt > 160)
    {
      counttttt = 40;

    } else
    {
      servo2Tilt.write(counttttt);
      delay(5);
    }
    int strength = analogRead(grip);
  }
  servo2Tilt.write(up);
}

int planB()
{
 /*use ir front
 if (nothing to the left && something to the right)
    start_position=1
 else if (something to the left && nothing to the right)
    start_position=3
 else
    start_position=2
 return start_position
 */
 
}
void leftPath()
{
	//BALL 7
	forward()
	forward()
	forward()
	forward()
	forward()
	forward()
	if (bumpL==1 && bumpR==1)
		grab()
	else 
	{
		backward()
		//adjust
		forward()
	}
	forward()
	forward()
	forward()
	forward()
	forward()
	forward()
	if (bumpers hit)
		dropball()	
	else 
	{
		backward()
		//adjust
		forward()
	}
	//BALL 1
	forward()
	left()
	forward()
	forward()
	if (bumpL==1 && bumpR==1)
		grab()
	else 
	{
		backward()
		//adjust
		forward()
	}	backward()
	forward()
	forward()
	right()
	forward()
	if (bumpers hit)
		dropball()	
	else
	{
		backward()
		//adjust
		forward()
	}
		//BALL 13
	forward()
	forward()
	forward()
	right()
	forward()
	forward()
	forward()
	forward()
	if (bumpL==1 && bumpR==1)
		grab()
	else 
	{
		backward()
		//adjust
		forward()
	}	
	forward()
	forward()
	forward()
	forward()
	left()
	forward()
	forward()
	forward()
	if (bumpers hit)
		dropball()	
	else 
	{
		backward()
		//adjust
		forward()
	}		
	//BALL 4
	forward()
	forward()
	forward()
	forward()
	left()
	forward()
	forward()
	if (bumpL==1 && bumpR==1)
		grab()
	else 
	{
		backward()
		//adjust
		forward()
	}	
	forward()
	forward()
	right()
	forward()
	forward()
	forward()
	forward()
	if (bumpers hit)
		dropball()	
	else 
	{
		backward()
		//adjust
		forward()
	}
	//BALL 5
	forward()
	forward()
	forward()
	forward()
	forward()
	left() 
	forward()
	forward()
	if (bumpL==1 && bumpR==1)
		grab()
	else 
	{
		backward()
		//adjust
		forward()
	}	
	forward()
	forward()
	right()
	forward()
	forward()
	forward()
	forward()
	forward()
	if (bumpers hit)
		dropball()	
	else 
	{
		backward()
		//adjust
		forward()
	}
}

void centerPath()
{
	//BALL 8
	forward()
	forward()
	forward()
	forward()
	forward()
	forward()
	if (bumpL==1 && bumpR==1)
		grab()
	else 
	{
		backward()
		//adjust
		forward()
	}
	forward()
	forward()
	forward()
	forward()
	forward()
	forward()
	if (bumpers hit)
		dropball()	
	else 
		backward()
		//adjust
		forward()
	//BALL 2
	forward()
	forward()
	left()
	forward()
	forward()
	forward()
	if (bumpL==1 && bumpR==1)
		grab()
	else 
	{
		backward()
		//adjust
		forward()
	}	
	forward()
	forward()
	forward()
	right()
	forward()
	forward()
	if (bumpers hit)
		dropball()	
	else 
	{
		backward()
		//adjust
		forward()	
	}
	//BALL 14
	forward()
	forward()
	right()
	forward()
	forward()
	forward()
	if (bumpL==1 && bumpR==1)
		grab()
	else 
	{
		backward()
		//adjust
		forward()
	}	backward()
	forward()
	forward()
	forward()
	left()
	forward()
	forward()
	if (bumpers hit)
		dropball()	
	else 
	{
		backward()
		//adjust
		forward()	
	}
	//BALL 6
	forward()
	forward()
	forward()
	forward()
	forward()
	left()
	forward()
	forward()
	right()
	forward()
	if (bumpL==1 && bumpR==1)
		grab()
	else 
	{
		backward()
		//adjust
		forward()
	}	
	forward()
	left()
	forward()
	forward()
	right()
	forward()
	forward()
	forward()
	forward()
	forward()
	if (bumpers hit)
		dropball()	
	else 
	{
		backward()
		//adjust
		forward()	
	}
	//BALL 10
	forward()
	forward()
	forward()
	forward()
	forward()
	right()
	forward()
	forward()
	left()
	forward()
	if (bumpL==1 && bumpR==1)
		grab()
	else 
	{
		backward()
		//adjust
		forward()
	}	
	forward()
	right()
	forward()
	forward()
	left()
	forward()
	forward()
	forward()
	forward()
	forward()
	if (bumpers hit)
		dropball()	
	else 
	{
		backward()
		//adjust
		forward()
	}
}

void rightPath
{
	//BALL 9
	forward()
	forward()
	forward()
	forward()
	forward()
	forward()
	if (bumpL==1 && bumpR==1)
		grab()
	else 
	{
		backward()
		//adjust
		forward()
	}
	forward()
	forward()
	forward()
	forward()
	forward()
	forward()
	if (bumpers hit)
		dropball()	
	else 
	{
		backward()
		//adjust
		forward()
	}
	//BALL 3
	forward()
	forward()
	forward()
	left()
	forward()
	forward()
	forward()
	forward()
	if (bumpL==1 && bumpR==1)
		grab()
	else 
	{
		backward()
		//adjust
		forward()
	}
	forward()
	forward()
	forward()
	forward()
	right()
	forward()
	forward()
	forward()
	forward()
	if (bumpers hit)
		dropball()	
	else 
	{
		backward()
		//adjust
		forward()	
	}
	//BALL 15
	forward()
	right()
	forward()
	forward()
	if (bumpL==1 && bumpR==1)
		grab()
	else 
	{
		backward()
		//adjust
		forward()
	}
	forward()
	forward()
	left()
	forward()
	if (bumpers hit)
		dropball()	
	else 
	{
		backward()
		//adjust
		forward()
	}		
	//BALL 11
	forward()
	forward()
	forward()
	forward()
	forward()
	right()
	forward()
	forward()
	if (bumpL==1 && bumpR==1)
		grab()
	else 
	{
		backward()
		//adjust
		forward()
	}
	forward()
	forward()
	left()
	forward()
	forward()
	forward()
	forward()
	forward()
	if (bumpers hit)
		dropball()	
	else
	{		
		backward()
		//adjust
		forward()	
	}
	//BALL 12
	forward()
	forward()
	forward()
	forward()
	right()
	forward()
	forward()
	if (bumpL==1 && bumpR==1)
		grab()
	else 
	{
		backward()
		//adjust
		forward()
	}
	forward()
	forward()
	left()
	forward()
	forward()
	forward()
	forward()
	if (bumpers hit)
		dropball()	
	else 
	{
		backward()
		//adjust
		forward()
	}
}


void setup() {
  //Declarations
  Serial.begin(9600);
  servo3Grip.attach(9);
  servo2Tilt.attach(10);  
  servo1Pan.attach(11);
  servo1Pan.write(110);
  servo2Tilt.write(70);
  servo3Grip.write(40);
  pinMode(motorL, OUTPUT);
  pinMode(motorR, OUTPUT);
  pinMode(speedL, OUTPUT);
  pinMode(speedR, OUTPUT);
  pinMode(button,  INPUT);
  pinMode(bumpL,   INPUT);
  pinMode(bumpL,   INPUT);
  pinMode(encodeL, INPUT);
  pinMode(encodeR, INPUT);
  analogWrite(speedR,  0);
  analogWrite(speedL,  0);
  waitButton();
  speed_locked = false;
  
}





/*void loop 
{

int cycle = 0; // Used to be able to make sure that no collisions occur
int position=whichStart()
if (position!=0)          //IR sensor worked
	if(position == 1)
    leftPath()
  else if(position == 2)
    centerPath()
  else if(position == 3)
    rightPath()
else                    //check position based on other robots
  position= planB()   
  
      
	

}*/


  
