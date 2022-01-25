#include <WiFi.h>
#include <ESP32Servo.h>

byte dutyCycle = 0;
byte arrayLength = 0;
const byte rightForward = 0;
const byte leftForward = 1;
const byte rightBackward = 2;
const byte leftBackward= 3;
byte previousState = 0;

byte receivedBytes[4]; 

Servo servoOne;
Servo servoTwo;
byte currentPosOne = 90;
byte currentPosTwo = 90;

void setup() {
  const byte RFPWM = 18;
  const byte LFPWM = 19;
  const byte RBPWM = 17;
  const byte LBPWM = 16;
  const int frequency = 5000;
  const byte resolution = 8;

  const byte servoOnePWM = 15;
  const byte servoTwoPWM = 2;

  arrayLength = sizeof receivedBytes / sizeof receivedBytes[0];


  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  servoOne.setPeriodHertz(50);
  servoTwo.setPeriodHertz(50);
  servoOne.attach(servoOnePWM, 1000, 2000);
  servoTwo.attach(servoTwoPWM, 1000, 2000);
  
  Serial.begin(9600);
  Serial.setDebugOutput(true);
  ledcSetup(rightForward, frequency, resolution);
  ledcSetup(leftForward, frequency, resolution);
  ledcSetup(rightBackward, frequency, resolution);
  ledcSetup(leftBackward, frequency, resolution);
 
  ledcAttachPin(RFPWM, rightForward);
  ledcAttachPin(LFPWM, leftForward);
  ledcAttachPin(RBPWM, rightBackward);
  ledcAttachPin(LBPWM, leftBackward);

  WiFi.mode(WIFI_OFF);
  servoOne.write(90);
  servoTwo.write(90);
}

void loop() {
  if(Serial.available() > 0) {
        Serial.readBytesUntil(10, receivedBytes, arrayLength + 1);   
        if(isStopCode())
        {
          allStop();
        }
        else
        {
          setDutyCycle();
          if(receivedBytes[0] == 53)
          {
            if(previousState != 0)
              setState(previousState);
          }
          else
          {
            setState(receivedBytes[0]);
            previousState = receivedBytes[0];
          }
        } 
        flushArray(); 
      }
}





void flushArray(){
  for(int i = 0; i < arrayLength; i++){
      receivedBytes[i] = 0;
    }
}

void setState(byte state)
{
  switch(state)
      {
        case 48:
          moveForward();
          break;
        case 50:
          reverse();
          break;
        case 49:
          turnRight();
          break;
        case 51:
          turnLeft();
          break;
        case 55:
          setServo(servoOne, currentPosOne);
          break;
        case 56:
          setServo(servoTwo, currentPosTwo);
          break;
        default:       
          break;
      }
}

byte setDutyCycle()
{
  byte index = 1;
  char temp[3];  
  while(receivedBytes[index] != 0 && index != arrayLength)
  {
    temp[index - 1] = (char)receivedBytes[index];
    index++;
  }
  dutyCycle = atoi(temp);
}

void moveForward()
{
   ledcWrite(rightForward, dutyCycle);
   ledcWrite(leftForward, dutyCycle);
   ledcWrite(rightBackward, 0);
   ledcWrite(leftBackward, 0);
}

void reverse()
{
  ledcWrite(rightForward, 0);
  ledcWrite(leftForward, 0);
  ledcWrite(rightBackward, dutyCycle);
  ledcWrite(leftBackward, dutyCycle);
}

void turnRight()
{
  ledcWrite(rightForward, 0);
  ledcWrite(leftForward, dutyCycle);
  ledcWrite(rightBackward, dutyCycle);
  ledcWrite(leftBackward, 0);
}


void turnLeft()
{
  ledcWrite(rightForward, dutyCycle);
  ledcWrite(leftForward, 0);
  ledcWrite(rightBackward, 0);
  ledcWrite(leftBackward, dutyCycle);
}

void setServo(Servo& servo, byte& pos) 
{
  if((pos != dutyCycle) && (dutyCycle >= 0) && (dutyCycle <= 180))
  {   
    servo.write(dutyCycle);
    pos = dutyCycle;
  }
}




bool isStopCode()
{
  for(int i = 0; i < 3; i++)
  {
    if(receivedBytes[i] != 57)
        return false;
  }
  return true;
}

void allStop()
{
    dutyCycle = 0;
    ledcWrite(rightForward, dutyCycle);
    ledcWrite(leftForward, dutyCycle);
    ledcWrite(rightBackward, dutyCycle);
    ledcWrite(leftBackward, dutyCycle);
}
