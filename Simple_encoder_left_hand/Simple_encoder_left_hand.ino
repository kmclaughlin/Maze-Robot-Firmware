#include <Encoder.h>
#include <Ultrasonic.h>

#define MIN_MOTOR_SPEED 1
#define MAX_MOTOR_SPEED 255

#define LED 13

//Motor pins
#define R_MOTOR_ENABLE 5
#define L_MOTOR_ENABLE 6
#define R_MOTOR_1 8
#define R_MOTOR_2 7
#define L_MOTOR_1 11
#define L_MOTOR_2 9

//encoders
Encoder leftEncoder(3, 4);
Encoder rightEncoder(2, 10);

Ultrasonic frontSensor(A5, A4, 60000UL);
Ultrasonic leftSensor(A3, A2, 60000UL);
Ultrasonic rightSensor(A1, A0, 60000UL);
          
bool motorControlFlag = false;

long leftEncoderStore, rightEncoderStore;
volatile int rightMotorSpeed, leftMotorSpeed;
bool useLeftHandRule = false;

// motor control interrupt timer
ISR(TIMER0_COMPA_vect) {
  //alternate left and right motor control to reduce processor load. Each now called every 2ms
  if (motorControlFlag) {
  //leftMotor
    long encoderReading = leftEncoder.read();
    long measuredSpeed = abs(leftEncoderStore - encoderReading) * 15;
    leftEncoderStore = encoderReading;
    if (measuredSpeed > leftMotorSpeed)
      digitalWrite(L_MOTOR_ENABLE, LOW);
    else
      digitalWrite(L_MOTOR_ENABLE, HIGH);
  }
  else {
  //rightMotor
    long encoderReading = rightEncoder.read();
    long measuredSpeed = abs(rightEncoderStore - encoderReading) * 15;
    rightEncoderStore = encoderReading;
    if (measuredSpeed > rightMotorSpeed)
      digitalWrite(R_MOTOR_ENABLE, LOW);
    else
      digitalWrite(R_MOTOR_ENABLE, HIGH);
  }
  motorControlFlag = !motorControlFlag;
}

void setup() {
  //https://learn.adafruit.com/multi-tasking-the-arduino-part-2/timers
  //https://www.teachmemicro.com/arduino-timer-interrupt-tutorial/
  //https://arduino.stackexchange.com/questions/30968/how-do-interrupts-work-on-the-arduino-uno-and-similar-boards
  OCR0A = 0xAF; // set the compare register A for timer0
  TIMSK0 |= _BV(OCIE0A);  //enable the compare interrupt A for timer 0
  
  Serial.begin(9600);
  //setup motors
  pinMode(L_MOTOR_1, OUTPUT);
  pinMode(L_MOTOR_2, OUTPUT);
  pinMode(R_MOTOR_1, OUTPUT);
  pinMode(R_MOTOR_2, OUTPUT);
  pinMode(L_MOTOR_ENABLE, OUTPUT);
  pinMode(R_MOTOR_ENABLE, OUTPUT);
  pinMode(LED, OUTPUT);

  //chooseMazeRule();
}

void loop() {
 // if (useLeftHandRule)
 //   leftHandRule();
 // else 
    rightHandRule();
}

void chooseMazeRule(){
  int leftDistance = leftSensor.read();
  delay(60);
  int rightDistance = rightSensor.read();
  if (leftDistance < rightDistance){
    useLeftHandRule = true;
    for (int i = 0; i < 2; i++){
      digitalWrite(LED, HIGH);
      delay(100);
      digitalWrite(LED, LOW);
      delay(200);
    }
  }
  else {
    useLeftHandRule = false;
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, LOW);
    delay(200);
    digitalWrite(LED, HIGH);
  }
}

void leftHandRule(){
  delay(40);
  if (frontSensor.read() < 10){
    turnRight();
  }
  else {
    wallFollowLeft();
  }
}

void rightHandRule(){
  delay(40);
  if (frontSensor.read() < 10){
    turnLeft();
  }
  else {
    wallFollowRight();
  }
}

void wallFollowLeft(){
  int leftDistance = leftSensor.read();
  if (leftDistance < 4){
    driveLeftMotor(100);
    driveRightMotor(-100);
  } else if (leftDistance < 8){
    driveLeftMotor(180);
    driveRightMotor(60);
  } else if (leftDistance > 10) {
    driveLeftMotor(60);
    driveRightMotor(180);
  } else {
    driveLeftMotor(180);
    driveRightMotor(180);
  }
}

void wallFollowRight(){
  int rightDistance = rightSensor.read();
  if (rightDistance < 4){
    driveLeftMotor(-120);
    driveRightMotor(120);
  } else if (rightDistance < 8){
    driveLeftMotor(60);
    driveRightMotor(180);
  } else if (rightDistance > 10) {
    driveLeftMotor(180);
    driveRightMotor(60);
  } else {
    driveLeftMotor(180);
    driveRightMotor(180);
  }
}

void turnRight(){
  driveLeftMotor(500);
  driveRightMotor(-500);
  delay(190);
  driveLeftMotor(0);
  driveRightMotor(0);
  delay(100);
}

void turnLeft(){
  driveLeftMotor(-500);
  driveRightMotor(500);
  delay(190);
  driveLeftMotor(0);
  driveRightMotor(0);
  delay(100);
}

//MOTOR DRIVES
void driveLeftMotor(int speed){
  leftMotorSpeed = abs(speed);
  if (leftMotorSpeed < MIN_MOTOR_SPEED) {  
    digitalWrite(L_MOTOR_1, LOW);
    digitalWrite(L_MOTOR_2, LOW);  
  }
  else {
    if (speed > 0){
      digitalWrite(L_MOTOR_1, HIGH);
      digitalWrite(L_MOTOR_2, LOW);
    }
    else{
      digitalWrite(L_MOTOR_1, LOW);
      digitalWrite(L_MOTOR_2, HIGH);
    }
  }
}

void driveRightMotor(int speed){
  rightMotorSpeed = abs(speed);
  if (rightMotorSpeed < MIN_MOTOR_SPEED) { 
    digitalWrite(R_MOTOR_1, LOW);
    digitalWrite(R_MOTOR_2, LOW);  
  }
  else {
    if (speed > 0){
      digitalWrite(R_MOTOR_1, LOW);
      digitalWrite(R_MOTOR_2, HIGH);
    }
    else {
      digitalWrite(R_MOTOR_1, HIGH);
      digitalWrite(R_MOTOR_2, LOW);
    }
  }
}
