#include <Arduino.h>

/* Variables */
// PID values
int error = 0;
int lastError = 0;
int sumError = 0;
float KP = 20;
float KD = 10;
int baseSpeed = 130;
int leftSpeed = 0;
int rightSpeed = 0;
int checkP = 0;
// Tracking values
int started = 0;
// Line Sensor
int L1 = 0;
int L2 = 0;
int L3 = 0;
int L4 = 0;
int L5 = 0;

// Pin line sensor trai -> phai
int LP1 = 13;
int LP2 = 12;
int LP3 = 11;
int LP4 = 10;
int LP5 = 9;

// Ultrasonic sensor
long duration;
int distance;
const int trig = A4;
const int echo = A5;

// Drivers and motors
// Right
const int enA = 3;
const int in1 = 4;
const int in2 = 5;
// Left
const int in3 = 7;
const int in4 = 8;
const int enB = 6;

/** Initial Code **/
void setup()
{
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  // Sensor Pin Setup
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  // Motor Pin Setup
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  // pinMode(A2, INPUT);
  // pinMode(A3, INPUT);

  // Line Sensor Setup
  pinMode(L1, INPUT);
  pinMode(L2, INPUT);
  pinMode(L3, INPUT);
  pinMode(L4, INPUT);
  pinMode(L5, INPUT);

  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
}

// Distance calculation functions
int calculateDistance()
{
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  duration = pulseIn(echo, HIGH);
  distance = duration * 0.034 / 2;
  Serial.println(distance);
  return distance;
}

// 90 degree turn detection

// Function to control motor speed
void motorControl(int L, int R)
{
  analogWrite(enA, R);
  analogWrite(enB, L);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

void SensorValue()
{
  L1 = digitalRead(LP1);
  L2 = digitalRead(LP2);
  L3 = digitalRead(LP3);
  L4 = digitalRead(LP4);
  L5 = digitalRead(LP5);
}

void whiteLine()
{
  while (L1 == 1 && L3 == 0 && L5 == 1)
  {
    motorControl(baseSpeed, baseSpeed);
  }
}

void checkPoint()
{
  switch (checkP)
  {
  case 0:
    baseSpeed = 80;
    KP = 10;
    KD = 5;
    break;
  case 1:
    baseSpeed = 100;
    KP = 25;
    KD = 10;
    break;

  case 2:
    baseSpeed = 130;
    KP = 20;
    KD = 10;
    break;
  case 3:
    baseSpeed = 80;
    KP = 10;
    KD = 5;
    break;
  case 4:
    baseSpeed = 130;
    KP = 20;
    KD = 10;
    break;
  }
  checkP++;
  delay(200);
}

void ninety(int direction)
{
  if (direction == 1)
  {
    motorControl(0, baseSpeed);
    delay(1000);
  }
  if (direction == 2)
  {
    motorControl(baseSpeed, 0);
    delay(1000);
  }
}

void CheckError()
{
  if (L1 == 0 && L2 == 0 && L3 == 0 && L4 == 0 && L5 == 0) // All White
  {
    error = 0; // 00000
  }
  if (L1 == 1 && L2 == 1 && L3 == 1 && L4 == 1 && L5 == 1) // All black
  {
    checkPoint();
    error = 0; // 11111
  }
  if (L1 == 0 && L2 == 1 && L3 == 1 && L4 == 1 && L5 == 0) // Go Straight
  {
    error = 0; // 01110
  }
  if (L1 == 0 && L2 == 0 && L3 == 1 && L4 == 1 && L5 == 0) // Small left
  {
    error = 1; // 00110
  }
  if (L1 == 0 && L2 == 1 && L3 == 1 && L4 == 0 && L5 == 0) // Small right
  {
    error = -1; // 01100
  }
  if (L1 == 0 && L2 == 0 && L3 == 1 && L4 == 1 && L5 == 1) // medium left
  {
    error = 2; // 00111
  }
  if (L1 == 1 && L2 == 1 && L3 == 1 && L4 == 0 && L5 == 0) // medium right
  {
    error = -2; // 11100
  }
  if (L1 == 0 && L2 == 0 && L3 == 0 && L4 == 1 && L5 == 1) // Big left
  {
    error = 3; // 00011
  }
  if (L1 == 1 && L2 == 1 && L3 == 0 && L4 == 0 && L5 == 0) // Big right
  {
    error = -3; // 11000
  }
  if (L1 == 0 && L2 == 0 && L3 == 0 && L4 == 0 && L5 == 1) // Very Big left
  {
    error = 4; // 00001
  }
  if (L1 == 1 && L2 == 0 && L3 == 0 && L4 == 0 && L5 == 0) // Very Big right
  {
    error = -4; // 10000
  }
  if (L1 == 1 && L3 == 0 && L5 == 1) // White line
  {
    whiteLine();
  }
  if (L1 == 0 && L2 == 1 && L3 == 1 && L4 == 1 && L5 == 1) // 90 right
  {
    ninety(2); // 01111 -> 90 cua phai
  }
  if (L1 == 0 && L2 == 1 && L3 == 1 && L4 == 1 && L5 == 1) // 90 right
  {
    ninety(1); // 11110 -> 90 cua trai
  }
}
/* Main Code */
void loop()
{
  // Check if the barrier has opened and start the robot
  if (started == 0)
  {
    if (calculateDistance() <= 15)
    {
      delay(500);
      return;
    }
    else
    {
      started = 1;
      // Turn off led to indicate that the robot has started
      digitalWrite(LED_BUILTIN, LOW);
      // run forward to get out of starting position
      motorControl(baseSpeed, baseSpeed);
      delay(1000);
      return;
    }
  }

  // Robot has started
  if (started == 1)
  {
    SensorValue();
    CheckError();

    int motorSpeed = KP * error + KD * (error - lastError);
    // base = 100
    lastError = error;
    leftSpeed = baseSpeed + motorSpeed;
    rightSpeed = baseSpeed - motorSpeed;
    if (leftSpeed > 255)
      leftSpeed = 255;
    if (rightSpeed > 255)
      rightSpeed = 255;
    if (leftSpeed < 0)
      leftSpeed = 0;
    if (rightSpeed < 0)
      rightSpeed = 0;
    motorControl(leftSpeed, rightSpeed);
  }
  delay(10);
}
