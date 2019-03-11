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
#define LTHRESH17 900
#define CTHRESH17 900
#define RTHRESH17 900
byte  left_speed17  = 127;
byte  right_speed17 = 127;
bool speed_locked17;
#define DELTA     5

//Initial Variables
int   grip_angle  = 40; 
int   theChar     = 0;
int   start_position = 0;

//Make robot go backwards
void backup() {
  digitalWrite(motorR, LOW);
  analogWrite(speedR, 155);
  digitalWrite(motorL, LOW);
  analogWrite(speedL, 150);
  if(leftIR == LTHRESH && centerIR == CTHRESH && rightIR == RTHRESH){
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
  if(leftIR == LTHRESH && centerIR == CTHRESH && rightIR == RTHRESH){
  analogWrite(speedL, 0);
  analogWrite(speedR, 0);
  }
}

//Make robot go left
void left() {
  digitalWrite(motorR, HIGH);
  analogWrite(speedR, left_speed);
  digitalWrite(motorL, LOW);
  analogWrite(speedL, 0);
  if(leftIR == LTHRESH && centerIR == CTHRESH && rightIR == RTHRESH){
  analogWrite(speedL, 0);
  analogWrite(speedR, 0);
  }
}

//Make robot go right
void right() {
  digitalWrite(motorR, LOW);
  analogWrite(speedR, 0);
  digitalWrite(motorL, HIGH);
  analogWrite(speedL, right_speed);
  if(leftIR == LTHRESH && centerIR == CTHRESH && rightIR == RTHRESH){
  analogWrite(speedL, 0);
  analogWrite(speedR, 0);
  }
}

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
    Backup();
    delay(1000);
    digitalWrite(motorL, HIGH);
    digitalWrite(motorR, LOW);
    analogWrite(speedR, 105);
    analogWrite(speedL, 100);
    delay(1500);
  } 
  else if (digitalRead(bumpL) && !digitalRead(bumpR)) {
    Backup();
    delay(1000);
    digitalWrite(motorL, LOW);
    digitalWrite(motorR, HIGH);
    analogWrite(speedR, 105);
    analogWrite(speedL, 100);
    delay(1000);
  } 
  else if (!digitalRead(bumpL) && digitalRead(bumpR)){
    Backup();
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
    left_speed17-=DELTA;
    right_speed17+=DELTA;
    Serial.println("increasing right, decreasing left");
  }
  else if(digitalRead(bumpL) && !digitalRead(bumpR))
  {
    left_speed17+=DELTA;
    right_speed17-=DELTA;
    Serial.println("increasing left, decreasing right");
  }
  //user indicates we're going straight now
  else if(!digitalRead(bumpL) && !digitalRead(bumpR))
  {
    
    Serial.println("comitting speed changes");
    EEPROM.write(0, left_speed17);  //save left speed to EEPROM address 0
    EEPROM.write(1, right_speed17); //save right speed to EEPROM address 1
    right_speed17 = left_speed17 = 0; //set wheel speeds to zero to stop Bot
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

//Determines the starting position of the robot, need to write
void whichStart() {
  no_start = 0;
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
  }
}




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
  WaitButton();
  speed_locked17 = false;
  
}

void loop() {
int cycle = 0; // Used to be able to make sure that no collisions occur

//If we are using the hard coding method, this will be the format we will use.
switch(cycle){
  case //number
    if(start_position == 1){
      //path
     }
     
     else if(start_position == 2){
     //second path
     }

    else //Third Start Position{
      // third path
     }
    
}


  
}
